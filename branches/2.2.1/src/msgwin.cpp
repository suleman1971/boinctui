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
#include "msgwin.h"
#include "net.h"
#include "resultparse.h"


void MsgWin::updatedata() //обновить данные с сервера
{
    if (srv == NULL)
	return;
    srv->updatemsgs(); //обновить данные в хранилище
    if (srv->msgdom == NULL)
    {
	clearcontent();
	return;
    }
    if (lastmsgno == srv->lastmsgno) //в srv есть новые еще не отрисованные сообщения?
	return;
    // === дополняем массив визуальных строк ===
    if (lastmsgno == 0)
        clearcontent(); //очищаем если отображение идет с начала
    Item* msgs = srv->msgdom->findItem("msgs");
    if (msgs != NULL)
    {
	std::vector<Item*> mlist = msgs->getItems("msg");
	std::vector<Item*>::iterator it;
	for (it = mlist.begin(); it != mlist.end(); it++) //цикл по сообщениям
	{
	    Item* number = (*it)->findItem("seqno"); 	//номер текущего сообщения
	    if (number->getivalue()<=lastmsgno)
		continue; //пропускаем старые (уже добавленные ранее) сообщения
	    Item* body = (*it)->findItem("body"); 	//текст сообщения
	    Item* time = (*it)->findItem("time"); 	//время сообщения
	    if ((body != NULL)&&(time != NULL))
	    {
		time_t t = time->getivalue(); 		//время в секундах от 1.1.1970
		tm* ltime = localtime(&t);
		char tbuf[128];
		strftime(tbuf, sizeof(tbuf),"%-e %b %-k:%M",ltime);
		Item* project = (*it)->findItem("project");
		std::string sproject = "_no_";
		if (project != NULL)
		    sproject = project->getsvalue();
		addstring(getcolorpair(COLOR_CYAN,COLOR_BLACK) /*| A_BOLD*/,"#%d %s ", number->getivalue(), tbuf); //номер и время сообщения
		content.back()->append(getcolorpair(COLOR_YELLOW,COLOR_BLACK),"%s",sproject.c_str()); //добавить имя проекта другим цветом
		addstring(getcolorpair(COLOR_WHITE,COLOR_BLACK), "%s", body->getsvalue()); //само сообщение
	    }
	}
    }
    lastmsgno = srv->lastmsgno;
    //поскольку есть новые сообщения, то делаем автоскроллинг в конец
    setautoscroll(true);
}


void MsgWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NScrollView::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_PPAGE:
		//wprintw(win,"PgUp");
		scrollto(-getheight()/2); //вверх на полокна
		setautoscroll(false);
		break;
	    case KEY_NPAGE:
		//wprintw(win,"PgDn");
		if (!getautoscroll())
		{
		    int oldpos = startindex;
		    scrollto(getheight()/2); 	//вниз на пол окна
		    if ( oldpos == startindex) 	//позиция не изменилась (уже достигли конца)
			setautoscroll(true);	//включаем автоскроллинг
		}
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}
