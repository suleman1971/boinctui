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

#include <stdlib.h>
#include <string.h>
#include "nselectlist.h"
#include "kclog.h"


void NSelectList::addstring(void* userobj, int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    NColorString* cs = new NColorString(attr, fmt, args);
    va_end(args);
    NScrollView::addstring(cs);
    objects.push_back(userobj);
}


void NSelectList::addstring(void* userobj, NColorString* cstring)
{
    NScrollView::addstring(cstring);
    objects.push_back(userobj);
}


void NSelectList::drawcontent() //отрисовывает буфер строк
{
    //выводим строки начиная со startindex
    for (int line = 0; line < getheight(); line++) //цикл по экранным строкам
    {
	if (startindex+line < content.size()) //цикл по строкам
	{
	    NColorString* cstring = content[startindex + line];
	    std::list<NColorStringPart*>::iterator it;
	    wmove(win,line,0);
	    for (it = cstring->parts.begin(); it != cstring->parts.end(); it++) //цикл по частям тек строки
	    {
		NColorStringPart* part = *it;
		if ((!selectorenable)||(startindex + line != selectedindex)) //эта строка не выделена или селектор выключен
		    wattrset(win,part->attr); //включаем атрибут
		else
		{
		    //получаем из атрибута цвета для текста и фона
		    short f,b;
		    f = b = 0;
		    unsigned int a = 0;
		    a = a | (part->attr & A_BOLD); //нужен-ли аттрибут
		    pair_content(PAIR_NUMBER(part->attr),&f,&b); //цвета тек куска для пары с номером PAIR_NUMBER(part->attr)
		    //kLogPrintf("part->attr=%X PAIR_NUMBER(%X) -> #%d f=%x b=%x attr=%x\n",part->attr, part->attr,PAIR_NUMBER(part->attr),f,b,a);
		    //kLogPrintf("A_BOLD=%X\n",A_BOLD);
		    wattrset(win, getcolorpair(f, selectorbgcolor) | a); //включаем новый цвет и атрибут
		}
		wprintw(win,"%s",part->s.c_str());
	    } //цикл частей одной строки
	    //wattrset(win,0); //отключаем атрибут
	    //очищаем до конца строки
	    if ((!selectorenable)||(startindex + line != selectedindex))
	        wclrtoeol(win);
	    else
	    {
		wbkgdset(win,getcolorpair(COLOR_WHITE,selectorbgcolor));
		wclrtoeol(win); //очищаем до конца строки
		wbkgdset(win,getcolorpair(COLOR_WHITE, COLOR_BLACK));
	    }
	}
	else //очищаем нижнюю незанятую часть окна (если есть)
	{
	    wmove(win,line,0);
	    wclrtoeol(win); //очищаем до конца строки
	}
    }
}


void NSelectList::refresh()
{
    if (needrefresh)
    {
	drawcontent();
	NView::refresh(); //Именно так!!!
    }
}


void* NSelectList::getselectedobj()
{
    if ((selectedindex >= 0)&&(selectedindex < objects.size()))
    {
	return objects[selectedindex];
    }
    else
	return NULL;
}


void NSelectList::selectorup()
{
    if (selectedindex >= 0)
    {
	selectedindex--;
	needrefresh = true;
	//проверяем не нужно ли скролить
	if (selectedindex - startindex < 4) //видимых строк выше серлектора осталось 
	{
	    scrollto(-1);
	}
    }
}


void NSelectList::selectordown()
{
    if (selectedindex < (int)content.size())
    {
	selectedindex++;
	needrefresh = true;
	//проверяем не нужно ли скролить
	if ((startindex + getheight() < content.size())&&(startindex + getheight() - selectedindex < 4)) //видимых строк ниже серлектора осталось 
	{
	    scrollto(1);
	}
    }
}
