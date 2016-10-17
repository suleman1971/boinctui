// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

#include <stdio.h>
#include <algorithm>
#include <stdarg.h>
#ifdef HAVE_OPENSSL
#include <openssl/md5.h>
#else
#include <gnutls/openssl.h>
#endif
#include <unistd.h>
#include "srvdata.h"
#include "resultparse.h"
#include "kclog.h"

#define		STATE_TIME_INTERVAL		2	//интервал в секундах между запросами <get_state>
#define		MSG_TIME_INTERVAL		2	//интервал в секундах между запросами <get_message_count><get_messages>
#define		DISKUSAGE_TIME_INTERVAL		60	//интервал в секундах между запросами <get_disk_usage>
#define		STATISTICS_TIME_INTERVAL	300	//интервал в секундах между запросами <get_statistics>
#define		CCSTATUS_TIME_INTERVAL		600	//интервал в секундах между запросами <get_cc_status>

const int ERR_IN_PROGRESS 	= -204;
const int BOINC_SUCCESS		= 0;


//=============================================================================================

PtrList::~PtrList()
{
    kLogPrintf("PtrList::~PtrList() list.size()=%d\n",list.size());
    while(!list.empty())
    {
	delete list.front();
	list.erase(list.begin());
    }
    pthread_mutex_destroy(&mutex); 
}


Item* PtrList::hookptr() //получить указатель из хвоста списка
{
    Item* result = NULL;
    pthread_mutex_lock(&mutex);
    if (!list.empty())
    {
	list.back()->refcount++;
	result = list.back()->dom;
    }
    pthread_mutex_unlock(&mutex);
    return result;
}


void PtrList::releaseptr(Item* ptr) //сообщить списку что указатель больше не нужен (список сам решит нужно ли его удалять)
{
    std::list<DomPtr*>::iterator it;
    pthread_mutex_lock(&mutex);
    //ищем элемент связанный с ptr
    for( it = list.begin(); it != list.end(); it++)
    {
	if ( (*it)->dom == ptr) //нашли
	{
	    (*it)->refcount--; //уменьшаем счетчик ссылок
	}
    }
    //очищаем старые ненужные эл-ты
    bool done;
    do
    {
	done = true;
	for( it = list.begin(); it != list.end(); it++)
	{
	    if ( ((*it) != list.back()) && ((*it)->refcount <= 0) ) //нашли (последний не трогаем)
	    {
		delete (*it)->dom;
		delete (*it);
		list.erase(it);
		done = false;
		break;
	    }
	}
    }
    while (!done);
    pthread_mutex_unlock(&mutex);
}


//=============================================================================================

bool daily_statisticsCmpAbove( Item* stat1, Item* stat2 ) //для сортировки статистики true если дата stat1 > stat2
{
    Item* day1 = stat1->findItem("day");
    Item* day2 = stat2->findItem("day");

    if ( (day1 != NULL)&&(day2 != NULL) )
    {
	if (day1->getdvalue() > day2->getdvalue())
	    return true;
	else
	    return false;
    }
    return false;
}

//=============================================================================================


SrvList::SrvList(/*Config* cfg*/)
{
//    this->cfg = cfg;
    refreshcfg();
}


SrvList::~SrvList()
{
    kLogPrintf("SrvList::~SrvList() servers.size()=%d\n",servers.size());
    clear();
}


void SrvList::refreshcfg() //перечитать из конфига
{
    clear(); //удаляем старые сервера (если есть)

    Item* boinctui_cfg = gCfg->getcfgptr();
    if (boinctui_cfg == NULL)
	return;
    std::vector<Item*> slist = boinctui_cfg->getItems("server");
    std::vector<Item*>::iterator it;
    for (it = slist.begin(); it != slist.end(); it++)
    {
	Item* host = (*it)->findItem("host");
	Item* port = (*it)->findItem("port");
	Item* pwd = (*it)->findItem("pwd");
	if ((host != NULL)&&(port != NULL))
	{
	    if (pwd == NULL)
		servers.push_back(new Srv(host->getsvalue(), port->getsvalue(), ""));
	    else
		servers.push_back(new Srv(host->getsvalue(), port->getsvalue(), pwd->getsvalue()));
	}
    }
    if (!servers.empty())
    {
	cursrv = servers.begin();
	(*cursrv)->setactive(true);
    }
}


