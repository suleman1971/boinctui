#ifndef INFOPANEL_H
#define INFOPANEL_H


#include "nview.h"
#include "resultdom.h"
#include "srvdata.h"


class InfoPanel : public NView
{
  public:
    InfoPanel(NRect rect) : NView(rect) {};
    void updatedata();
    virtual void refresh();
    void*	setserver(Srv* srv) { this->srv = srv; };
  protected:
    Srv*	srv;
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
    std::vector<std::string> 	projnames;	//список названий проектов
    std::vector<double>	projuser;	//очки по проекту user
    std::vector<double>	projhost;	//очки по проекту host
    std::vector<time_t>	projectlaststattime; //дата последней статистики по каждому проекту
    std::vector<double>	projectlastuser; //очки юзера за последний день по каждому проекту
    std::vector<double>	projectlasthost; //очки хоста за последний день по каждому проекту
};

#endif //INFOPANEL_H