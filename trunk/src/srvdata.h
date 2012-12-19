#ifndef SRVDATA_H
#define SRVDATA_H

#include <string.h>
#include <stdlib.h>
#include <list>
#include "net.h"
#include "resultdom.h"
#include "cfg.h"


bool daily_statisticsCmpAbove( Item* stat1, Item* stat2 ); //для сортировки статистики true если дата stat1 > stat2


class Srv : public TConnect //описание соединения с сервером
{
  public:
    Srv(const char* shost, const char* sport, const char* pwd) : TConnect(shost, sport) { msgdom = statedom = dusagedom = statisticsdom = ccstatusdom = NULL; this->pwd = strdup(pwd); lastmsgno = 0; };
    virtual ~Srv();
    void updatemsgs();		//обновить список сообщений <get_messages>
    void updatestate();		//обновить состояние <get_state>
    void updatediskusage();	//обновить состояние <get_disk_usage>
    void updateccstatus();	//обновить состояние <get_cc_status>
    void updatestatistics();	//обновить статистику <get_statistics>
    std::string findProjectName(Item* tree, const char* url); //найти в дереве tree имя проекта с заданным url
    std::string findProjectUrl(Item* tree, const char* name); //найти в дереве tree url проекта с заданным именем
    Item* findresultbyname(const char* resultname);
    Item* findprojectbyname(const char* projectname);
    void  opactivity(const char* op); //изменение режима активности BOINC сервера "always" "auto" "newer"
    void  opnetactivity(const char* op); //изменение режима активности сети "always" "auto" "newer"
    void  optask(Item* result, const char* op); //действия над задачей ("suspend_result",...)
    void  opproject(const char* name, const char* op); //действия над проектом ("project_suspend","project_resume",...)
    void  runbenchmarks(); //запустить бенчмарк
    time_t	getlaststattime(); //вернет время последней имеющейся статистики
    Item*	msgdom; 	//xml дерево сообщений
    int		lastmsgno; 	//номер последнего сообщения полученного с сервера
    Item*	statedom; 	//xml дерево состояний
    Item*	ccstatusdom;	//xml дерево для <get_cc_status>
    Item*	dusagedom;	//xml дерево для <get_disk_usage>
    Item*	statisticsdom;	//xml дерево для <get_statistics>
    Item* req(const char* fmt, ...);  //выполнить запрос (вернет дерево или NULL)
    bool  login(); 		//авторизоваться на сервере
    virtual void  createconnect();
  protected:
    void updatedata();		//обновить данные с сервера
    char* pwd;
    //Item* req(const char* op);  //выполнить запрос (вернет дерево или NULL)
};


class SrvList //список всех серверов
{
  public:
    SrvList(Config* cfg);
    virtual ~SrvList();
    //Srv* addserver(const char* shost, const char* sport) { return new Srv(shost, sport); }; //добавить сервер (вернет указаталь на сервер)
    void requestdata() {}; //опросить все сервера
    //Srv* findserver(const char* shost, const char* sport) { return servers.front(); }; //ЗАГЛУШКА здесь нужен поиск по аресу проту
    Srv* getcursrv() { if (cursrv != servers.end()) return (*cursrv); else return NULL; }; //получить текущий сервер или NULL
    void refreshcfg(); //перечитать из конфига
    void clear(); //удалить все соединения
    void nextserver(); //переключиться на след сервер в списке
  protected:
    void addserver(Srv* c) { servers.push_back(c); };
    std::list<Srv*> servers; //список соединений
    std::list<Srv*>::iterator cursrv; //текущиq сервер
    Config*	cfg;
};


//extern SrvList* gsrvlist; //глобальный список серверов


#endif //SRVDATA_H