void SrvList::clear() //удалить все соединения
{
    kLogPrintf("SrvList::clear()\n");
    std::list<Srv*>::iterator it;
    cursrv = servers.begin();
    for (it = servers.begin(); it != servers.end(); it++) //чистим все соединения
    {
	kLogPrintf("+delete server\n");
	delete (*it);
	kLogPrintf("-server deleted success\n");
    }
    servers.clear();
}


void SrvList::nextserver() //переключиться на след сервер в списке
{
    (*cursrv)->setactive(false); //деактивируем тред
    cursrv++;
    if (cursrv == servers.end()) //дошли до конца переходим в начало списка
	cursrv = servers.begin();
    (*cursrv)->setactive(true); //активиркем тред
}

//=============================================================================================


Srv::Srv(const char* shost, const char* sport, const char* pwd) : TConnect(shost, sport)
{
    allprojectsdom = NULL;
    this->pwd = strdup(pwd);
    lastmsgno = 0;
    active = false;
    loginfail = false;
    ccstatusdomneedupdate = false;
    pthread_mutex_init(&mutex, NULL);
}


Srv::~Srv()
{
    kLogPrintf("+Srv::~Srv() host=%s:%s\n",shost,sport);
    if (isactive())
    {
	setactive(false); //завершаем опросный тред (если он есть)
	kLogPrintf("waiting stop...\n");
	pthread_join(thread, NULL); //ждем пока тред остановится
    }
    if (allprojectsdom != NULL) delete allprojectsdom;
    if (pwd != NULL) delete pwd;
    pthread_mutex_destroy(&mutex);
    kLogPrintf("-Srv::~Srv()\n");
}


Item* Srv::req(const char* fmt, ...) //выполнить запрос (вернет дерево или NULL)
{
    if (hsock == -1)
	createconnect();
    if (hsock == -1)
	return NULL;
    // === посылаем запрос ===
    char req[1024];
    strcpy(req, "<boinc_gui_rpc_request>\n");
    strcat(req, fmt);
    strcat(req, "\n</boinc_gui_rpc_request>\n\003");
    va_list	args;
    lock();
    va_start(args, fmt);
    sendreq(req, args);
    va_end(args);
    char* result = waitresult();
    unlock();
    if (result != NULL) //получен ответ
    {
	// === отрезаем теги <boinc_gui_rpc_reply> </boinc_gui_rpc_reply>
	const char* teg1 = "<boinc_gui_rpc_reply>";
	const char* teg2 = "</boinc_gui_rpc_reply>";
	char* b = strstr(result,teg1);
	char* e = strstr(result,teg2);
	if (( b == NULL)||( e == NULL))
	{
	    free(result);
	    return NULL;
	}
	*e = '\0';
	b = b + strlen(teg1);
	while ( (*b != '\0')&&(*b != '<') )
	    b++; //отрезаем лидирующие переводы строки и т.д.
	// === костыль ТОЛЬКО для <get_messages>
	if (strstr(fmt, "<get_messages>") != NULL)
	    b =  (char*)stripinvalidtag(b, strlen(b)); //убираем кривые теги
	// === разбираем ответ ===
	lock();
    std::string errmsg;
	Item* dom = xmlparse(b, strlen(b), errmsg); //парсим xml
	unlock();
	free(result); //рез-т в тесктовом виде больше не нужен
	return dom;
    }
    else
	return NULL;
}


void Srv::createconnect()
{
    lock();
    TConnect::createconnect();
    unlock();
    loginfail = false;
    if (hsock != -1)
	loginfail = login();
}


bool Srv::login() //авторизоваться на сервере
{
    bool result = false;
//    if (strlen(pwd) == 0)
//	return true; //пароль не задан (считаем что логин серверу не требуется)
    //получить случайную строку (nonce)
    Item* r1 = req("<auth1/>");
    if (r1 == NULL)
	return result;
    kLogPrintf("login() nonce='%s'\n", r1->toxmlstring().c_str());
    Item* nonce = r1->findItem("nonce");
    if ( nonce == NULL )
    {
	delete r1;
	return result;
    }
    const char* snonce = r1->findItem("nonce")->getsvalue();
    //расчитать хэш md5 от nonce+pwd
    unsigned char md5digest[MD5_DIGEST_LENGTH];
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, snonce, strlen(snonce));
    MD5_Update(&c, pwd , strlen(pwd));
    MD5_Final(md5digest,&c);
    char shash[1024]; //строковое представление хэша
    for (int i=0;i<MD5_DIGEST_LENGTH;i++)
	sprintf(shash+i*2,"%02x",md5digest[i]);
    kLogPrintf("login() md5_hash '%s%s' = %d\n",snonce,pwd,shash);
    //вторая фаза авторизации
    Item* r2 = req("<auth2>\n<nonce_hash>%s</nonce_hash>\n</auth2>",shash);
    kLogPrintf("login() Boinc answer ---\n%s\n", r2->toxmlstring().c_str());
    if ( r2->findItem("unauthorized") != NULL ) //авторизация неуспешна
	result = true;
    delete r2;
    return result;
}


