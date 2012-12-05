#include <stdio.h>
#include <algorithm>
#include <stdarg.h>
#include <openssl/md5.h>
#include "srvdata.h"
#include "resultparse.h"
#include "kclog.h"


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


Srv::~Srv()
{
    if (msgdom != NULL) delete msgdom;
    if (statedom != NULL) delete statedom;
    if (dusagedom != NULL) delete dusagedom;
    if (statisticsdom != NULL) delete statisticsdom;
    if (pwd != NULL) delete pwd;
}


//Item* Srv::req(const char* op) //выполнить запрос (вернет дерево или NULL)
Item* Srv::req(const char* fmt, ...) //выполнить запрос (вернет дерево или NULL)
{
    if (hsock == -1)
	createconnect();
    if (hsock == -1)
	return NULL;
    // === посылаем запрос ===
    char req[1024];
//    snprintf(r,sizeof(r),"<boinc_gui_rpc_request>\n%s\n</boinc_gui_rpc_request>\n\003",op);
    strcpy(req, "<boinc_gui_rpc_request>\n");
    strcat(req, fmt);
    strcat(req, "\n</boinc_gui_rpc_request>\n\003");
    va_list	args;
    va_start(args, fmt);
//    sendreq("<boinc_gui_rpc_request>\n%s\n</boinc_gui_rpc_request>\n\003", fmt, args);
    sendreq(req, args);
    va_end(args);
    //sendreq("<boinc_gui_rpc_request>\n%s\n</boinc_gui_rpc_request>\n\003",op);
    char* result = waitresult();
    if (result != NULL) //получен ответ
    {
	// === костыль ТОЛЬКО для <get_messages>
	if (strstr(/*op*/fmt, "<get_messages>") != NULL)
	    result =  (char*)stripinvalidtag(result, strlen(result)); //убираем кривые теги
	// === разбираем ответ ===
	Item* dom = xmlparse(result, strlen(result)); //парсим xml
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
    if (statisticsdom != NULL)
	delete statisticsdom; //очищаем предыдущий рез-т
    statisticsdom = req("<get_statistics/>");
}


void Srv::updatediskusage()
{
    if (dusagedom != NULL)
	delete dusagedom; //очищаем предыдущий рез-т
    dusagedom = req("<get_disk_usage/>");
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
    sprintf(req2,"<get_messages>\n<seqno>%d</seqno>\n",msgno);
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

/*
void Srv::suspendtask(Item* result) //приостановить задачу
{
    Item* name = result->findItem("name");
    Item* project_url = result->findItem("project_url");
    if ((name == NULL) || (project_url == NULL))
	return;
    if (result->findItem("active_task") == NULL)
	return; //меняем состояние только для активных
    //char req[1024];
    //snprintf(req, sizeof(req), "<boinc_gui_rpc_request>\n<suspend_result>\n<project_url>%s</project_url>\n<name>%s</name>\n</suspend_result>\n</boinc_gui_rpc_request>\n\003",project_url->getsvalue(),name->getsvalue());
    sendreq("<boinc_gui_rpc_request>\n<suspend_result>\n<project_url>%s</project_url>\n<name>%s</name>\n</suspend_result>\n</boinc_gui_rpc_request>\n\003",project_url->getsvalue(),name->getsvalue());
    char* s = waitresult();
    free(s); //результат не проверяем
}
*/

void Srv::optask(Item* result, const char* op) //действия над задачей ("suspend_result",...)
//void Srv::resumetask(Item* result) //возобновить задачу
{
    Item* name = result->findItem("name");
    Item* project_url = result->findItem("project_url");
    if ((name == NULL) || (project_url == NULL))
	return;
    if (result->findItem("active_task") == NULL)
	return; //меняем состояние только для активных
    sendreq("<boinc_gui_rpc_request>\n<%s>\n<project_url>%s</project_url>\n<name>%s</name>\n</%s>\n</boinc_gui_rpc_request>\n\003",op,project_url->getsvalue(),name->getsvalue(),op);
    char* s = waitresult();
    free(s); //результат не проверяем
}


void  Srv::opproject(const char* name, const char* op) //действия над проектом ("project_suspend","project_resume",...)
//void Srv::suspendproject(const char* name) //приостановить проект
{
    if (statedom == NULL)
	return;
    std::string url = findProjectUrl(statedom,name);
    sendreq("<boinc_gui_rpc_request>\n<%s>\n<project_url>%s</project_url>\n</%s>\n</boinc_gui_rpc_request>\n\003",op,url.c_str(),op);
    char* s = waitresult();
    free(s); //результат не проверяем
}

/*
void Srv::resumeproject(const char* name) //продолжить проект
{
    if (statedom == NULL)
	return;
    std::string url = findProjectUrl(statedom,name);
    sendreq("<boinc_gui_rpc_request>\n<project_resume>\n<project_url>%s</project_url>\n</project_resume>\n</boinc_gui_rpc_request>\n\003",url.c_str());
    char* s = waitresult();
    free(s); //результат не проверяем
}
*/

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
    std::vector<Item*> results = client_state->getItems("result"); //список проектов
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

