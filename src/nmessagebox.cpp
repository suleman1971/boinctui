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
#include <math.h>
#include "kclog.h"
#include "nmessagebox.h"


// =============================================================================


NMButton::NMButton(const char* text, NEvent* pevent, ...) : NStaticText(NRect(3, 20, getmaxy(stdscr)/2-3/2,getmaxx(stdscr)/2-20/2))
{

    appendstring(getcolorpair(COLOR_WHITE, COLOR_CYAN)," ");
    appendstring(getcolorpair(COLOR_WHITE, COLOR_CYAN) | A_BOLD ,text);
    appendstring(getcolorpair(COLOR_WHITE, COLOR_CYAN)," ");
    this->pevent = pevent;
    //размер кнопки
    resize(1, mbstrlen(text) + 2);
    //список клавиш на которые реагирует эта кнопка
    va_list pk;
    va_start(pk, pevent);
    int ch = va_arg(pk, int);
    while ( ch != 0)
    {
	keys.push_back((char)ch);
	ch = va_arg(pk, int); //следующий ключ
    }
    va_end(pk);
}


void NMButton::eventhandle(NEvent* ev) 	//обработчик событий
{
    NStaticText::eventhandle(ev); //предок
    if ( ev->done )
	return;
    //одиночный или двойной клик
    NMouseEvent* mevent = (NMouseEvent*)ev;
    if (( ev->type == NEvent::evMOUSE ) && (mevent->cmdcode & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED)))
    {
	if (isinside(mevent->row, mevent->col))
	{
		ev->done = true;
		NEvent* tmp = pevent;
		pevent = NULL;
		putevent(tmp); //активируем событие связанное с этой кнопкой
	}
    }
    //клавиатура
    if ( ev->type == NEvent::evKB )
    {
	if ( keys.end() != std::find(keys.begin(), keys.end(), ev->cmdcode) )
	{
	    kLogPrintf("NMButton::eventhandle() got '%c' key\n", ev->cmdcode);
	    if (pevent)
	    {
		ev->done = true;
		NEvent* tmp = pevent;
		pevent = NULL;
		putevent(tmp); //активируем событие связанное с этой кнопкой
	    }
	}
    }
}


// =============================================================================


NMessageBox::NMessageBox(const char* text) : NGroup(NRect(3, 40, 1, 1))
{
    //расчитать сколько строк нужно для отображения контента
    int contentheight = 0;
    int bsize = strlen(text); //количество байт
    int result = 0; //подсчитанное кол-во символов
    int nbytes = 0; //просмотренное кол-во байтов
    int nlines = 0; //количество экранных строк
    int col = getwidth() - 4;
    const char* p = text;
    do
    {
	col++;
	if ((col >= getwidth() - 4)||(*p == '\n'))
	{
	    if (*p == '\n')
		col = 0;
	    else
		col = 1;
	    contentheight++; //след строка
	}
	int symlen = mblen(p,bsize-nbytes);
	nbytes = nbytes + symlen;
	result++;
	p = p + symlen; //адрес начала след символа
    }
    while ( (*p != 0)&&(nbytes < bsize) ); //дошли до конца
    //заполняем содержимое
    content = new NStaticText(NRect(contentheight, getwidth()-4, 2, 2));
    content->setbgcolor(getcolorpair(COLOR_WHITE, COLOR_BLACK));
    insert(content);
    content->appendstring(getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD, text);
    modalflag = true;
    resize(contentheight + 6,getwidth());
    wattrset(win,getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD);
    box(win,0,0);
    //content->setalign(1);
    move(getmaxy(stdscr)/2-getheight()/2,getmaxx(stdscr)/2-getwidth()/2); //центрируем

}


void NMessageBox::addbutton(NMButton* button) //добавить кнопку
{
    insert(button);

    std::list<NView*>::iterator it;
    int w = 0; //суммарная ширина всех кнопок
    for (it = ++items.begin(); it != items.end(); it++) // ++ пропустить content
    {
	w = w + (*it)->getwidth() + 2; // +2 промежутки между кнопками
    }
    int row = getheight() - 3;
    int col = (getwidth() - w) / 2 + 2;
    for (it = ++items.begin(); it != items.end(); it++)
    {
	(*it)->move(row, col);
	col = col + (*it)->getwidth() + 2;
    }
}


void NMessageBox::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок

    if ( ev->done )
	return;
    //одиночный или двойной клик
    NMouseEvent* mevent = (NMouseEvent*)ev;
    if (( ev->type == NEvent::evMOUSE )/* && (mevent->cmdcode & (BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED))*/)
    {
	//блокируем все мышиные мнутри окна
//	if (isinside(mevent->row, mevent->col))
	    ev->done = true;
    }
    //клавиатура
    if (ev->type == NEvent::evKB )
    {
	//блокировать все клавиатурные кроме кода закрытия формы
	if (ev->keycode != 27)
	    ev->done = true;
    }
}