void Srv::updatestatistics()
{
    statisticsdom.addptr(req("<get_statistics/>"));
}


void Srv::updateccstatus()	//обновить состояние <get_cc_status>
{
    ccstatusdom.addptr(req("<get_cc_status/>"));
}


void Srv::updatediskusage()
{
    dusagedom.addptr(req("<get_disk_usage/>"));
}


void Srv::updatestate()
{
    statedom.addptr(req("<get_state/>"));
}


void Srv::updatemsgs() //обновить сообщения
{
    Item* domtree1 = req("<get_message_count/>");
    if (domtree1 == NULL)
	return;
    Item* seqno = domtree1->findItem("seqno");
    if (seqno == NULL)
    {
	delete domtree1;
	return;
    }
    int curseqno = seqno->getivalue(); //кол-во сообщений на сервере в тек. момент
    delete domtree1;
    if (curseqno == lastmsgno) //если с прошлого вызова updatedata() новых сообщений нет, то просто выходим
	return;

    // === запрашиваем содержимое сообщений начиная с lastmsg сервере  ===
    char req2[256];
    int msgno; //номер первого запрашиваемого сообщения
    if (lastmsgno>0)
	msgno = lastmsgno; //запрашиваем начиная с последнего отображенного
    else
	msgno = 0; //начинаем с первого
    snprintf(req2,sizeof(req2),"<get_messages>\n<seqno>%d</seqno>\n",msgno);
    Item* domtree = req(req2);
    if (domtree == NULL)
	return;
    // === дополняем массив визуальных строк ===
    if (!msgdom.empty()) //если есть ранее полученные сообщения
    {
	Item* tmpmsgdom = msgdom.hookptr();
	//объединяем ветку "msgs" новых сообщений с основным деревом (msgdom)
	msgdom.lock();
	(tmpmsgdom->findItem("msgs"))->mergetree(domtree->findItem("msgs"));
	msgdom.unlock();
	delete domtree; //очищаем рез-т
	msgdom.releaseptr(tmpmsgdom);
    }
    else
	msgdom.addptr(domtree);
    lastmsgno = curseqno;
}


void Srv::updateallprojects()
{
    if (allprojectsdom != NULL)
	delete allprojectsdom; //очищаем предыдущий рез-т
    allprojectsdom = req("<get_all_projects_list/>");
}


void Srv::updateacctmgrinfo()//обновить статистику <acct_mgr_info>
{
    acctmgrinfodom.addptr(req("<acct_mgr_info/>"));
}


void Srv::opactivity(const char* op) //изменение режима активности BOINC сервера "always" "auto" "newer"
{
    Item* d = req("<set_run_mode><%s/><duration>0</duration></set_run_mode>",op);
    if (d != NULL)
	delete d;
    ccstatusdomneedupdate = true;
}


void Srv::opnetactivity(const char* op) //изменение режима активности сети "always" "auto" "newer"
{
    Item* d = req("<set_network_mode><%s/><duration>0</duration></set_network_mode>",op);
    if (d != NULL)
	delete d;
    ccstatusdomneedupdate = true;
}


void Srv::opgpuactivity(const char* op) //изменение режима активности GPU "always" "auto" "newer"
{
    Item* d = req("<set_gpu_mode><%s/><duration>0</duration></set_gpu_mode>",op);
    if (d != NULL)
	delete d;
    ccstatusdomneedupdate = true;
}


void Srv::optask(const char* url, const char* name, const char* op) //действия над задачей ("suspend_result",...)
{
    Item* d = req("<%s>\n<project_url>%s</project_url>\n<name>%s</name>\n</%s>",op,url,name,op);
    if (d != NULL)
	delete d;
}


