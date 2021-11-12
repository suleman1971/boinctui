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

#ifndef SRVDATA_H
#define SRVDATA_H

#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include "net.h"
#include "resultdom.h"
#include "cfg.h"


struct DomPtr
{
    DomPtr(Item* ptr) {refcount = 0; dom = ptr; };
    Item* dom; //указатель на данные
    int   refcount; //количество hook-ов данного указателя
};


class PtrList
{
  public:
    PtrList() { pthread_mutex_init(&mutex, NULL); needupdate = true; };
    ~PtrList();
    void lock() { pthread_mutex_lock(&mutex); };
    void unlock() { pthread_mutex_unlock(&mutex); };
    Item* hookptr(); //получить указатель из хвоста списка
    void releaseptr(Item* ptr); //сообщить списку что указатель больше не нужен (список сам решит нужно ли его удалять)
    void addptr(Item* ptr) { pthread_mutex_lock(&mutex); if (ptr != NULL) list.push_back(new DomPtr(ptr)); needupdate = false; pthread_mutex_unlock(&mutex);}; //добавить указатель в хвост списка
    bool empty() { pthread_mutex_lock(&mutex); bool result = list.empty(); pthread_mutex_unlock(&mutex); return result; };
    bool needupdate; //говорит треду что нужно незамедлительно обновлять данные
  private:
    std::list<DomPtr*> list;
    pthread_mutex_t	mutex;
};


bool daily_statisticsCmpAbove( Item* stat1, Item* stat2 ); //для сортировки статистики true если дата stat1 > stat2


class Srv : public TConnect //описание соединения с сервером
{
  public:
    Srv(const char* shost, const char* sport, const char* pwd, const char* hostid);
    virtual ~Srv();
    void updateallprojects();	//обновить статистику <get_all_projects_list>
    static std::string findProjectName(Item* tree, const char* url); //найти в дереве tree имя проекта с заданным url
    std::string findProjectUrl(Item* tree, const char* name); //найти в дереве tree url проекта с заданным именем
    Item* findresultbyname(const char* resultname);
    Item* findprojectbyname(const char* projectname); //ищет в getstate
    Item* findprojectbynamefromall(const char* projectname); //ищет в allprojectsdom
    Item* findaccountmanager(const char* mgrname); //ищет менеджер по имени
    Item* findappbywuname(const char* wuname); //найти приложение для данного WU
    void  opactivity(const char* op); //изменение режима активности BOINC сервера "always" "auto" "newer"
    void  opnetactivity(const char* op); //изменение режима активности сети "always" "auto" "newer"
    void  opgpuactivity(const char* op); //изменение режима активности GPU "always" "auto" "newer"
    void  optask(const char* url, const char* name, const char* op); //действия над задачей ("suspend_result",...)
    void  opproject(const char* name, const char* op); //действия над проектом ("project_suspend","project_resume",...)
    void  runbenchmarks(); //запустить бенчмарк
    bool  projectattach(const char* url, const char* prjname, const char* email, const char* pass, std::string& errmsg); //подключить проект
    bool  createaccount(const char* url, const char* email, const char* pass, const char* username, const char* teamname, std::string& errmsg); //создать аккаунт
    bool  accountmanager(const char* url, const char* username, const char* pass, bool useconfigfile, std::string& errmsg); //подключить аккаунт менеджер
    bool  getprojectconfig(const char* url, std::string& errmsg); //получить c сервера файл конфигурации
    time_t	getlaststattime(); //вернет время последней имеющейся статистики
    PtrList	msgdom; 	//xml дерево сообщений
    int		lastmsgno; 	//номер последнего сообщения полученного с сервера
    PtrList	statedom; 	//xml дерево состояний
    PtrList	ccstatusdom;	//xml дерево для <get_cc_status>
    PtrList	dusagedom;	//xml дерево для <get_disk_usage>
    PtrList	statisticsdom;	//xml дерево для <get_statistics>
    Item*	allprojectsdom;	//xml дерево для <get_all_projects_list>
    PtrList	acctmgrinfodom;	//xml дерево для <acct_mgr_info>
    bool	ccstatusdomneedupdate; //если true тред обновит ccstatusdom без ожидания
    Item* req(const char* fmt, ...);  //выполнить запрос (вернет дерево или NULL)
    virtual void  createconnect();
    void  setactive(bool b); //включить/выключить тред обновления данных
    bool  isactive() {return active;};
    void lock() { pthread_mutex_lock(&mutex); };
    void unlock() { pthread_mutex_unlock(&mutex); };
    bool	loginfail; //true если получен unauthorize
    char* hostid;
  protected:
    void updatestate();		//обновить состояние <get_state>
    void updatemsgs();		//обновить список сообщений <get_messages>
    void updatestatistics();	//обновить статистику <get_statistics>
    void updatediskusage();	//обновить состояние <get_disk_usage>
    void updateccstatus();	//обновить состояние <get_cc_status>
    void updateacctmgrinfo();//обновить статистику <acct_mgr_info>
    time_t gettimeelapsed(time_t t); //вернет соличество секунд между t и тек. временем
    bool  login(); 		//авторизоваться на сервере
    char* pwd;
  private:
    unsigned int 	takt; //номер оборота цикла updatethread()
    static void* 	updatethread(void* args); //трейд опрашивающий сервер
    pthread_t		thread;
    bool		active; //true если трейд активен
    pthread_mutex_t	mutex;
};


class SrvList //список всех серверов
{
  public:
    SrvList(/*Config* cfg*/);
    virtual ~SrvList();
    //Srv* addserver(const char* shost, const char* sport) { return new Srv(shost, sport); }; //добавить сервер (вернет указаталь на сервер)
    void requestdata() {}; //опросить все сервера
    //Srv* findserver(const char* shost, const char* sport) { return servers.front(); }; //ЗАГЛУШКА здесь нужен поиск по аресу проту
    Srv* getcursrv() { if (cursrv != servers.end()) return (*cursrv); else return NULL; }; //получить текущий сервер или NULL
    void refreshcfg(); //перечитать из конфига
    void clear(); //удалить все соединения
    void nextserver(); //переключиться на след сервер в списке
    void prevserver();
  protected:
    void addserver(Srv* c) { servers.push_back(c); };
    std::list<Srv*> servers; //список соединений
    std::list<Srv*>::iterator cursrv; //текущиq сервер
//    Config*	cfg;
};


#endif //SRVDATA_H
