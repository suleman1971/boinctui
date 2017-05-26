// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012-2014 Sergey Suslov
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

#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include "kclog.h"
#include "statwin.h"


StatWin::StatWin(const char* caption, Srv* srv)
:	NGroup(NRect(getmaxy(stdscr) - 10, getmaxx(stdscr) - 60, 3, 3))
{
    modalflag = true;
    this->srv = srv;
    this->caption = " ";
    this->caption  = this->caption + caption;
    this->caption  = this->caption + " ";
    content = new NScrollView(NRect(getheight()-4, getwidth()-4, 2, 2));
    insert(content);
    updatedata();
    //корректируем высоту в меньшую сторону если возможно
    if (content->getstringcount() < content->getheight())
    {
	content->resize(content->getstringcount(), content->getwidth());
	resize(content->getheight() + 4, getwidth());
    }
    //растягиваем по ширине
    //content->resize(content->getheight(), maxlen1 + maxlen2 + 10);
    //resize(getheight(), content->getwidth() + 4);
    if (getwidth() > getmaxx(stdscr) - 10) //если слишком широко обрезаем
    {
	resize(getheight(), getmaxx(stdscr) - 10);
	content->resize(content->getheight(), getwidth() - 4);
    }
    if(asciilinedraw == 1)
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    else
	box(win,0,0);
    mvwprintw(win,0,getwidth()/2-(strlen(caption)/2), this->caption.c_str());

    scrollbar = new NScrollBar(NRect(content->getheight()+2,1, content->getbegrow()-1, getwidth()-1), 0, 0, ACS_VLINE);
    content->setscrollbar(scrollbar);
    insert(scrollbar);

    refresh();
}