void Srv::opproject(const char* name, const char* op) //действия над проектом ("project_suspend","project_resume",...)
{
    if (statedom.empty())
	return;
    Item* tmpdom = statedom.hookptr();
    std::string url = findProjectUrl(tmpdom,name);
    Item* d = req("<%s>\n<project_url>%s</project_url>\n</%s>",op,url.c_str(),op);
    statedom.releaseptr(tmpdom);
    if (d != NULL)
	delete d;
}


void Srv::runbenchmarks() //запустить бенчмарк
{
    Item* d = req("<run_benchmarks/>");
    if (d != NULL)
	delete d;
}


bool Srv::getprojectconfig(const char* url, std::string& errmsg) //получить c сервера файл конфигурации
{
    //запрос на начало передачи
    Item* res = req("<get_project_config>\n<url>%s</url></get_project_config>\n", url);
    if (res == NULL)
	return false;
    kLogPrintf("request=\n ?\n\n answer=\n%s\n", res->toxmlstring().c_str());
    free(res);
    //ждем завершения
    bool done = false;
    int count = 30; //не больше 30 запросов
    do
    {
	res = req("<get_project_config_poll/>");
	if (res == NULL)
	    return false;
	kLogPrintf("request=\n ?\n\n answer=\n%s\n", res->toxmlstring().c_str());
	Item* error_num = res->findItem("error_num");
	if (error_num != NULL)
	{
	    int errnum = error_num->getivalue();
	    if (errnum == ERR_IN_PROGRESS) //ждать?
	        sleep(1); //ERR_IN_PROGRESS ждем 1 сек
	    else
	    {
	        free(res);
	        break;
	    }
	}
	else
	{
	    Item* project_config = res->findItem("project_config");
	    if (project_config != NULL)
		done = true;
	    else
	    {
		free(res);
		break;
	    }
	}
	free(res);
    }
    while((count--)&&(!done));
    return done;
}


bool Srv::createaccount(const char* url, const char* email, const char* pass, const char* username, const char* teamname, std::string& errmsg) //создать аккаунт
{
    kLogPrintf("createaccount(url=%s, email=%s, pass=%s, username=%s, teamname=%s)\n", url, email, pass, username, teamname);

    //расчитать хэш md5 от pass+email
    unsigned char md5digest[MD5_DIGEST_LENGTH];
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, pass , strlen(pass));
    MD5_Update(&c, email, strlen(email));
    MD5_Final(md5digest,&c);
    char shash[1024]; //строковое представление хэша
    for (int i=0;i<MD5_DIGEST_LENGTH;i++)
	sprintf(shash+i*2,"%02x",md5digest[i]);
    //формируем запрос для создания аккаунта
    char sreq[1024];
    snprintf(sreq,sizeof(sreq),
        "<create_account>\n"
        "   <url>%s</url>\n"
        "   <email_addr>%s</email_addr>\n"
        "   <passwd_hash>%s</passwd_hash>\n"
        "   <user_name>%s</user_name>\n"
        "   <team_name>%s</team_name>\n"
        "</create_account>\n",
	url,email,shash,username,teamname);
    Item* res = req(sreq);
    if (res == NULL)
	return false;
    kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
    free(res);
    //ждем завершения
    int count = 30; //не больше 30 запросов
    snprintf(sreq,sizeof(sreq),"<create_account_poll/>");
    bool done = false;
    do
    {
	res = req(sreq);
	if (res == NULL)
	    return false;
	kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
	Item* error_num = res->findItem("error_num");
	if ((error_num != NULL)&&(error_num->getivalue() != ERR_IN_PROGRESS))
	{
	    Item* error_msg = res->findItem("error_msg");
	    if (error_msg != NULL)
	    	errmsg = error_msg->getsvalue(); //возврат строки ошибки
	    return false;
	}
	if (res->findItem("authenticator") != NULL)
	    done = true;
	free(res);
	sleep(1); //ждем 1 сек
    }
    while((count--)&&(!done));
    return true;
}


