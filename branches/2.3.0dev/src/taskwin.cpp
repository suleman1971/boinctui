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

#include <string.h>
#include <algorithm>
#include <sstream>
#include "taskwin.h"
#include "net.h"
#include "resultparse.h"
#include "mbstring.h"
#include "tuievent.h"
#include "kclog.h"


typedef bool (*FnResultCmpLess)( Item* res1, Item* res2 ); //тип для сортировки списка задач


bool resultCmpLessByState( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    Item* active_task1 = res1->findItem("active_task");
    Item* active_task2 = res2->findItem("active_task");
    Item* ready_to_report1 = res1->findItem("ready_to_report");
    Item* ready_to_report2 = res2->findItem("ready_to_report");

    if ( (active_task1 != NULL)&&(active_task2 == NULL) )
	return true;
    if ( (active_task2 != NULL)&&(active_task1 == NULL) )
	return false;

    if ( (ready_to_report1 == NULL)&&(ready_to_report2 != NULL) )
	return true;
    if ( (ready_to_report2 != NULL)&&(ready_to_report1 == NULL) )
	return false;

    return false;
}


bool resultCmpLessByDone( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    Item* fraction_done1 = res1->findItem("fraction_done");
    Item* fraction_done2 = res2->findItem("fraction_done");
    Item* ready_to_report1 = res1->findItem("ready_to_report");
    Item* ready_to_report2 = res2->findItem("ready_to_report");
    //kLogPrintf("resultCmpLessByDone: res1 (%s) %p\n", res1->findItem("name")->getsvalue(), fraction_done1);
    //kLogPrintf("resultCmpLessByDone: res2 (%s) %p\n", res2->findItem("name")->getsvalue(), fraction_done2);
    if ( (fraction_done1 == NULL)&&(fraction_done2 == NULL) )
    {
	if ((ready_to_report1 != NULL)&&(ready_to_report2 == NULL)) //завершенные считаем как наименьшие !!!
	    return true;
    }
    if ( (fraction_done1 == NULL)&&(fraction_done2 != NULL) )
	return true; //неактивные меньше активных
    if ( (fraction_done1 != NULL)&&(fraction_done2 != NULL) )
    {
	//kLogPrintf("resultCmpLessByDone(%f,%f) = %c\n\n",fraction_done1->getdvalue(), fraction_done2->getdvalue(),fraction_done1->getdvalue() < fraction_done2->getdvalue() ? 't' : 'f');
	return (fraction_done1->getdvalue() < fraction_done2->getdvalue());
    }
    //kLogPrintf("false\n");
    return false;
}


bool resultCmpAboveByDone( Item* res1, Item* res2 ) //для сортировки задач true если res1 > res2
{
    return resultCmpLessByDone(res2, res1);
}


bool resultCmpLessByProject( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    std::string pname1 = Srv::findProjectName(res1->getparent()/*->statedom*/, res1->findItem("project_url")->getsvalue());
    std::string pname2 = Srv::findProjectName(res2->getparent()/*->statedom*/, res2->findItem("project_url")->getsvalue());
    std::transform(pname1.begin(), pname1.end(),pname1.begin(), ::toupper);
    std::transform(pname2.begin(), pname2.end(),pname2.begin(), ::toupper);
    return (pname1 < pname2);
}


bool resultCmpLessByEstimate( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    Item* estimated_cpu_time_remaining1 = res1->findItem("estimated_cpu_time_remaining");
    Item* estimated_cpu_time_remaining2 = res2->findItem("estimated_cpu_time_remaining");

    if ( ( estimated_cpu_time_remaining1 != NULL) && (estimated_cpu_time_remaining2 != NULL) )
	return (estimated_cpu_time_remaining1->getdvalue() < estimated_cpu_time_remaining2->getdvalue());

    if ( ( estimated_cpu_time_remaining1 != NULL) && (estimated_cpu_time_remaining2 == NULL) )
	return true;

    return false;
}


bool resultCmpLessByDL( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    Item* report_deadline1 = res1->findItem("report_deadline");
    Item* report_deadline2 = res2->findItem("report_deadline");

    if ( ( report_deadline1 != NULL) && (report_deadline2 != NULL) )
	return (report_deadline1->getdvalue() < report_deadline2->getdvalue());

    if ( ( report_deadline1 != NULL) && (report_deadline2 == NULL) )
	return true;

    return false;
}

/*
bool resultCmpLessByApp( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    return false; //not implemented yet
}
*/

