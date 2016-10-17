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

#ifndef TASKWIN_H
#define TASKWIN_H

#include "nselectlist.h"
#include "resultdom.h"
#include "srvdata.h"


struct TaskInfo
{
    TaskInfo(const char* taskname, const char* projecturl) {this->taskname = taskname; this->projecturl = projecturl; };
    std::string taskname;
    std::string projecturl;
};


class TaskWin : public NSelectList
{
  public:
    TaskWin(NRect rect/*, Config* cfg*/);
    virtual ~TaskWin();
    void	updatedata(); 	//обновить данные с серверa
    virtual void clearcontent();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void	setserver(Srv* srv) { this->srv = srv; startindex = 0; clearcontent(); selectedindex = -1; };
    bool iscolvisible(int n) { return ((1 << n)&columnmask); }; //true если n-ная колонка видимая
    void coldisable(int n) { columnmask = columnmask & (~(1 << n)); saveopttoconfig();}; //установить флаг невидимости n-ной колонки
    void colenable(int n) { columnmask = columnmask | (1 << n); saveopttoconfig();}; //установить флаг видимости n-ной колонки
    void saveopttoconfig(); //сохранить маску и т.д. в дереве конфига
    //unsigned int getcolumnmask() { return columnmask; };
  protected:
    Srv*	srv; //текущий отображаемый сервер
    unsigned int columnmask; //маска видмсости колонок
    int		taskslistmode; //0 все задачи 1 кроме завершенных 2 только активные
    int		taskssortmode; //0 нет сортировки 1 state 2 done 3 project 4 est 5 d/l 6 app 7 task
//    Config*	cfg;
};


#endif //TASKWIN_H