bool Srv::projectattach(const char* url, const char* prjname, const char* email, const char* pass, std::string& errmsg) //подключить проект
{
    //расчитать хэш md5 от pass+email
    unsigned char md5digest[MD5_DIGEST_LENGTH];
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, pass , strlen(pass));
    MD5_Update(&c, email, strlen(email));
    MD5_Final(md5digest,&c);
    char shash[1024]; //строковое представление хэша
    for (int i=0;i<MD5_DIGEST_LENGTH;i++)
	sprintf(shash+i*2,"%02x",md5digest[i]);
    //формируем запрос для получения authenticator
    char sreq[1024];
    snprintf(sreq,sizeof(sreq),"<lookup_account>\n<url>%s</url>\n<email_addr>%s</email_addr>\n<passwd_hash>%s</passwd_hash>\n</lookup_account>\n",url,email,shash);
    Item* res = req(sreq);
    if (res == NULL)
	return false;
    kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
    free(res);
    int count = 30; //не больше 30 запросов
    snprintf(sreq,sizeof(sreq),"<lookup_account_poll/>");
    std::string sauthenticator;
    bool done = false;
    do
    {
	res = req(sreq);
	if (res == NULL)
	    return false;
	kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
	Item* error_num = res->findItem("error_num");
	if ((error_num != NULL)&&(error_num->getivalue() != ERR_IN_PROGRESS))
	{
	    Item* error_msg = res->findItem("error_msg");
	    if (error_msg != NULL)
		errmsg = error_msg->getsvalue(); //возврат строки ошибки
	    return false;
	}
	Item* authenticator  = res->findItem("authenticator");
	if (authenticator != NULL)
	{
	    sauthenticator = authenticator->getsvalue();
	    done = true;
	}
	free(res);
	sleep(1); //ждем 1 сек
    }
    while((count--)&&(!done));
    if (!done)
	return false;
    //формируем запрос для подключения к проекту
    snprintf(sreq,sizeof(sreq),"<project_attach>\n<project_url>%s</project_url>\n<authenticator>%s</authenticator>\n<project_name>%s</project_name>\n</project_attach>\n",url,sauthenticator.c_str(),prjname);
    res = req(sreq);
    if (res == NULL)
	return false;
    kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
    bool result = (res->findItem("success") != NULL);
    free(res);
    return result;
}


bool Srv::accountmanager(const char* url, const char* username, const char* pass, bool useconfigfile, std::string& errmsg) //подключить аккаунт менеджер
{
    if (strlen(url) > 0)
    {
	//получить конфиг (не знаю зачем!!!)
	if (!getprojectconfig(url,errmsg))
	{
	    errmsg = "Can't get config";
	    return false;
	}
    }
    char sreq[1024];
    //формируем запрос
    if (useconfigfile)
	snprintf(sreq,sizeof(sreq),"<acct_mgr_rpc>\n<use_config_file/>\n</acct_mgr_rpc>\n");
    else
	snprintf(sreq,sizeof(sreq),"<acct_mgr_rpc>\n<url>%s</url>\n<name>%s</name>\n<password>%s</password>\n</acct_mgr_rpc>\n",url,username,pass);
    Item* res = req(sreq);
    if (res == NULL)
	return false;
    kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
    //ждем завершения
    snprintf(sreq,sizeof(sreq),"<acct_mgr_rpc_poll/>");
    bool done = false;
    int count = 30; //не больше 30 запросов
    do
    {
	res = req(sreq);
	if (res == NULL)
	    return false;
	kLogPrintf("request=\n %s\n\n answer=\n%s\n",sreq, res->toxmlstring().c_str());
	Item* error_num = res->findItem("error_num");
	if (error_num != NULL)
	{
	    int errnum = error_num->getivalue();
	    if (errnum == BOINC_SUCCESS) //успешно
		done = true;
	    else
		if (errnum != ERR_IN_PROGRESS) //ошибка выходим
		{
		    Item* message = res->findItem("message");
		    if (message != NULL)
	    		errmsg = message->getsvalue(); //возврат строки ошибки
		    free(res);
		    return false;
		}
		else
		    sleep(1); //ERR_IN_PROGRESS ждем 1 сек
	}
	free(res);
    }
    while((count--)&&(!done));
    acctmgrinfodom.needupdate = true; //чтобы тред обновил данные
    sleep(1); //даем треду 1 сек на обновление
    kLogPrintf("RET %b\n",done);
    return done;
}


std::string Srv::findProjectName(Item* tree, const char* url) //найти в дереве tree имя проекта с заданным url
{
    Item* client_state = tree->findItem("client_state");
    if (client_state == NULL)
	return "ERR2";
    std::vector<Item*> projects = client_state->getItems("project"); //список проектов
    std::vector<Item*>::iterator it;
    for (it = projects.begin(); it!=projects.end(); it++)
    {
 	Item* master_url = (*it)->findItem("master_url");
	if ( strcmp(url,master_url->getsvalue()) == 0 ) //url совпали НАШЛИ!
	{
	    Item* project_name = (*it)->findItem("project_name");
	    return project_name->getsvalue();
	}
    }
    return "???";
}


