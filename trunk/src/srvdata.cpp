#include <stdio.h>
#include <algorithm>
#include <stdarg.h>
#include <openssl/md5.h>
#include "srvdata.h"
#include "resultparse.h"
#include "kclog.h"

#define		DISKUSAGE_TIME_INTERVAL		60	//интервал в секундах между запросами <get_disk_usage>
#define		STATISTICS_TIME_INTERVAL	300	//интервал в секундах между запросами <get_statistics>

const int ERR_IN_PROGRESS = -204;



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


SrvList::SrvList(Config* cfg)
{
    this->cfg = cfg;
    refreshcfg();
}


SrvList::~SrvList()
{
    clear();
}


void SrvList::refreshcfg() //перечитать из конфига
{
    clear(); //удаляем старые сервера (если есть)

    Item* boinctui_cfg = cfg->getcfgptr();
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
	cursrv = servers.begin();
}


void SrvList::clear() //удалить все соединения
{
    std::list<Srv*>::iterator it;
    cursrv = servers.begin();
    for (it = servers.begin(); it != servers.end(); it++) //чистим все соединения
    {
	delete (*it);
    }
    servers.clear();
}


void SrvList::nextserver() //переключиться на след сервер в списке
{
    cursrv++;
    if (cursrv == servers.end()) //дошли до конца переходим в начало списка
	cursrv = servers.begin();
}

//=============================================================================================


Srv::Srv(const char* shost, const char* sport, const char* pwd) : TConnect(shost, sport)
{
    msgdom = statedom = dusagedom = statisticsdom = allprojectsdom = ccstatusdom = NULL;
    this->pwd = strdup(pwd);
    lastmsgno = 0;
    diskusagetstamp = 0;
};


Srv::~Srv()
{
    if (msgdom != NULL) delete msgdom;
    if (statedom != NULL) delete statedom;
    if (dusagedom != NULL) delete dusagedom;
    if (ccstatusdom != NULL) delete ccstatusdom;
    if (statisticsdom != NULL) delete statisticsdom;
    if (pwd != NULL) delete pwd;
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
    va_start(args, fmt);
    sendreq(req, args);
    va_end(args);
    char* result = waitresult();
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
	Item* dom = xmlparse(b, strlen(b)); //парсим xml
	free(result); //рез-т в тесктовом виде больше не нужен
	return dom;
    }
    else
	return NULL;
}


void Srv::createconnect()
{
    TConnect::createconnect();
    if (hsock != -1)
	login();
}


bool Srv::login() //авторизоваться на сервере
{
    bool result = false;
    if (strlen(pwd) == 0)
	return true; //пароль не задан (считаем что логин серверу не требуется)
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
    if ( r2->findItem("authorize") != NULL ) //авторизация успешна
	result = true;
    delete r2;
    return result;
}


void Srv::updatedata() //обновить данные с сервера
{
    updatemsgs(); //обновить сообщения
    updatestate(); //состояние
    updatediskusage(); //disk_usage
}


void Srv::updatestatistics()
{
    if (gettimeelapsed(statisticststamp) < STATISTICS_TIME_INTERVAL)
	return; //если прошло мало времени игнорируем вызов
    if (statisticsdom != NULL)
	delete statisticsdom; //очищаем предыдущий рез-т
    statisticsdom = req("<get_statistics/>");
    statisticststamp = time(NULL);
}


void Srv::updateccstatus()	//обновить состояние <get_cc_status>
{
    if (ccstatusdom != NULL)
	delete ccstatusdom; //очищаем предыдущий рез-т
    ccstatusdom = req("<get_cc_status/>");
}


void Srv::updatediskusage()
{
    if (gettimeelapsed(diskusagetstamp) < DISKUSAGE_TIME_INTERVAL)
	return; //если прошло мало времени игнорируем вызов
    if (dusagedom != NULL)
	delete dusagedom; //очищаем предыдущий рез-т
    dusagedom = req("<get_disk_usage/>");
    diskusagetstamp = time(NULL);
}


void Srv::updatestate()
{
    if (statedom != NULL)
	delete statedom; //очищаем предыдущий рез-т
    statedom = req("<get_state/>");
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
    if (msgdom != NULL) //если есть ранее полученные сообщения
    {
	//объединяем ветку "msgs" новых сообщений с основным деревом (msgdom)
	(msgdom->findItem("msgs"))->mergetree(domtree->findItem("msgs"));
	delete domtree; //очищаем рез-т
    }
    else
	msgdom = domtree;
    lastmsgno = curseqno;
}