void StatWin::updatedata()
{
    if (srv == NULL)
	return;
    if (srv->statisticsdom.empty())
	return;
    //===данные статистики===
    content->clearcontent();
    projects.clear();
    std::map<std::string, std::vector<DayStat> > allstat;

    Item* tmpstatedom = srv->statedom.hookptr();
    Item* tmpstatisticsdom = srv->statisticsdom.hookptr();
    Item* statistics = tmpstatisticsdom->findItem("statistics");
    if (statistics!=NULL)
    {
	std::vector<Item*> project_statistics = statistics->getItems("project_statistics");
	std::vector<Item*>::iterator it;
	for (it = project_statistics.begin(); it!=project_statistics.end(); it++) //цикл списка проектов
	{
	    char* sproject = strdup(srv->findProjectName(tmpstatedom, (*it)->findItem("master_url")->getsvalue()).c_str());
	    ProjStat project;
	    project.name = sproject;
///	    NColorString* cs = new NColorString(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, "%-*s   ", 40, sproject);
///	    content->addstring(cs);

	    std::vector<Item*> days = (*it)->getItems("daily_statistics");
	    std::vector<Item*>::iterator it2;
	    for (it2 = days.begin(); it2 != days.end(); it2++) //циул по дням одного проекта
	    {
//		tm* daytm = localtime(&d);
//	        char buf[128];
//		strftime(buf, sizeof(buf),"%-e %b",daytm); //"%-e %b %-k:%M"
		//mvwprintw(win,line++,0,"%-s %s\n",buf, getdayname(projects[i].laststattime).c_str());
//		tm* daytm = localtime(&d);
///		cs->append(getcolorpair(COLOR_RED, COLOR_BLACK) | A_BOLD, " (%s) ", buf);
		DayStat day;
		day.day = (*it2)->findItem("day")->getdvalue(); //время из статистики;
		day.scorehost = (*it2)->findItem("host_total_credit")->getdvalue();;
		day.scoreuser = 222;
		project.days.push_back(day); //добавить день в проект
	    }
	    projects.push_back(project); //добавляем в общую матрицу
/*
	    ProjectStat st; //заполнить эту структуру
	    st.name = srv->findProjectName(tmpstatedom,((*it)->findItem("master_url")->getsvalue()));
	    st.sstatus = "";
	    //строка статуса
	    if (!srv->statedom.empty())
	    {
		Item* project = srv->findprojectbyname(st.name.c_str());
		if (project != NULL)
		{
		    st.sstatus = st.sstatus + (project->findItem("suspended_via_gui")? "[Susp.] " : "");
		    st.sstatus = st.sstatus + (project->findItem("dont_request_more_work") ? "[N.N.Tsk.] " : "");
		}
	    }
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
		Item* frontday = daily_statistics.front(); //берем последний несмотря на его дату
		if (daily_statistics.size() > 1)
		{
		    Item* predfrontday;
		    predfrontday = daily_statistics[1];

		    st.userlastday = frontday->findItem("user_total_credit")->getdvalue()-predfrontday->findItem("user_total_credit")->getdvalue();
		    st.hostlastday = frontday->findItem("host_total_credit")->getdvalue()-predfrontday->findItem("host_total_credit")->getdvalue();
		}
		st.laststattime = frontday->findItem("day")->getdvalue();
		usertotal = usertotal + frontday->findItem("user_total_credit")->getdvalue();
		useravg = useravg + frontday->findItem("user_expavg_credit")->getdvalue();
		hosttotal = hosttotal + frontday->findItem("host_total_credit")->getdvalue();
		hostavg = hostavg + frontday->findItem("host_expavg_credit")->getdvalue();

		st.user = frontday->findItem("user_total_credit")->getdvalue();
		st.host = frontday->findItem("host_total_credit")->getdvalue();
	    }
	    else
	    {
		st.user = 0;
		st.host = 0;
		st.laststattime = 0;
		st.userlastday = 0;
		st.hostlastday = 0;
	    }
	    projects.push_back(st); //вставляем статистику в вектор проектов
*/
	} //проекты
//	lastdayuser = usertotallastday - usertotalpredday;
//	lastdayhost = hosttotallastday - hosttotalpredday;
	//сортируем проекты чтобы наиболее актуальные отображались первыми
//	std::sort(projects.begin(), projects.end(), ProjectStat::CmpAbove);

	//формируем содержимое окна на основе структуры projects
	std::vector<ProjStat>::iterator itprj;
	std::list<time_t> daylist; //для заголовков дней
	NColorString* cs = new NColorString(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, "%-*s ", 7, "");
	for (itprj = projects.begin(); itprj != projects.end(); itprj++) //проекты
	{
	    std::string s = (*itprj).name.c_str();
	    if (s.size() > 10)
		s.resize(10);
	    cs->append(getcolorpair(COLOR_RED, COLOR_BLACK) | A_BOLD, " (%*s) ", 10, s.c_str()); //имена проектов
	    std::list<DayStat>::iterator itdays;
	    for(itdays = (*itprj).days.begin(); itdays!=(*itprj).days.end(); itdays++)
	    {
		if (find (daylist.begin(), daylist.end(), (*itdays).day) == daylist.end()) //еще нет в списке
		    daylist.push_back((*itdays).day); //добавляем
	    }
	}
	content->addstring(cs);
	daylist.sort();
	std::list<time_t>::reverse_iterator itday;
	for (itday = daylist.rbegin(); itday != daylist.rend(); itday++) //дни
	{
	    tm* daytm = localtime(&(*itday));
	    char buf[128];
	    strftime(buf, sizeof(buf),"%-e %b",daytm); //"%-e %b %-k:%M"
	    //mvwprintw(win,line++,0,"%-s %s\n",buf, getdayname(projects[i].laststattime).c_str());
	    NColorString* cs = new NColorString(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, "%-*s ", 10, strdup(buf));
	    for (itprj = projects.begin(); itprj != projects.end(); itprj++) //перебрать все проекты по этому дню
	    {
		//находим нужный день среди всех дней проекта
		std::list<DayStat>::iterator itdays;
		long int value = -1;
		for(itdays = (*itprj).days.begin(); itdays!=(*itprj).days.end(); itdays++)
		{
		    if ((*itdays).day == *itday)
		    {
			value = (*itdays).scorehost; //найден нужный день
			break;
		    }
		}
		if (value != -1)
		    cs->append(getcolorpair(COLOR_RED, COLOR_BLACK) | A_BOLD, " (%*ld) ",10, value);
		else
		    cs->append(getcolorpair(COLOR_RED, COLOR_BLACK) | A_BOLD, " (%*s) ",10, "-");
	    }
	    content->addstring(cs);
	}
    }
    srv->statisticsdom.releaseptr(tmpstatisticsdom);
    srv->statedom.releaseptr(tmpstatedom);
}


void StatWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;
    //реакция на мышь
    NMouseEvent* mevent = (NMouseEvent*)ev;
    if ( ev->type == NEvent::evMOUSE )
    {
	//блокируем все что внутри
	if (isinside(mevent->row, mevent->col))
	    ev->done = true;
	//закрываем при любом клике независимо от координат
	if (mevent->cmdcode & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED))
	{
	    putevent(new NEvent(NEvent::evKB, 27));
	    ev->done = true;
	}
    }
    //клавиатура
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_PPAGE:
		content->scrollto(-getheight()/2); //вверх на полокна
		content->setautoscroll(false);
		break;
	    case KEY_NPAGE:
		if (!content->getautoscroll())
		{
		    int oldpos = content->getstartindex();
		    content->scrollto(getheight()/2); 	//вниз на пол окна
		    if ( oldpos == content->getstartindex()) 	//позиция не изменилась (уже достигли конца)
			content->setautoscroll(true);	//включаем автоскроллинг
		}
		break;
	    default:
		//блокировать все клавиатурные кроме кода закрытия формы
		if (ev->keycode == 27)
		    ev->done = false;
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
    if ( ev->type == NEvent::evTIMER )
    {
	updatedata();	//запросить данные с сервера
	refresh(); 		//перерисовать окно
    }
}