std::string Srv::findProjectUrl(Item* tree, const char* name) //найти в дереве tree url проекта с заданным именем
{
    Item* client_state = tree->findItem("client_state");
    if (client_state == NULL)
	return "ERR2";
    std::vector<Item*> projects = client_state->getItems("project"); //список проектов
    std::vector<Item*>::iterator it;
    for (it = projects.begin(); it!=projects.end(); it++)
    {
 	Item* project_name = (*it)->findItem("project_name");
	if ( strcmp(name, project_name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    Item* master_url = (*it)->findItem("master_url");
	    return master_url->getsvalue();
	}
    }
    return "";
}


Item* Srv::findresultbyname(const char* resultname)
{
    if (statedom.empty())
	return NULL;
    if (resultname == NULL)
	return NULL;
    Item* tmpstatedom = statedom.hookptr();
    Item* client_state = tmpstatedom->findItem("client_state");
    if (client_state == NULL)
    {
	statedom.releaseptr(tmpstatedom);
	return NULL;
    }
    std::vector<Item*> results = client_state->getItems("result"); //список задач
    std::vector<Item*>::iterator it;
    for (it = results.begin(); it!=results.end(); it++)
    {
	Item* name = (*it)->findItem("name");
	if ( strcmp(resultname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    statedom.releaseptr(tmpstatedom);
	    return (*it);
	}
    }
    statedom.releaseptr(tmpstatedom);
    return NULL;
}


Item* Srv::findprojectbyname(const char* projectname)
{
    if (statedom.empty())
	return NULL;
    if (projectname == NULL)
	return NULL;
    Item* tmpstatedom = statedom.hookptr();
    Item* client_state = tmpstatedom->findItem("client_state");
    if (client_state == NULL)
    {
	statedom.releaseptr(tmpstatedom);
	return NULL;
    }
    std::vector<Item*> projects = client_state->getItems("project"); //список проектов
    std::vector<Item*>::iterator it;
    for (it = projects.begin(); it!=projects.end(); it++)
    {
	Item* name = (*it)->findItem("project_name");
	if ( strcmp(projectname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    statedom.releaseptr(tmpstatedom);
	    return (*it);
	}
    }
    statedom.releaseptr(tmpstatedom);
    return NULL;
}


Item* Srv::findprojectbynamefromall(const char* projectname) //ищет в allprojectsdom
{
    if (allprojectsdom == NULL)
	return NULL;
    if (projectname == NULL)
	return NULL;
    Item* projects = allprojectsdom->findItem("projects");
    if (projects == NULL)
	return NULL;
    std::vector<Item*> projectlist = projects->getItems("project"); //список проектов
    std::vector<Item*>::iterator it;
    for (it = projectlist.begin(); it!=projectlist.end(); it++)
    {
	Item* name = (*it)->findItem("name");
	if ( strcmp(projectname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    return (*it);
	}
    }
    return NULL;
}


Item* Srv::findaccountmanager(const char* mgrname) //ищет менеджер по имени
{
    if (allprojectsdom == NULL)
	return NULL;
    if (mgrname == NULL)
	return NULL;
    Item* projects = allprojectsdom->findItem("projects");
    if (projects == NULL)
	return NULL;
    std::vector<Item*> mgrlist = projects->getItems("account_manager"); //список менеджеров
    std::vector<Item*>::iterator it;
    for (it = mgrlist.begin(); it!=mgrlist.end(); it++)
    {
	Item* name = (*it)->findItem("name");
	if ( strcmp(mgrname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    return (*it);
	}
    }
    return NULL;
}


Item* Srv::findappbywuname(const char* wuname) //найти приложение для данного WU
{
    if (statedom.empty())
	return NULL;
    if (wuname == NULL)
	return NULL;
    Item* tmpstatedom = statedom.hookptr();
    Item* client_state = tmpstatedom->findItem("client_state");
    if (client_state == NULL)
    {
	statedom.releaseptr(tmpstatedom);
	return NULL;
    }
    std::vector<Item*> wulist = client_state->getItems("workunit"); //список WU
    std::vector<Item*>::iterator it;
    //ищем WU
    for (it = wulist.begin(); it!=wulist.end(); it++)
    {
	Item* name = (*it)->findItem("name");
	if ( strcmp(wuname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ WU!
	{
	    Item* app_name = (*it)->findItem("app_name");
	    std::vector<Item*> applist = client_state->getItems("app"); //список APP
	    std::vector<Item*>::iterator it2;
	    for (it2 = applist.begin(); it2!=applist.end(); it2++)
	    {
		Item* name = (*it2)->findItem("name");
		if ( strcmp(app_name->getsvalue(),name->getsvalue()) == 0 ) //имена совпали НАШЛИ APP!
		{
		    statedom.releaseptr(tmpstatedom);
		    return (*it2);
		}
	    }
	}
    }
    statedom.releaseptr(tmpstatedom);
    return NULL;
}


time_t	Srv::getlaststattime() //вернет время последней имеющейся статистики
{
    time_t result = 0;
    if (statisticsdom.empty())
	return 0;
    Item* tmpstatisticsdom = statisticsdom.hookptr();
    Item* statistics = tmpstatisticsdom->findItem("statistics");
    if (statistics!=NULL)
    {
	std::vector<Item*> project_statistics = statistics->getItems("project_statistics");
	std::vector<Item*>::iterator it;
	for (it = project_statistics.begin(); it!=project_statistics.end(); it++) //цикл списка проектов
	{
	    std::vector<Item*> daily_statistics = (*it)->getItems("daily_statistics"); //все дни проекта в этом векторе
	    if (!daily_statistics.empty())
	    {
		std::sort(daily_statistics.begin(), daily_statistics.end(), daily_statisticsCmpAbove); //сортируем по убыванию дат
		Item* lastday = daily_statistics.front(); //последний день должен быть сверху
		double d = lastday->findItem("day")->getdvalue();
		if (d > result)
		    result = d;
	    }
	} //проекты
    }
    statisticsdom.releaseptr(tmpstatisticsdom);
    return result;
}


time_t Srv::gettimeelapsed(time_t t) //вернет соличество секунд между t и тек. временем
{
    return (time(NULL) - t);
}


void* Srv::updatethread(void* args) //трейд опрашивающий сервер
{
    Srv* me = (Srv*)args;
    kLogPrintf("%s:%s::updatethread() started\n",me->gethost(),me->getport());
    me->takt = 0;
    while(me->active)
    {
	//get data from remote server
	if ( me->statedom.empty() || ( (me->takt % STATE_TIME_INTERVAL) == 0 ) )
	    me->updatestate(); //<get_state>
	if (me->isconnected()&&(!me->loginfail)) //если нет коннекта то пропускаем прочие запросы
	{
	    if ( me->msgdom.empty() || ( (me->takt % MSG_TIME_INTERVAL) == 0 ) )
		me->updatemsgs(); //<get_message_count>/<get_messages>
	    if ( me->statisticsdom.empty() || ( (me->takt % STATISTICS_TIME_INTERVAL) == 0 ) )
		me->updatestatistics(); //<get_statistics>
	    if ( me->dusagedom.empty() || ( (me->takt % DISKUSAGE_TIME_INTERVAL) == 0 ) )
		me->updatediskusage(); //<get_disk_usage>
	    if ( me->ccstatusdom.empty() || ( (me->takt % CCSTATUS_TIME_INTERVAL) == 0 ) || me->ccstatusdomneedupdate )
	    {
		me->updateccstatus(); //<get_cc_status>
		me->ccstatusdomneedupdate = false;
	    }
	    if (me->acctmgrinfodom.needupdate)
		me->updateacctmgrinfo(); //ин-я по аккаунт менеджеру
	}
	//спим 1 секунду проверяя me->ccstatusdomneedupdate
	for (int i = 0; i < 10; i++)
	{
	    usleep(100000); //100 milisec
	    if (me->ccstatusdomneedupdate)
		break; //прервать сон если нужен срочный апдейт
	}
	me->takt++;
    }
    kLogPrintf("%s:%s::updatethread() stopped\n",me->gethost(),me->getport());
    return NULL;
}


void  Srv::setactive(bool b) //включить/выключить тред обновления данных
{
    if (isactive() != b)
    {
	if (b)
	{
	    active = true;
	    if ( 0 != pthread_create(&thread, NULL, updatethread, this))
		kLogPrintf("pthread_create() error\n");
	}
	else
	{
	    active = false; //сигнализирует треду остановиться
	}
    }
}