void Srv::updateallprojects()
{
    if (allprojectsdom != NULL)
	delete allprojectsdom; //очищаем предыдущий рез-т
    allprojectsdom = req("<get_all_projects_list/>");
}


void Srv::opactivity(const char* op) //изменение режима активности BOINC сервера "always" "auto" "newer"
{
    sendreq("<boinc_gui_rpc_request>\n<set_run_mode><%s/><duration>0</duration></set_run_mode>\n</boinc_gui_rpc_request>\n\003",op);
    char* s = waitresult();
    free(s); //результат не проверяем
}


void Srv::opnetactivity(const char* op) //изменение режима активности сети "always" "auto" "newer"
{
    sendreq("<boinc_gui_rpc_request>\n<set_network_mode><%s/><duration>0</duration></set_network_mode>\n</boinc_gui_rpc_request>\n\003",op);
    char* s = waitresult();
    free(s); //результат не проверяем
}


void Srv::opgpuactivity(const char* op) //изменение режима активности GPU "always" "auto" "newer"
{
    sendreq("<boinc_gui_rpc_request>\n<set_gpu_mode><%s/><duration>0</duration></set_gpu_mode>\n</boinc_gui_rpc_request>\n\003",op);
    char* s = waitresult();
    free(s); //результат не проверяем
}


void Srv::optask(Item* result, const char* op) //действия над задачей ("suspend_result",...)
{
    Item* name = result->findItem("name");
    Item* project_url = result->findItem("project_url");
    if ((name == NULL) || (project_url == NULL))
	return;
    if ((strcmp(op,"suspend_result")==0)||(strcmp(op,"resume_result")==0))
    {
	if (result->findItem("active_task") == NULL)
	    return; //меняем состояние только для активных
    }
    sendreq("<boinc_gui_rpc_request>\n<%s>\n<project_url>%s</project_url>\n<name>%s</name>\n</%s>\n</boinc_gui_rpc_request>\n\003",op,project_url->getsvalue(),name->getsvalue(),op);
    char* s = waitresult();
    free(s); //результат не проверяем
}


void  Srv::opproject(const char* name, const char* op) //действия над проектом ("project_suspend","project_resume",...)
{
    if (statedom == NULL)
	return;
    std::string url = findProjectUrl(statedom,name);
    sendreq("<boinc_gui_rpc_request>\n<%s>\n<project_url>%s</project_url>\n</%s>\n</boinc_gui_rpc_request>\n\003",op,url.c_str(),op);
    char* s = waitresult();
    free(s); //результат не проверяем
}


void Srv::runbenchmarks() //запустить бенчмарк
{
    sendreq("<boinc_gui_rpc_request>\n<run_benchmarks/>\n</boinc_gui_rpc_request>\n\003");
    char* s = waitresult();
    free(s); //результат не проверяем
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
	if ((error_num != 0)&&(error_num->getivalue() != ERR_IN_PROGRESS))
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
	if ((error_num != 0)&&(error_num->getivalue() != ERR_IN_PROGRESS))
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
    if (statedom == NULL)
	return NULL;
    if (resultname == NULL)
	return NULL;
    Item* client_state = statedom->findItem("client_state");
    if (client_state == NULL)
	return NULL;
    std::vector<Item*> results = client_state->getItems("result"); //список задач
    std::vector<Item*>::iterator it;
    for (it = results.begin(); it!=results.end(); it++)
    {
	Item* name = (*it)->findItem("name");
	if ( strcmp(resultname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    return (*it);
	}
    }
    return NULL;
}


Item* Srv::findprojectbyname(const char* projectname)
{
    if (statedom == NULL)
	return NULL;
    if (projectname == NULL)
	return NULL;
    Item* client_state = statedom->findItem("client_state");
    if (client_state == NULL)
	return NULL;
    std::vector<Item*> projects = client_state->getItems("project"); //список проектов
    std::vector<Item*>::iterator it;
    for (it = projects.begin(); it!=projects.end(); it++)
    {
	Item* name = (*it)->findItem("project_name");
	if ( strcmp(projectname,name->getsvalue()) == 0 ) //имена совпали НАШЛИ!
	{
	    return (*it);
	}
    }
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


time_t	Srv::getlaststattime() //вернет время последней имеющейся статистики
{
    time_t result = 0;
    if (statisticsdom == NULL)
	return 0;
    Item* statistics = statisticsdom->findItem("statistics");
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
    return result;
}


time_t Srv::gettimeelapsed(time_t t) //вернет соличество секунд между t и тек. временем
{
    return (time(NULL) - t);
}
