// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

#include <string.h>
#include "about.h"
#include "tuievent.h"

//стрингификатор
#define XSTR(S)		STR(S)
#define STR(S)		#S


AboutWin::AboutWin(int rows, int cols) : NGroup(NRect(rows, cols, getmaxy(stdscr)/2-rows/2,getmaxx(stdscr)/2-cols/2))
{
    caption = strdup(" BOINCTUI ");
    resize(10,getwidth());
    wattrset(win,getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD);
    box(win,0,0);
    mvwprintw(win,0,getwidth()/2-(strlen(caption)/2),caption);
    char buf[1024];
    snprintf(buf,sizeof(buf),"%s ver %s","BOINC Client manager", XSTR(VERSION));
    mvwprintw(win,3,getwidth()/2-(strlen(buf)/2),buf);
    mvwprintw(win,5,getwidth()/2-(strlen("(c) 2012,2013 Sergey Suslov")/2),"(c) 2012,2013 Sergey Suslov");
    mvwprintw(win,6,getwidth()/2-(strlen("suleman1971@gmail.com")/2),"suleman1971@gmail.com");
}


void AboutWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;
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
