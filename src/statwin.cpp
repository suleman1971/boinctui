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

#define COLWIDTH 10 //ширина колонки для одного проекта


StatWin::StatWin(Srv* srv)
:	NGroup(NRect(getmaxy(stdscr) - 10, getmaxx(stdscr) - 60, 3, 3))
{
    modalflag = true;
    this->srv = srv;
    this->hpos = 0;
    this->hostmode = true;
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
    content->resize(content->getheight(), content->getmaxcontentwidth());
    resize(getheight(), content->getwidth() + 4);
    if (getwidth() > getmaxx(stdscr) - 10) //если слишком широко обрезаем
    {
	resize(getheight(), getmaxx(stdscr) - 10);
	content->resize(content->getheight(), getwidth() - 4);
    }
    scrollbar = new NScrollBar(NRect(getheight()-2,1, content->getbegrow()-1, getwidth()-1), 0, 0, ACS_VLINE);
    content->setscrollbar(scrollbar);
    insert(scrollbar);
    updatecaption();
    refresh();
}


void StatWin::updatecaption()
{
    this->caption  = " Statistics ";
    //обновить заголовок окна
    if (hostmode)
	this->caption  += "[host] ";
    else
	this->caption  += "[user] ";
    if (srv == NULL)
	return;
    if(asciilinedraw == 1)
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    else
	box(win,0,0);
    mvwprintw(win,0,getwidth()/2-(caption.size()/2), "%s",this->caption.c_str());
}


void StatWin::updatedata()
{
    if (!srv)
	return;
    if (srv->statisticsdom.empty())
	return;
    //===данные статистики===
    int startindexsave = content->getstartindex();
    content->clearcontent();
    content->setstartindex(startindexsave);
    projects.clear();
    std::map<std::string, std::vector<DayStat> > allstat;

    Item* tmpstatedom = srv->statedom.hookptr();
    Item* tmpstatisticsdom = srv->statisticsdom.hookptr();
    Item* statistics = tmpstatisticsdom->findItem("statistics");
    if (statistics!=NULL)
    {
	std::vector<Item*> project_statistics = statistics->getItems("project_statistics");
	std::vector<Item*>::iterator it;
	std::map<time_t, unsigned long int> daysumhost; //сумарные очки по всем проектам для хоста
	std::map<time_t, unsigned long int> daysumuser; //сумарные очки по всем проектам для юзера
	for (it = project_statistics.begin(); it!=project_statistics.end(); it++) //цикл списка проектов
	{
	    double laststatH = 0;
	    double laststatU = 0;
	    ProjStat project;
	    project.name = srv->findProjectName(tmpstatedom, (*it)->findItem("master_url")->getsvalue());
//	    project.sumhost = 0;
//	    project.sumuser = 0;
	    std::vector<Item*> days = (*it)->getItems("daily_statistics");
	    std::vector<Item*>::iterator it2;
	    for (it2 = days.begin(); it2 != days.end(); it2++) //циул по дням одного проекта
	    {
		DayStat day;
		day.day = (*it2)->findItem("day")->getdvalue(); //время из статистики;
		double x1 = (*it2)->findItem("host_total_credit")->getdvalue();
		day.scorehost = x1 - laststatH;
		double x2 =  (*it2)->findItem("user_total_credit")->getdvalue();
		day.scoreuser = x2 - laststatU;
		if (it2 != days.begin()) //самый старый день не учитываем (из-за отсутствия laststat)
		    project.days.push_back(day); //добавить день в проект
		laststatH = x1;
		laststatU = x2;
		//накапливаем суммарные за этот день
		if ( 0 == daysumhost.count(day.day))
		    daysumhost[day.day] = day.scorehost;
		else
		    daysumhost[day.day] += day.scorehost;
		if ( 0 == daysumuser.count(day.day))
		    daysumuser[day.day] = day.scoreuser;
		else
		    daysumuser[day.day] += day.scoreuser;
	    }
	    projects.push_back(project); //добавляем в общую матрицу
	} //проекты
	//сортируем чтобы более актуальные попали в начало списка
	if (hostmode)
	    std::sort(projects.begin(), projects.end(), ProjStat::CmpAboveH);
	else
	    std::sort(projects.begin(), projects.end(), ProjStat::CmpAboveU);
	//формируем содержимое окна на основе структуры projects
	std::vector<ProjStat>::iterator itprj;
	std::list<time_t> daylist; //для заголовков дней
	NColorString* cs = new NColorString(getcolorpair(COLOR_CYAN, COLOR_BLACK) | A_BOLD, "  %-*s %-*s", COLWIDTH-2, "", COLWIDTH-2, "Summary");
	int n = hpos;
	for (itprj = projects.begin(); itprj != projects.end(); itprj++, n--) //проекты
	{
	    if ( n > 0 )
		continue; //пропустить если отображаем не с первого проекта (гориз скроллинг)
	    std::string s = (*itprj).name.c_str();
	    if (s.size() > COLWIDTH)
		s.resize(COLWIDTH);
	    cs->append(getcolorpair(COLOR_RED, COLOR_BLACK) | A_BOLD, " %*s ", COLWIDTH, s.c_str()); //имена проектов
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
	    char buf[128] = "?";
	    strftime(buf, sizeof(buf),"%-2e %b",daytm); //"%-e %b %-k:%M"
	    NColorString* cs = new NColorString(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, "%-*s", COLWIDTH, buf);
	    cs->append(getcolorpair(COLOR_CYAN, COLOR_BLACK) | A_BOLD, " %*ld ", COLWIDTH, hostmode ? daysumhost[*itday] : daysumuser[*itday]);
	    n = hpos;
	    for (itprj = projects.begin(); itprj != projects.end(); itprj++, n--) //перебрать все проекты по этому дню
	    {
		if ( n > 0 )
		    continue; //пропустить если отображаем не с первого проекта (гориз скроллинг)
		//находим нужный день среди всех дней проекта
		std::list<DayStat>::iterator itdays;
		long int value = -1;
		for(itdays = (*itprj).days.begin(); itdays!=(*itprj).days.end(); itdays++)
		{
		    if ((*itdays).day == *itday)
		    {
			//найден нужный день
			if (hostmode)
			    value = (*itdays).scorehost;
			else
			    value = (*itdays).scoreuser;
			break;
		    }
		}
		if (value != -1)
		    cs->append(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, " %*ld ",COLWIDTH, value);
		else
		    cs->append(getcolorpair(COLOR_BLACK, COLOR_BLACK) | A_BOLD, " %*s ",COLWIDTH, "-");
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
	    case 'U':
	    case 'u':
		hostmode = !hostmode;
		updatedata();
		updatecaption();
		break;
	    case KEY_PPAGE:
		content->scrollto(-getheight()/2); //вверх на полокна
		break;
	    case KEY_NPAGE:
		if (!content->getautoscroll())
		{
            //int oldpos = content->getstartindex();
		    content->scrollto(getheight()/2); 	//вниз на пол окна
		}
		break;
	    case KEY_LEFT:
		if ( hpos > 0 )
		    hpos--;
		    updatedata();
		break;
	    case KEY_RIGHT:
	    {
		int w1 = (projects.size()-hpos) * COLWIDTH + 38; //сколько символов нужно для отображения всех видимых столбцов
		if (w1 > getwidth())
		    hpos++;
		    updatedata();
		break;
	    }
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
