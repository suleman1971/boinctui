#include <string.h>
#include <algorithm>
#include <sstream>
#include "taskwin.h"
#include "net.h"
#include "resultparse.h"
#include "mbstring.h"


bool resultCmpLess( Item* res1, Item* res2 ) //для сортировки задач true если res1 < res2
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


std::string getresultstatestr(Item* result)
{
    Item* state = result->findItem("state");
//    Item* ready_to_report = result->findItem("ready_to_report");
    Item* active_task_state = result->findItem("active_task_state");
    if (result->findItem("ready_to_report") != NULL) //расчет завершен
	return "Done";
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


std::string getestimatetimestr(Item* result) //получить в виде строки прогнозируемое время завершения задачи
{
    Item* estimated_cpu_time_remaining = result->findItem("estimated_cpu_time_remaining");
    std::stringstream s;
    if ( estimated_cpu_time_remaining != NULL )
    {
	double dtime = estimated_cpu_time_remaining->getdvalue();
	time_t time = dtime; //берем только целую часть
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
//	s << t->tm_yday << "d" << t->tm_hour << "h" << t->tm_min << "m"<< t->tm_sec << "s";
	return s.str();
    }
    return "inf";
}


TaskWin::~TaskWin()
{
    clearcontent();
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
    srv->updatestate(); //обновляем данные в контейнере
    clearcontent();
    if (srv->statedom == NULL)
	return;
    Item* client_state = srv->statedom->findItem("client_state");
    int i = 1; //счетчик заданий строк
    if (client_state != NULL)
    {
        std::vector<Item*> results = client_state->getItems("result");
	std::vector<Item*>::iterator it;
	std::sort(results.begin(), results.end(), resultCmpLess); //сортируем
	
	for (it = results.begin(); it!=results.end(); it++,i++) //цикл списка задач
	{
	    Item* name  = (*it)->findItem("name");
	    Item* fraction_done = (*it)->findItem("fraction_done");
	    if (name)
	    {
		char sdone[64];
		if (!fraction_done) //для неактивных секция fraction_done отсутствует
		    strcpy(sdone,"  --  ");
		else
		    sprintf(sdone,"%6.2f",100*fraction_done->getdvalue());
		//процент выполнения
		std::string sstate = getresultstatestr(*it);
		//имя проекта
		std::string pname = srv->findProjectName(srv->statedom, (*it)->findItem("project_url")->getsvalue());//findProjectName(srv->statedom, *it);
		char* sproject = strdup(pname.c_str());
		//цвет и атрибут
		int attr = A_NORMAL;
		if ((*it)->findItem("ready_to_report") != NULL)
		    attr = getcolorpair(COLOR_BLACK,COLOR_BLACK) | A_BOLD;
		if ((*it)->findItem("active_task") != NULL)
		    attr = A_BOLD;
		if ( sstate == "Run")
		    attr = getcolorpair(COLOR_MAGENTA,COLOR_BLACK) | A_BOLD;
		NColorString* cs = new NColorString(attr, " %2d  %-6s  %6s  %-20s ",i, sstate.c_str(), sdone, mbstrtrunc(sproject,20));
		Item* estimated_cpu_time_remaining = (*it)->findItem("estimated_cpu_time_remaining");
		if ( estimated_cpu_time_remaining != NULL )
		{ 
		    double dtime = estimated_cpu_time_remaining->getdvalue();
		    if ( ( sstate == "Run" )&&( dtime < 3600) ) //меньше часа
			cs->append(getcolorpair(COLOR_RED,COLOR_BLACK) | A_BOLD,"%4s", getestimatetimestr(*it).c_str());
		    else
			cs->append(attr,"%4s", getestimatetimestr(*it).c_str());
		}
		cs->append(attr,"   %s", name->getsvalue());
		//addstring(strdup(name->getsvalue()), attr, " %2d  %-6s  %6s  %-20s  %4s  %s",i, sstate.c_str(), sdone, mbstrtrunc(sproject,20), getestimatetimestr(*it).c_str(), name->getsvalue());
		addstring(strdup(name->getsvalue()),cs);
		free(sproject);
	    }
	} //цикл списка задач
    }
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
		suspendresumetask('S');
		break;
	    case 'R':
	    case 'r':
		suspendresumetask('R');
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}


void TaskWin::suspendresumetask(char op)
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
}
