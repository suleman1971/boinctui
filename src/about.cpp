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
#include "about.h"
#include "tuievent.h"
#include "commondef.h"


AboutWin::AboutWin(int rows, int cols) : NGroup(NRect(rows, cols, getmaxy(stdscr)/2-rows/2,getmaxx(stdscr)/2-cols/2))
{
    modalflag = true;
    caption = strdup(" BOINCTUI ");
    resize(12,getwidth());
    wattrset(win,getcolorpair(COLOR_WHITE, getbgcolor()) | A_BOLD);
    if(asciilinedraw == 1)
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    else
	box(win,0,0);
    mvwprintw(win,0,getwidth()/2-(strlen(caption)/2),"%s",caption);
    char buf[1024];
    snprintf(buf,sizeof(buf),"%s ver %s","BOINC Client manager", XSTR(VERSION));
    mvwprintw(win,3,getwidth()/2-(strlen(buf)/2),"%s",buf);
    mvwprintw(win,5,getwidth()/2-(strlen("(c) Sergey Suslov")/2),"(c) Sergey Suslov");
    mvwprintw(win,6,getwidth()/2-(strlen("suleman1971@gmail.com")/2),"suleman1971@gmail.com");
    mvwprintw(win,7,getwidth()/2-(strlen("Community PRs included by")/2),"Community PRs included by");
    mvwprintw(win,8,getwidth()/2-(strlen("Mark Pentler - GitHub: mpentler")/2),"Mark Pentler - GitHub: mpentler");
}


void AboutWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;
    //закрываем при любом клике независимо от координат
    NMouseEvent* mevent = (NMouseEvent*)ev;
    if (( ev->type == NEvent::evMOUSE ) && (mevent->cmdcode & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED)))
	putevent(new TuiEvent(evABOUT));

    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
	switch(ev->cmdcode)
	{
	    case 27:
	    case KEY_ENTER:
	    case ' ':
	    case '\n':
		putevent(new TuiEvent(evABOUT)); //NEvent(NEvent::evPROG, 3)); //создаем событие с кодом 3 "окно About"
		break;
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}