bool resultCmpLessByTask( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
{
    Item* name1 = res1->findItem("name");
    Item* name2 = res2->findItem("name");
    std::string sname1 = name1->getsvalue();
    std::string sname2 = name2->getsvalue();
    std::transform(sname1.begin(), sname1.end(), sname1.begin(), ::toupper);
    std::transform(sname2.begin(), sname2.end(), sname2.begin(), ::toupper);
    return (sname1 < sname2);
}


std::string getresultstatestr(Item* result)
{
    Item* state = result->findItem("state");
//    Item* ready_to_report = result->findItem("ready_to_report");
    Item* active_task_state = result->findItem("active_task_state");
    if (result->findItem("ready_to_report") != NULL) //расчет завершен
    {
	if (state->getivalue() == 3)
	    return "DoneEr"; //была завершена с ошибкой
	else
	    return "Done";
    }
    if (result->findItem("suspended_via_gui") != NULL) //задача suspend via gui
	return "GSusp.";
    switch(state->getivalue())
    {
        case 0:
		return "New";
        case 1:
		return "Dwnld";
        case 3:
		return "Err";
        case 4:
		return "Upld";
    }
    if (active_task_state != NULL)
    {
	switch(active_task_state->getivalue())
	{
	    case 0:
		return "WaitR"; //разобраться с суспендом и возобновлением
	    case 1:
		return "Run";
	    case 9:
		return "Susp.";
	    case 5:
		return "Abort";
	    case 8:
		return "Quit";
	    case 10:
		return "Copy";
	    default:
		return "A"+active_task_state->getivalue();
	}
    }
    return "Wait";
}


std::string gethumanreadabletimestr(time_t time) //получить в виде строки прогнозируемое время завершения задачи
{
    std::stringstream s;
    tm* t = gmtime(&time);
    if ( t->tm_yday > 0 )
	s << t->tm_yday << "d";
    else
	if ( t->tm_hour > 0 )
	    s << t->tm_hour << "h";
	else
	    if ( t->tm_min > 0 )
		s << t->tm_min << "m";
	    else
		if ( t->tm_sec > 0 )
		    s << t->tm_sec << "s";
		else
		    s << "- ";
	return s.str();
}


TaskWin::TaskWin(NRect rect, Config* cfg) : NSelectList(rect) 
{
    setselectorbgcolor(COLOR_CYAN);
    columnmask = ~0;
    taskslistmode = 0;
    taskssortmode = 1;
    //читаем опции из конфига если нет то создаем
    if (cfg != NULL)
    {
	Item* rootcfg = cfg->getcfgptr();
	if (rootcfg != NULL)
	{
	    Item* column_view_mask = rootcfg->findItem("column_view_mask");
	    if (column_view_mask == NULL) //создать
	    {
		column_view_mask = new Item("column_view_mask");
		column_view_mask->setivalue(columnmask);
		rootcfg->addsubitem(column_view_mask);
	    }
	    columnmask = column_view_mask->getivalue();

	    Item* tasks_list_mode = rootcfg->findItem("tasks_list_mode");
	    if (tasks_list_mode == NULL) //создать
	    {
		tasks_list_mode = new Item("tasks_list_mode");
		tasks_list_mode->setivalue(taskslistmode);
		rootcfg->addsubitem(tasks_list_mode);
	    }
	    taskslistmode = tasks_list_mode->getivalue();

	    Item* tasks_sort_mode = rootcfg->findItem("tasks_sort_mode");
	    if (tasks_sort_mode == NULL) //создать
	    {
		tasks_sort_mode = new Item("tasks_sort_mode");
		tasks_sort_mode->setivalue(taskssortmode);
		rootcfg->addsubitem(tasks_sort_mode);
	    }
	    taskssortmode = tasks_sort_mode->getivalue();
	}
    }
    this->cfg = cfg;
};


TaskWin::~TaskWin()
{
    kLogPrintf("TaskWin::~TaskWin()\n");
    clearcontent();
}


void TaskWin::saveopttoconfig() //сохранить маску и т.д. в дереве конфига
{
    //пишем в конфиг
    if (cfg != NULL)
    {
	Item* rootcfg = cfg->getcfgptr();
	if (rootcfg != NULL)
	{
	    Item* column_view_mask = rootcfg->findItem("column_view_mask");
	    if (column_view_mask != NULL)
		column_view_mask->setivalue(columnmask);
	    Item* tasks_list_mode = rootcfg->findItem("tasks_list_mode");
	    if (tasks_list_mode != NULL)
		tasks_list_mode->setivalue(taskslistmode);
	    Item* tasks_sort_mode = rootcfg->findItem("tasks_sort_mode");
	    if (tasks_sort_mode != NULL)
		tasks_sort_mode->setivalue(taskssortmode);
	}
    }
}


void TaskWin::clearcontent()
{
    std::vector<void*>::iterator it;
    for (it = objects.begin(); it != objects.end(); it++) //очищаем строки ассоцииронанные с визуальными
    {
	if ((*it) != NULL)
	    free (*it);
    }
    objects.clear();
    NSelectList::clearcontent();
}


void TaskWin::updatedata() //обновить данные с сервера
{
    if (srv == NULL)
	return;
    clearcontent();
    if (srv->statedom.empty())
	return;
    Item* tmpstatedom = srv->statedom.hookptr();
    Item* client_state = tmpstatedom->findItem("client_state");
    int i = 1; //счетчик заданий строк
    if (client_state != NULL)
    {
        std::vector<Item*> results = client_state->getItems("result");
	std::vector<Item*>::iterator it;
	if (taskssortmode != 0)
	{
	    //критерий сортировки
	    FnResultCmpLess fcmpless = resultCmpLessByState; //по умолчанию
	    switch(taskssortmode)
	    {
		case 1:
		    fcmpless = resultCmpLessByState; //по state
		    break;
		case 2:
		    fcmpless = resultCmpAboveByDone; //по done%
		    break;
		case 3:
		    fcmpless = resultCmpLessByProject; //по project
		    break;
		case 4:
		    fcmpless = resultCmpLessByEstimate; //по estimate
		    break;
		case 5:
		    fcmpless = resultCmpLessByDL; //по deadline
		    break;
		case 7:
		    fcmpless = resultCmpLessByTask; //по task
		    break;
	    }//switch
	    std::sort(results.begin(), results.end(), fcmpless); //сортируем
	}
	int tasknumber = 1;
	for (it = results.begin(); it!=results.end(); it++,i++) //цикл списка задач
	{
	    Item* name  = (*it)->findItem("name");
	    Item* fraction_done = (*it)->findItem("fraction_done");
	    if (name)
	    {
		int column = 0;
		int attr = getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_NORMAL; //ставим цвет по умолчанию
		NColorString* cs = new NColorString(attr, "");
		std::string sstate = getresultstatestr(*it); //состояние задачи
		//цвет и атрибут в зависимости от состояния задачи
		if ((*it)->findItem("ready_to_report") != NULL)
		    attr = getcolorpair(COLOR_BLACK,COLOR_BLACK) | A_BOLD;
		if ((*it)->findItem("active_task") != NULL)
		    attr =  getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD; //ставим цвет по умолчанию + A_BOLD;
		if ( sstate == "Run")
		    attr = getcolorpair(COLOR_YELLOW,COLOR_BLACK) | A_BOLD;
		if ( sstate == "Upld")
		    attr = getcolorpair(COLOR_BLUE,COLOR_BLACK) | A_BOLD;
		if ( sstate == "Dwnld")
		    attr = getcolorpair(COLOR_GREEN,COLOR_BLACK) | A_BOLD;
		int stateattr = attr;
		if ( sstate == "DoneEr")
		    stateattr = getcolorpair(COLOR_RED,COLOR_BLACK);
		//проверяем нужно-ли отображать эту задачу
		if
		( !(
		    (taskslistmode == 0)
		    || ( ( taskslistmode == 1) && (sstate != "Done" ) )
		    || ( ( taskslistmode == 2) && ((*it)->findItem("active_task") != NULL) )
		) )
		    continue; //пропускаем задачи не подходящие фильтру tasklistmode
		//колонка 0 номер задачи
		if(iscolvisible(column++))
		    cs->append(attr, " %2d  ", tasknumber++);
		//колонка 1 состояние
		char sdone[64];
		if (!fraction_done) //для неактивных секция fraction_done отсутствует
		    strcpy(sdone,"   -  ");
		else
		    sprintf(sdone,"%6.2f",100*fraction_done->getdvalue());
		if (iscolvisible(column++))
		    cs->append(stateattr, "%-6s", sstate.c_str());
		//колонка 2 процент выполнения и подвсетка для GPU задач
		int attrgpu = attr;
		Item* plan_class = (*it)->findItem("plan_class");
		if (plan_class != NULL)
		{
		    if ((strstr(plan_class->getsvalue(),"ati") != NULL )||(strstr(plan_class->getsvalue(),"opencl") != NULL))
			attrgpu = getcolorpair(COLOR_MAGENTA,COLOR_BLACK) | A_BOLD;
		    if (strstr(plan_class->getsvalue(),"cuda") != NULL )
			attrgpu = getcolorpair(COLOR_GREEN,COLOR_BLACK) | A_BOLD;
		}
		if (( sstate != "Run" )&&( sstate != "Done"))
		    attrgpu = attrgpu & (~A_BOLD); //выключаем болд для незапущенных
		if(iscolvisible(column++))
		    cs->append(attrgpu, "  %6s", sdone);
		//колонка 3 имя проекта
		std::string pname = srv->findProjectName(tmpstatedom, (*it)->findItem("project_url")->getsvalue());//findProjectName(srv->statedom, *it);
		char* sproject = strdup(pname.c_str());
		if(iscolvisible(column++))
		    cs->append(attr, "  %-20s", mbstrtrunc(sproject,20));
		//колонка 4 время эстимейт
		if(iscolvisible(column++))
		{
		    Item* estimated_cpu_time_remaining = (*it)->findItem("estimated_cpu_time_remaining");
		    int attr2 = attr;
		    if ( estimated_cpu_time_remaining != NULL )
		    {
			double dtime = estimated_cpu_time_remaining->getdvalue();
			if ( ( sstate == "Run" )&&( dtime < 600)&&( dtime >= 0 ) ) //осталось [0-600[ сек
			    attr2 = getcolorpair(COLOR_RED,COLOR_BLACK) | A_BOLD;
			if ( dtime >= 0)
			    cs->append(attr2," %4s", gethumanreadabletimestr(dtime).c_str()); //естимейт
			else
			    cs->append(attr2," %4s", "?"); //естимейт отрицательный (BOINC bug?)
		    }
		    else
			cs->append(attr2," %4s", "?");
		}
		//колонка 5 время дедлайн
		if(iscolvisible(column++))
		{
		    Item* report_deadline = (*it)->findItem("report_deadline");
		    int attr2 = attr;
		    if (report_deadline != NULL)
		    {
			double dtime = report_deadline->getdvalue();
			double beforedl = dtime - time(NULL); //число секунд до дедлайна
			if ( ( sstate != "Done")&&( beforedl < 3600 * 24 * 2) ) //осталось меньше 2-х дней
			    attr2 = getcolorpair(COLOR_BLUE,COLOR_BLACK) | A_BOLD;
			cs->append(attr2," %4s", gethumanreadabletimestr(beforedl).c_str());
		    }
		    else
			cs->append(attr2," %4s", "?");
		}
		//колонка 6 имя приложения
		if(iscolvisible(column++))
		{
		    char buf[256];
		    snprintf(buf, sizeof(buf),"%s","unknown application");
		    Item* wu_name = (*it)->findItem("wu_name");
		    if (wu_name != NULL)
		    {
			Item* app = srv->findappbywuname(wu_name->getsvalue());
			if (app != NULL)
			{
			    Item* user_friendly_name = app->findItem("user_friendly_name");
			    if (user_friendly_name != NULL)
				snprintf(buf, sizeof(buf),"%s",user_friendly_name->getsvalue());
			}
		    }
		    if (iscolvisible(column)) //если след колонка (task) видима нужно обрезать
			mbstrtrunc(buf,30);
		    cs->append(attr,"  %-30s", buf);
		}
		//колонка 7 имя задачи
		if(iscolvisible(column++))
		    cs->append(attr,"  %s", name->getsvalue()); 
		//добавляем сформированную строку и поле данных с именем задачи (для селектора)
		addstring(strdup(name->getsvalue()),cs);
		free(sproject);
	    }
	} //цикл списка задач
    }
    srv->statedom.releaseptr(tmpstatedom);
}


void TaskWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NSelectList::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_UP:
		selectorup();
		break;
	    case KEY_DOWN:
		selectordown();
		break;
	    case 'S':
	    case 's':
		optask('S');
		break;
	    case 'R':
	    case 'r':
		optask('R');
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
    if (ev->type == NEvent::evPROG) //прграммные
    {
	if (ev->cmdcode == evABORTRES) //событие "abort_result"
	{
	    optask('A');
	}
	if (ev->cmdcode == evCOLVIEWCH) //событие изменения видимости колонки
	{
	    TuiEvent* ev1 = (TuiEvent*) ev;
	    if (iscolvisible(ev1->idata1))
		coldisable(ev1->idata1);
	    else
		colenable(ev1->idata1);
	}
	if (ev->cmdcode == evVIEWMODECH)
	{
	    TuiEvent* ev1 = (TuiEvent*) ev;
	    taskslistmode = ev1->idata1;
	    saveopttoconfig();
	    setstartindex(0);	//отображать с начала
	    selectedindex = -1;	//указатель
	}
	if (ev->cmdcode == evSORTMODECH)
	{
	    TuiEvent* ev1 = (TuiEvent*) ev;
	    taskssortmode = ev1->idata1;
	    saveopttoconfig();
	}
    }
    if (ev->type == NEvent::evTIMER) //таймер
    {
	updatedata(); 	//запросить данные с сервера
	refresh();	//перерисовать окно
    }
}


void TaskWin::optask(char op)
{
    char* name = (char*)getselectedobj();
    if (name == NULL)
	return;
    Item* result = srv->findresultbyname(name);
    if (result == NULL)
	return;
    if ((op=='S')&&(result->findItem("suspended_via_gui") == NULL)) //задача НЕ suspend via gui
	srv->optask(result,"suspend_result");
    if ((op=='R')&&(result->findItem("suspended_via_gui") != NULL)) //задача suspend via gui
	srv->optask(result,"resume_result");
    if (op=='A')
	srv->optask(result,"abort_result");
}
