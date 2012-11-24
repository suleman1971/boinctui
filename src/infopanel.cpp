#include <string.h>
#include <algorithm>
#include "infopanel.h"
#include "net.h"
#include "resultparse.h"

/*
bool daily_statisticsCmpAbove( Item* stat1, Item* stat2 ) //для сортировки задач true если дата stat1 > stat2
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
*/

Item* findDay( std::vector<Item*>& days, time_t d ) //ищем в статистике день d если нет вернет NULL
{
    std::vector<Item*>::reverse_iterator rit;
    for (rit = days.rbegin(); rit != days.rend(); rit++)
    {
	Item* day = (*rit)->findItem("day");
	if (day != NULL)
	{
	    if ( day->getdvalue() == d )
		return (*rit);
	}
    }
    return NULL;
}

void InfoPanel::refresh()
{
    if (srv == NULL)
	return;
    if (srv->statedom == NULL)
    {
	needrefresh = true;
	werase(win);
	mvwprintw(win,0,0,"%s:%s\noffline",srv->gethost(),srv->getport());
	NView::refresh();
	return;
    }
    wattrset(win,A_REVERSE);
    mvwprintw(win,0,0,"       Tasks        ");
    wattrset(win,0);

    mvwprintw(win,1,0,"all              %3d",nalltasks);
    mvwprintw(win,2,0,"active           %3d",nactivetasks);
    mvwprintw(win,3,0,"run              %3d",nruntasks);
    mvwprintw(win,4,0,"wait             %3d",nqueuetasks);
    mvwprintw(win,5,0,"done             %3d",ndonetasks);
    mvwprintw(win,6,0,"other            %3d",nothertasks);

    wattrset(win,A_REVERSE);
    mvwprintw(win,7,0,"      Storage       ");
    wattrset(win,0);

    mvwprintw(win,8,0, "total     %8.2fGb",dtotal/(1024*1024*1024));
    mvwprintw(win,9,0, "free      %8.2fGb",dfree/(1024*1024*1024));
    mvwprintw(win,10,0,"allowed   %8.2fGb",dallowed/(1024*1024*1024));
    mvwprintw(win,11,0,"boinc     %8.2fGb",dboinc/(1024*1024*1024));

    wattrset(win,A_REVERSE);
    mvwprintw(win,12,0,"     Statistics     ");
    wattrset(win,0);

    mvwprintw(win,13,0,"user total%10.0f",usertotal);
    mvwprintw(win,14,0,"user avg  %10.0f",useravg);
    mvwprintw(win,15,0,"host total%10.0f",hosttotal);
    mvwprintw(win,16,0,"host avg  %10.0f",hostavg);

    tm* ltime = localtime(&laststattime);
    char buf[128];
    strftime(buf, sizeof(buf),"%-e %b %-k:%M",ltime);
    //wattrset(win,COLOR_PAIR(3));
    mvwprintw(win,17,0,"%-s", buf); //дата/время последней статистики
    //wattrset(win,0);
    mvwaddch(win,18,0,ACS_LTEE);     waddch(win,ACS_HLINE); wprintw(win,">user   %10.0f",lastdayuser);
    mvwaddch(win,19,0,ACS_LLCORNER); waddch(win,ACS_HLINE); wprintw(win,">host   %10.0f",lastdayhost);

    //по проектам
    wprintw(win,"\n");
    int i = 0;
    int line,col;
    getyx(win,line,col);
    for (int i = 0; i < projnames.size(); i++) //цикл по названиям проектов
    {
	if ( getheight()-(line+i*6) < 6 )
	    break; //не выводим если осталось меньше 3х строк
	wattrset(win,COLOR_PAIR(3));
	mvwprintw(win,line+i*6,0,"%s\n",projnames[i].c_str());
	wattrset(win,0);
	mvwprintw(win,line+i*6+1,0,"user total%10.0f\n",projuser[i]);
	mvwprintw(win,line+i*6+2,0,"host total%10.0f\n",projhost[i]);
	ltime = localtime(&projectlaststattime[i]);
	strftime(buf, sizeof(buf),"%-e %b %-k:%M",ltime);
	mvwprintw(win,line+i*6+3,0,"%-s\n",buf);
	mvwaddch(win,line+i*6+4,0,ACS_LTEE);     waddch(win,ACS_HLINE); wprintw(win,">user   %10.0f\n",projectlastuser[i]);
	mvwaddch(win,line+i*6+5,0,ACS_LLCORNER); waddch(win,ACS_HLINE); wprintw(win,">host   %10.0f\n",projectlasthost[i]);
    }
    wclrtobot(win); //чистим нижню часть окна
    //wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    NView::refresh();
}


