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

#include "kclog.h"
#include "taskinfowin.h"


void Tree2Text(Item* item, std::vector<std::pair<std::string, std::string> >& vout, int& maxlen1, int& maxlen2)
{
    std::vector<Item*> v = item->getItems(""); //получить все эл-ты
    std::vector<Item*>::iterator it2;
    for (it2 = v.begin(); it2!=v.end(); it2++) //цикл по эл-там задачи
    {
	if ((*it2)->isnode())
	{
	    vout.push_back(std::pair<std::string, std::string>( "", "")); //разделитель
	    Tree2Text(*it2, vout, maxlen1, maxlen2); //рекурсия
	    vout.push_back(std::pair<std::string, std::string>( "", "")); //разделитель
	}
	else
	{
	    if (strlen((*it2)->getname()) > maxlen1)
		maxlen1 = strlen((*it2)->getname());
	    if (strlen((*it2)->getsvalue()) > maxlen2)
		maxlen2 = strlen((*it2)->getsvalue());
	    vout.push_back(std::pair<std::string, std::string>((*it2)->getname(), (*it2)->getsvalue()));
	}
    }
}


// =============================================================================


TaskInfoWin::TaskInfoWin(const char* caption, Srv* srv, const char* projecturl, const char* taskname)
:	NGroup(NRect(getmaxy(stdscr) - 10, getmaxx(stdscr) - 60, 3, 3))
{
    modalflag = true;
    this->srv = srv;
    this->projecturl = projecturl;
    this->taskname = taskname;
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
    box(win,0,0);
    mvwprintw(win,0,getwidth()/2-(strlen(caption)/2), this->caption.c_str());
    refresh();
}


void TaskInfoWin::updatedata()
{
    if (srv == NULL)
	return;
    //===данные по процессам===
    if (srv->statedom.empty())
	return;
    Item* tmpstatedom = srv->statedom.hookptr();
    Item* client_state = tmpstatedom->findItem("client_state");
    std::vector<std::pair<std::string, std::string> > ss;
    int maxlen1 = 0;
    int maxlen2 = 0;
    if (client_state != NULL)
    {
	content->clearcontent();
        std::vector<Item*> results = client_state->getItems("result");
	std::vector<Item*>::iterator it;
	for (it = results.begin(); it!=results.end(); it++) //цикл списка задач
	{
	    Item* project_url = (*it)->findItem("project_url");
	    Item* name = (*it)->findItem("name");
	    if ((project_url != NULL)&&
		(name != NULL)&&
		(strcmp(project_url->getsvalue(), projecturl.c_str()) == 0)&&
		(strcmp(name->getsvalue(), taskname.c_str()) == 0))
	    {
		Tree2Text(*it, ss, maxlen1, maxlen2);
		break;
	    }
	} //цикл списка задач
	needrefresh = true;
    }
    srv->statedom.releaseptr(tmpstatedom);
    //заполняем визуальные строки
    std::vector<std::pair<std::string, std::string> >::iterator it;
    for (it = ss.begin(); it!=ss.end(); it++) //цикл списка задач
    {
	content->addstring(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, "%-*s : %s\n", maxlen1, (*it).first.c_str(), (*it).second.c_str());
    }
}


void TaskInfoWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;
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
