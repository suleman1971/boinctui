#include <stdio.h>
#include <algorithm>
#include "srvdata.h"
#include "resultparse.h"



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
	Item* ip = (*it)->findItem("ip");
	Item* port = (*it)->findItem("port");
	if ((ip != NULL)&&(port != NULL))
	{
	    servers.push_back(new Srv(ip->getsvalue(),port->getsvalue()));
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
}


Item* Srv::req(const char* op) //выполнить запрос (вернет дерево или NULL)
{
    // === посылаем запрос ===
    //char r[256];
    //snprintf(r,sizeof(r),"<boinc_gui_rpc_request>\n%s\n</boinc_gui_rpc_request>\n\003",op);
    sendreq("<boinc_gui_rpc_request>\n%s\n</boinc_gui_rpc_request>\n\003",op);
    char* result = waitresult();
    if (result != NULL) //получен ответ
    {
	// === костыль ТОЛЬКО для <get_messages>
	if (strstr(op, "<get_messages>") != NULL)
	    result =  (char*)stripinvalidtag(result, strlen(result)); //убираем кривые теги
	// === разбираем ответ ===
	Item* dom = xmlparse(result, strlen(result)); //парсим xml
	free(result); //рез-т в тесктовом виде больше не нужен
	return dom;
    }
    else
	return NULL;
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


void Srv::resumetask(Item* result) //возобновить задачу
{
    Item* name = result->findItem("name");
    Item* project_url = result->findItem("project_url");
    if ((name == NULL) || (project_url == NULL))
	return;
    if (result->findItem("active_task") == NULL)
	return; //меняем состояние только для активных
    //char req[1024];
    //snprintf(req, sizeof(req), "<boinc_gui_rpc_request>\n<resume_result>\n<project_url>%s</project_url>\n<name>%s</name>\n</resume_result>\n</boinc_gui_rpc_request>\n\003",project_url->getsvalue(),name->getsvalue());
    sendreq("<boinc_gui_rpc_request>\n<resume_result>\n<project_url>%s</project_url>\n<name>%s</name>\n</resume_result>\n</boinc_gui_rpc_request>\n\003",project_url->getsvalue(),name->getsvalue());
    char* s = waitresult();
    free(s); //результат не проверяем
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

