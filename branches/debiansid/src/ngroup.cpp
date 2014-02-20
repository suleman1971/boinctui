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
#include "ngroup.h"


NGroup::~NGroup()
{
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++)
    {
	delete (*it);
    }
}


NView* NGroup::getitembyid(const char* id) //получить эл-т зная id его класса
{
    NView* result = NULL;
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++)
    {
	if (strcmp((*it)->getid(), id) == 0)
	{
	    result = (*it);
	    break;
	}
    }
    return result;
}


bool NGroup::destroybyid(const char* id) //удалить (с деструкцией) эл-т зная id его класса
{
    bool result = false;
    NView* item = getitembyid(id);
    if (item != NULL)
    {
	remove(item);
	delete(item);
	result = true;
    }
    return result;
}


void NGroup::setneedrefresh()
{
    NView::setneedrefresh();
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перерисовать все подэлементы
    {
	(*it)->setneedrefresh();
    }
}


void NGroup::refresh()
{
    NView::refresh(); //предок
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перерисовать все подэлементы
    {
	(*it)->refresh();
    }
}


void NGroup::move(int begrow, int begcol)
{
    //перемещаем само окно
    NView::move(begrow, begcol); //предок
    //перемещаем содержимое (относительно этого окна)
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перместить все подэлементы
    {
	(*it)->move((*it)->getbegrow(),(*it)->getbegcol());
    }
}


void NGroup::centermodalitems(int maxy, int maxx) //центрировать все модальные эл-ты (maxy,maxx -размер экрана)
{
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //ищем модальные подэлементы
    {
	if ((*it)->ismodal())
	    (*it)->move(maxy/2-(*it)->getheight()/2,maxx/2-(*it)->getwidth()/2);
    }
}


void NGroup::eventhandle(NEvent* ev)	//обработчик событий
{
    //если событие уже кем-то обработано, то просто выходим
    if (ev->done)
	return;
    //посылаем событие всем своим подэлементам (последние вставленные
    //получат первыми. Если один из подэл-тов обработал, то выходим
    std::list<NView*>::reverse_iterator rit;
    for(rit = items.rbegin(); rit != items.rend(); rit++) //отправить всем подэлементам
    {
	(*rit)->eventhandle(ev);
	if (ev->done)
	    return; //прекращаем если событие обработано
    }
    //раз подэл-ты не обработали пытаемся обработать самостоятельно
    NView::eventhandle(ev); //предок
}