void InfoPanel::updatedata()
{
    if (srv == NULL)
	return;
    //===данные по процессам===
    srv->updatestate();
    if (srv->statedom == NULL)
	return;
    Item* client_state = srv->statedom->findItem("client_state");
    nactivetasks = 0;
    ndonetasks = 0;
    nruntasks = 0;
    nqueuetasks = 0;
    if (client_state != NULL)
    {
        std::vector<Item*> results = client_state->getItems("result");
	std::vector<Item*>::iterator it;
	nalltasks = results.size();
	for (it = results.begin(); it!=results.end(); it++) //цикл списка задач
	{
	    Item* ready_to_report = (*it)->findItem("ready_to_report");
	    if (ready_to_report != NULL)
	        ndonetasks++;
	    Item* active_task = (*it)->findItem("active_task");
	    if (active_task != NULL)
	    {
		nactivetasks++;
		if (active_task->findItem("active_task_state")->getivalue() == 1)
		    nruntasks++;
	    }
	    else
	    {
		if (ready_to_report == NULL)
		    nqueuetasks++;
	    }
	} //цикл списка задач
	nothertasks = nalltasks-nruntasks-ndonetasks-nqueuetasks;
	needrefresh = true;
    }
    //===данные по дискам===
    srv->updatediskusage();
    if (srv->dusagedom == NULL)
	return;
    Item* disk_usage_summary = srv->dusagedom->findItem("disk_usage_summary");
    if (disk_usage_summary != NULL)
    {
	dtotal = disk_usage_summary->findItem("d_total")->getdvalue();
	dfree = disk_usage_summary->findItem("d_free")->getdvalue();
	dboinc = disk_usage_summary->findItem("d_boinc")->getdvalue();
	dallowed = disk_usage_summary->findItem("d_allowed")->getdvalue();
        std::vector<Item*> results = disk_usage_summary->getItems("project");
	std::vector<Item*>::iterator it;
	for (it = results.begin(); it!=results.end(); it++) //цикл списка задач
	{
	    dboinc = dboinc + (*it)->findItem("disk_usage")->getdvalue();
	}
    }
    //===данные статистики===
    srv->updatestatistics();
    if (srv->statisticsdom == NULL)
	return;
    laststattime = srv->getlaststattime();
//    time_t predstattime = laststattime - (24*60*60); //предыдущий день
    usertotal = 0;
    useravg   = 0;
    hosttotal = 0;
    hostavg   = 0;
    lastdayuser = 0;
    lastdayhost = 0;
    double usertotallastday = 0;
    double hosttotallastday = 0;
    double usertotalpredday = 0;
    double hosttotalpredday = 0;
    projnames.clear();
    projuser.clear();
    projhost.clear();
    projectlaststattime.clear();
    projectlastuser.clear();
    projectlasthost.clear();
    Item* statistics = srv->statisticsdom->findItem("statistics");
    if (statistics!=NULL)
    {
	std::vector<Item*> project_statistics = statistics->getItems("project_statistics");
	std::vector<Item*>::iterator it;
	for (it = project_statistics.begin(); it!=project_statistics.end(); it++) //цикл списка проектов
	{
	    projnames.push_back(srv->findProjectName(srv->statedom,((*it)->findItem("master_url")->getsvalue())));
	    std::vector<Item*> daily_statistics = (*it)->getItems("daily_statistics"); //все дни проекта в этом векторе
	    std::sort(daily_statistics.begin(), daily_statistics.end(), daily_statisticsCmpAbove); //сортируем по убыванию дат
	    if (!daily_statistics.empty())
	    {
		Item* lastday = findDay(daily_statistics, laststattime); //последний день
		Item* predday = NULL;
//		time_t d = lastday->findItem("day")->getdvalue(); //время из статистики
		if ( lastday != NULL) //для этого проекта последний день есть
		{
		    usertotallastday = usertotallastday + lastday->findItem("user_total_credit")->getdvalue();
		    hosttotallastday = hosttotallastday + lastday->findItem("host_total_credit")->getdvalue();
		    //берем предпоследний
		    if (daily_statistics.size() > 1)
			predday = daily_statistics[1];
		}
//		else //в этом проекте за последний день ничего нет
//		{
//		    predday = daily_statistics.front(); //последний день этого проекта учитываем как предыдущий
//		}
		//накапливаем статистику за предыдущий день (если есть)
		if (predday != NULL)
		{
		    usertotalpredday = usertotalpredday + predday->findItem("user_total_credit")->getdvalue();
		    hosttotalpredday = hosttotalpredday + predday->findItem("host_total_credit")->getdvalue();
		}
		//суммарно по всем дням и проектам
		Item* frontday = daily_statistics.front(); //берем последний не смотря на его дату
		if (daily_statistics.size() > 1)
		{
		    Item* predfrontday;
		    predfrontday = daily_statistics[1];
		    projectlastuser.push_back(frontday->findItem("user_total_credit")->getdvalue()-predfrontday->findItem("user_total_credit")->getdvalue());
		    projectlasthost.push_back(frontday->findItem("host_total_credit")->getdvalue()-predfrontday->findItem("host_total_credit")->getdvalue());
		}
		projectlaststattime.push_back(frontday->findItem("day")->getdvalue());
		usertotal = usertotal + frontday->findItem("user_total_credit")->getdvalue();
		useravg = useravg + frontday->findItem("user_expavg_credit")->getdvalue();
		hosttotal = hosttotal + frontday->findItem("host_total_credit")->getdvalue();
		hostavg = hostavg + frontday->findItem("host_expavg_credit")->getdvalue();
		projuser.push_back(frontday->findItem("user_total_credit")->getdvalue());
		projhost.push_back(frontday->findItem("host_total_credit")->getdvalue());
	    }
	    else
	    {
		projuser.push_back(0);
		projhost.push_back(0);
		projectlaststattime.push_back(0);
		projectlastuser.push_back(0);
		projectlasthost.push_back(0);
	    }
	} //проекты
	lastdayuser = usertotallastday - usertotalpredday;
	lastdayhost = hosttotallastday - hosttotalpredday;
    }
}
