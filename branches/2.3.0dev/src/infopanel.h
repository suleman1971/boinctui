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

#ifndef INFOPANEL_H
#define INFOPANEL_H

#include "nview.h"
#include "resultdom.h"
#include "srvdata.h"


struct ProjectStat //статистика по проекту
{
    std::string		name;		//имя проекта
    std::string		sstatus;	//строка состояния проекта
    double		user;		//очки по проекту (всего для юзера)
    double		host;		//очки по проекту (всего для хоста)
    time_t		laststattime;	//дата последней статистики
    double 		userlastday; 	//очки юзера за последний день по каждому проекту
    double 		hostlastday;	//очки хоста за последний день по каждому проекту
    static bool CmpAbove( ProjectStat stat1, ProjectStat stat2 ) //для сортировки проектов true если дата stat1 > stat2
    {
	//суммарная для юзера и хоста
	double score1 = stat1.userlastday + stat1.hostlastday;
	double score2 = stat2.userlastday + stat2.hostlastday;
	if (stat1.laststattime == stat2.laststattime)
	    return (score1 > score2); //даты равны -> больше тот у кого больше очков
	else
	    return (stat1.laststattime > stat2.laststattime); //больше тот у кого дата больше
	/*
	//оба с нулевыми (или оба с ненеулевыми) очками -> ставниваем по датам
	if ( ((score1 == 0)&&(score2 == 0)) ||
	     ((score1 > 0)&&(score2 > 0)) )
	{
	    if (stat1.laststattime == stat2.laststattime)
	    {
		//даты равны -> больше тот у кого больше очков
		return (score1 > score2);
	    }
	    else //больше stat1 если у него дата больше
		return (stat1.laststattime > stat2.laststattime);
	}
	// если stat1 с ненулевыми очками (stat2 соотв с нулевыми) -> stat1 больше
	return (score1 > 0 );
	*/
    };
};


class InfoPanel : public NView
{
  public:
    InfoPanel(NRect rect) : NView(rect) {};
    void updatedata();
    virtual void refresh();
    void*	setserver(Srv* srv) { this->srv = srv; };
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    Srv*	srv;
    std::string getdayname(time_t ltime); //название дня "today" "yesterday" ""
    //---для процессов---
    int nalltasks;
    int nactivetasks;
    int nruntasks;
    int nqueuetasks;
    int ndonetasks;
    int nothertasks;
    //---для дисков---
    double dtotal;
    double dfree;
    double dboinc;
    double dallowed;
    //---статистика cуммарная---
    double usertotal;
    double useravg;
    double hosttotal;
    double hostavg;
    //---статистика за сегодня---
    time_t laststattime; //дата последней статистики
    double lastdayuser;
    double lastdayhost;
    std::vector<ProjectStat> projects; //статистика по отдельным проектам
};

#endif //INFOPANEL_H
