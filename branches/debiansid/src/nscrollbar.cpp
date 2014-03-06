// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013,2014 Sergey Suslov
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


#include <math.h>
#include "nscrollbar.h"
#include "kclog.h"


NScrollBar::NScrollBar(NRect rect, chtype chtop, chtype chbottom, chtype chinactive) : NView(rect)
{
    bgcolor = 0;
    this->chtop = chtop;
    this->chbottom = chbottom;
    this->chinactive = chinactive;
    setvisible(true);
}


void NScrollBar::setpos(int vmin, int vmax, int vpos1, int vpos2)
{
    //если видимая часть больше или равна контенту то выключаем
    setvisible(!((vpos1<=vmin)&&(vpos2>=vmax)));
    if ((this->vmin!=vmin)||(this->vmax!=vmax)||(this->vpos1!=vpos1)||(this->vpos2!=vpos2))
    {
	this->vmin=vmin;
	this->vmax=vmax;
	this->vpos1=vpos1;
	this->vpos2=vpos2;
	if (vmin>vmax)
	    vmax = vmin;
	if (vpos1 > vpos2)
	    vpos2 = vpos1;
	if (vpos1 < vmin)
	    vpos1 = vmin;
	if (vpos2 > vmax)
	    vpos2 = vmax;
	//kLogPrintf("NScrollBar::setpos(vmin=%d, vmax=%d, vpos1=%d, vpos2=%d)\n",vmin, vmax, vpos1, vpos2);
	refresh();
    }
}


void NScrollBar::refresh()
{
    wbkgd(win,bgcolor);
    wattrset(win,getcolorpair(COLOR_WHITE,COLOR_BLACK));
    chtype topsymbol = asciilinedraw ? '+'| (chtop & A_BOLD) : chtop;
    chtype bottomsymbol = asciilinedraw ? '+' | (chbottom & A_BOLD) : chbottom;
    chtype bodysymbol = asciilinedraw ? 'X' : ACS_CKBOARD;
    if (!visible)
	bodysymbol = asciilinedraw ? '|' | (chinactive & A_BOLD) : chinactive;
    int rowmin = 0; //строка с которой начинаем рисовать фон
    int rowmax = getheight() - 1; //строка до которой рисуем фон включительно
    if (chtop != 0) //верхний концевой символ есть
    {
	mvwaddch(win,0,0,topsymbol);
	rowmin = 1;
    }
    if (chbottom != 0)//нижний концевой символ есть
    {
	mvwaddch(win,getheight()-1,0,bottomsymbol);
	rowmax--; //укорачиваем
    }
    //фоновый заполнитель
    int len = rowmax - rowmin + 1; //высота заполнителя в символах
    mvwvline(win, rowmin, 0, bodysymbol, len);
    if (visible)
    {
    //отрисовка позиции
    if ((vpos2>vpos1)&&(vmax>vmin))
    {
	double scale = double(len-1)/(vmax-vmin); //сколько единиц на экранную строку
	int len2 = round(scale * (vpos2-vpos1)); //число выделенных строк
	int rowpos1 = rowmin + round(scale * (vpos1-vmin)); //строка начала выделения
	if (len2<1) //len2 всегда 1 и более
	    len2=1;
	//подпорки с краем диапазона чтобы были видны малые фрагменты
	//в начале и в конце
	if ((rowpos1==rowmin)&&(vpos1>vmin))
	{
	    if (rowpos1<rowmax)
		rowpos1++; //слегка сдвигаем вниз чтобы показать что это не начало
	}
	if ((rowpos1+len2-1>=rowmax)&&(vpos2<vmax))
	{
	    if(rowpos1>rowmin)
		rowpos1--; //слегка приподнять вверх чтобы показать что это не конец
	}
	if (vpos2==vmax)
	    len2=len-rowpos1+1;
	//kLogPrintf("len=%d vmin=%d vmax=%d vpos1=%d vpos2=%d  scale=%f rowmin=%d rowmax=%d rowpos1=%d len2=%d\n",len, vmin,vmax,vpos1,vpos2,scale,rowmin,rowmax,rowpos1,len2);
	//рисуем выделение
	mvwvline(win,rowpos1,0,' ' | getcolorpair(COLOR_CYAN,COLOR_WHITE), len2);
    }
    }
    NView::refresh();
}
