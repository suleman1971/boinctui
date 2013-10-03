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

#include "nscrollview.h"

NScrollView::~NScrollView()
{
    clearcontent();
}

void NScrollView::addstring(int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    NColorString* cs = new NColorString(attr, fmt, args);
    va_end(args);
    addstring(cs);
}


void NScrollView::clearcontent()
{
    std::vector<NColorString*>::iterator it;
    for (it = content.begin(); it != content.end(); it++)
    {
	delete (*it);
    }
    content.clear();
    needrefresh = true;
}


void NScrollView::drawcontent() //отрисовывает буфер строк
{
    //выводим строки начиная со startindex
    for (int line = 0; line < getheight(); line++) //цикл по экранным строкам
    {
	if (startindex+line < content.size())
	{
	    NColorString* cstring = content[startindex + line]; 
	    std::list<NColorStringPart*>::iterator it;
	    wmove(win,line,0);
	    for (it = cstring->parts.begin(); it != cstring->parts.end(); it++) //цикл по частям тек строки
	    {
		NColorStringPart* part = *it;
		wattrset(win,part->attr); //включаем атрибут
		wprintw(win,"%s",part->s.c_str());
//		wattrset(win,0); //отключаем атрибут
	    }
	    wclrtoeol(win); //очищаем до конца строки
	}
	else //очищаем нижнюю незанятую часть окна (если есть)
	{
	    wmove(win,line,0);
	    wclrtoeol(win); //очищаем до конца строки
	}
    }
}


void NScrollView::refresh()
{
    if (needrefresh)
    {
	drawcontent();
	NView::refresh();
    }
}


void NScrollView::resize(int rows, int cols)
{
    NView::resize(rows, cols);
    if (autoscroll)
    {
	setautoscroll(true); //костыль чтобы при ресайзе переустановилась позиция при автоскроле
    }
}


void NScrollView::scrollto(int delta)//сдвинуть отображение на drlta строк вверх или вниз
{
    int oldstartindex = startindex;
    startindex = startindex + delta;
    if ( startindex < 0 )
	startindex = 0;
    if ( startindex > content.size()-getheight() )
	startindex = content.size()-getheight()/* + 1*/; //+1 чтобы оставлять пустую строку
    if (oldstartindex != startindex) //позиция изменилась нужно перерисовываться
	needrefresh = true;
};


void NScrollView::setautoscroll(bool b) //true чтобы включить автоскроллинг
{
    int oldstartindex = startindex;
    autoscroll = b;
    if (b)
	startindex = content.size()-getheight();
    if (oldstartindex != startindex) //позиция изменилась нужно перерисовываться
	needrefresh = true;
};


void NScrollView::eventhandle(NEvent* ev) //обработчик событий
{
    NView::eventhandle(ev); //предок
    if ( ev->done )
	return;
    //реакция на мышь
    NMouseEvent* mevent = (NMouseEvent*)ev;
    if ( ev->type == NEvent::evMOUSE )
    {
	//скроллинг по колесу
	if (isinside(mevent->row, mevent->col))
	{
	    //колесо вверх
	    #if NCURSES_MOUSE_VERSION > 1
	    if (mevent->cmdcode & BUTTON4_PRESSED) //NOT TESTED
	    #else
	    if (mevent->cmdcode & BUTTON4_PRESSED)
	    #endif
	    {
		scrollto(-getheight()/2); //вверх на полокна
		setautoscroll(false);
		ev->done = true;
	    }
	    //колесо вниз
	    #if NCURSES_MOUSE_VERSION > 1
	    if (mevent->cmdcode & BUTTON5_PRESSED) //NOT TESTED
	    #else
	    if ( mevent->cmdcode & (BUTTON2_PRESSED | REPORT_MOUSE_POSITION)) //REPORT_MOUSE_POSITION подпорка иначе теряет эвенты при быстрой прокрутке вниз
	    #endif
	    {
		if (!getautoscroll())
		{
		    int oldpos = getstartindex();
		    scrollto(getheight()/2); 	//вниз на пол окна
		    if ( oldpos == getstartindex()) 	//позиция не изменилась (уже достигли конца)
			setautoscroll(true);	//включаем автоскроллинг
		    ev->done = true;
		}
	    }
	}
    }
    //клавиатура
/*
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
    }
*/
    if (ev->done) //если обработали, то нужно перерисоваться
	refresh();
}

