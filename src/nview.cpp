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

#include "nview.h"
#include "ngroup.h"
#include "kclog.h"


int asciilinedraw = 0; //1-рисовать рамки символами +----+


void initcolorpairs()
{
    kLogPrintf("COLORS=%d    COLOR_PAIRS=%d\n",COLORS,COLOR_PAIRS);
    //COLORS 0..7
    //COLOR_PAIRS 64  [(0),1..63] 0-predfined
    int npair; //номер генерируемой пары
    for ( int b = 0; b < COLORS; b++ ) //фон
    {
	for ( int f = 0; f < COLORS; f++ ) //текст
	{
	    npair = 1 + b * COLORS + f;
	    if ( npair < COLOR_PAIRS )
		init_pair(npair, f, b);
	}
    }
}


int getcolorpair(int fcolor, int bcolor) //получить пару для комбинации цветов
{
    int npair = 1 + bcolor * COLORS + fcolor;
    if ( npair <= COLOR_PAIRS)
	return COLOR_PAIR(npair);
    else
	return COLOR_PAIR(0);
}


NView::NView(NRect rect)
{
    this->rect = rect;
    win = newwin(rect.rows, rect.cols, rect.begrow, rect.begcol); //создаем окно curses
    pan = new_panel(win);
    scrollok(win,false);
    needrefresh = true;
    owner = NULL;
    modalflag = false;
    #ifdef DEBUG
    refreshcount = 0; //счетчик обновлений
    #endif
    wbkgdset(win,getcolorpair(COLOR_WHITE,COLOR_BLACK)); //бакграунд
    werase(win); //заполняем цветом фона
}


void NView::setowner(NGroup* owner)
{
    this->owner = owner;
    //позиционировать относительно овнера
    move(getbegrow(),getbegcol());
}


void NView::resize(int rows, int cols)
{
    wresize(win, rows, cols);
    rect.rows = rows;
    rect.cols = cols;
    needrefresh = true;
}


void NView::refresh() //перерисовать
{
    if (needrefresh)
    {
	#ifdef DEBUG
	refreshcount++;
	mvwprintw(win,0,getmaxx(win)-10,"r=%d",refreshcount);
	#endif
	update_panels();
	//doupdate();
	needrefresh = false;
    }
}


int NView::getabsbegrow() //получить начальную строку (на экране)
{
    if (owner == NULL)
	return getbegrow();
    else
	return getbegrow() + owner->getabsbegrow();
}


int NView::getabsbegcol() //получить начальный столбец (на экране)
{
    if (owner == NULL)
	return getbegcol();
    else
	return getbegcol() + owner->getabsbegcol();
}


bool NView::isinside(int row, int col) //true если координаты внутри окна (row col абсолютные!)
{
    bool result = true;
    int absbegrow = getabsbegrow();
    int absbegcol = getabsbegcol();
    if ((row < absbegrow)||(col < absbegcol))
	result = false;
    if ((row > absbegrow + rect.rows - 1)||(col > absbegcol+ rect.cols - 1))
	result = false;
    return result;
}


void NView::move(int begrow, int begcol)
{
    rect.begrow = begrow;
    rect.begcol = begcol;
    move_panel(pan, getabsbegrow(), getabsbegcol());
}


void NView::putevent(NEvent* ev) //отправить событие по цепочке владельцев в очередь
{
    if (owner != NULL)
	owner->putevent(ev);
}

