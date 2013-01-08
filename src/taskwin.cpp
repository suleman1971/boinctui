#include <string.h>
#include <algorithm>
#include <sstream>
#include "taskwin.h"
#include "net.h"
#include "resultparse.h"
#include "mbstring.h"
#include "tuievent.h"
#include "kclog.h"


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


//std::string getestimatetimestr(Item* result) //получить в виде строки прогнозируемое время завершения задачи
std::string gethumanreadabletimestr(time_t time) //получить в виде строки прогнозируемое время завершения задачи
{
//    Item* estimated_cpu_time_remaining = result->findItem("estimated_cpu_time_remaining");
    std::stringstream s;
//    if ( estimated_cpu_time_remaining != NULL )
//    {
//	double dtime = estimated_cpu_time_remaining->getdvalue();
//	time_t time = dtime; //берем только целую часть
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
//    }
//    return "inf";
}


TaskWin::TaskWin(NRect rect, Config* cfg) : NSelectList(rect) 
{
    setselectorbgcolor(COLOR_CYAN);
    columnmask = ~0;
    //читаем маску из конфига если ее нет то создаем
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
	}
    }
    this->cfg = cfg;
};


TaskWin::~TaskWin()
{
    kLogPrintf("TaskWin::~TaskWin()\n");
    clearcontent();
}


void TaskWin::savemasktoconfig() //сохранить маску в дереве конфига
{
    //пишем в конфиг маску
    if (cfg != NULL)
    {
	Item* rootcfg = cfg->getcfgptr();
	if (rootcfg != NULL)
	{
	    Item* column_view_mask = rootcfg->findItem("column_view_mask");
	    if (column_view_mask != NULL)
		column_view_mask->setivalue(columnmask);
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
		//колонка 0 номер задачи
		if(iscolvisible(column++))
		    cs->append(attr, " %2d  ", i);
		//колонка 1 состояние
		char sdone[64];
		if (!fraction_done) //для неактивных секция fraction_done отсутствует
		    strcpy(sdone,"   -  ");
		else
		    sprintf(sdone,"%6.2f",100*fraction_done->getdvalue());
		if (iscolvisible(column++))
		    cs->append(stateattr, "%-6s", sstate.c_str());
		//колонка 2 процент выполнения имя проекта и подвсетка для GPU задач
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
		std::string pname = srv->findProjectName(srv->statedom, (*it)->findItem("project_url")->getsvalue());//findProjectName(srv->statedom, *it);
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
