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

#ifndef NVIEW_H
#define NVIEW_H

#include <typeinfo>
#if HAVE_LIBNCURSESW == 1 && NCURSESW_HAVE_SUBDIR == 1
    #include <ncursesw/panel.h>
#else
    #include <panel.h>
#endif
#include "nrect.h"
#include "nevent.h"

class NGroup;


void initcolorpairs();
int getcolorpair(int fcolor, int bcolor); //получить пару для комбинации цветов
int getbgcolor(); //вернет цвет бакграунда (в зависимости от настройки transparentbg)
extern int asciilinedraw; //1-рисовать рамки символами +----+
extern int transparentbg; //1-прозрачный бэкграунд


class NView //базовый визуальный класс
{
  public:
    NView(NRect rect);
    virtual ~NView() { del_panel(pan); delwin(win); };
    //virtual void draw() {};
    virtual void resize(int rows, int cols);
    virtual void move(int begrow, int begcol);
    PANEL* getpan() 	{ return pan; }; //!!!!!!!!!!!!!!!!!!!!
    int getwidth()  	{ return rect.cols; }; 	//ширина в символах
    int getheight() 	{ return rect.rows; }; 	//высота в строках
    int getbegrow()	{ return rect.begrow; }; //начальная строка
    int getbegcol()	{ return rect.begcol; }; //начальный столбец
    void erase() 	{ werase(win); }; 	//очистить
    bool isinside(int row, int col); //true если координаты внутри окна (row col абсолютные!)
    virtual void refresh(); 	//перерисовать
    virtual void setneedrefresh() { needrefresh = true; };
    virtual void eventhandle(NEvent* ev) {/*EMPTY*/}; 	//обработчик событий
    virtual void putevent(NEvent* ev); //отправить событие по цепочке владельцев в очередь
    void setowner(NGroup* owner);
    virtual const char* getid() { return typeid(*this).name(); }; //возвращает имя класса
    virtual bool ismodal() { return modalflag; };
  protected:
    NGroup*	owner;	//владелец эл-та
    WINDOW* 	win;	//окно curses
    PANEL*	pan;	//панель curses на основе окна win
    bool needrefresh; //устанавливается в true когда нужна перерисовка
    #ifdef DEBUG
    int		refreshcount; //счетчик обновлений
    #endif
    bool	modalflag; //true если этот эл-т модальный
    int getabsbegrow(); //получить начальную строку (абсолютную на экране)
    int getabsbegcol(); //получить начальный столбец (абсолютный на экране)
  private:
    NRect	rect;	//координаты и размер

};

#endif //NVIEW_H