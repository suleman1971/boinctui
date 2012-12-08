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
	//оба с нулевыми (или оба с ненеулевыми) очками -> ставниваем по датам
	if ( ((score1 == 0)&&(score2 == 0)) ||
	     ((score1 > 0)&&(score2 > 0)) )
	    if (stat1.laststattime > stat2.laststattime)
		return true; //больше stat1 у него дата больше
	    else
	    {
		//даты равны -> больше тот у кого больше очков
		return (score1 > score2);
	    }
	// если stat1 с ненулевыми очками (stat2 соотв с нулевыми) -> stat1 больше
	return (score1 > 0 );
    };
};


class InfoPanel : public NView
{
  public:
    InfoPanel(NRect rect) : NView(rect) {};
    void updatedata();
    virtual void refresh();
    void*	setserver(Srv* srv) { this->srv = srv; };
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
