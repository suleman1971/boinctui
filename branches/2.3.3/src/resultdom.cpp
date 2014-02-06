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

#include "resultdom.h"
#include <stdio.h>
#include <string.h>


Item::~Item()
{
    std::list<Item*>::iterator it;
    //деструктим подэлементы
    for (it=subitems.begin(); it!=subitems.end(); it++)
    {
	delete *it;
    }
    //очищаем список
    subitems.clear();
}


std::vector<Item*> Item::getItems(const char* name) //получить из текущего эл-та список подэлементов с именем name (поиска в глубину НЕТ)
{
    std::vector<Item*> result;
    std::list<Item*>::iterator it;
    if (strlen(name) > 0)
    {
	for (it=subitems.begin(); it!=subitems.end(); it++)
	{
	    if ( strcmp((*it)->getname(),name) == 0 )
		result.push_back(*it);
	}
    }
    else //если имя не задано вернет все имеющиеся
    {
	for (it=subitems.begin(); it!=subitems.end(); it++)
		result.push_back(*it);
    }
    return result;
}


Item* Item::findItem(const char* name)	//ищет в поддереве (на всю глубину) элемент с именем name (вернет NULL если такого нет)
{
    Item* result;
    if (strcmp(this->name.c_str(), name) == 0)
    {
	return this; //нашли - он сам и есть искомый эл-т
    }
    else //ищем во всех подэлементах
    {
        std::list<Item*>::iterator it;
        for (it=subitems.begin(); it!=subitems.end(); it++)
	{
	    result = (*it)->findItem(name);
	    if (result != NULL)
		return result; //нашли!
	}
    }
    return NULL; //не нашли
}


void Item::setivalue(int ivalue) //присвоить целочисленное значение
{
    char buff[256];
    snprintf(buff,sizeof(buff),"%d",ivalue);
    setsvalue(buff);
}


double Item::getdvalue()		//получить значение double
{
    double result = 0.0;
    std::string numlocale = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C"); //чтобы работала sscanf с числами вида 1.234 а не 1,234
    sscanf(svalue.c_str(), "%lf", &result);
    setlocale(LC_NUMERIC, numlocale.c_str());
    return result;
}


void Item::mergetree(Item* tree) //объединяет tree с текущим эл-том
{
    if (tree->getfullname() == this->getfullname()) //полные пути одинаковые
    {
	// Сливаем два узла в один
	std::list<Item*>::iterator it;
	for (it = tree->subitems.begin(); it != tree->subitems.end(); it++)
	{
	    addsubitem(*it); //все подэлементы из tree переносим в текущий узел
	}
	tree->subitems.clear();
	//delete tree; //сам узел уже не нужен (все его эл-ты перенесены в this)
    }
    //else
    //{
	// добавляем tree к текущему узлу
    //	addsubitem(tree);
    //}
}


void Item::clearsubitems() //удалить все подэл-ты текущего узла
{
    std::list<Item*>::iterator it;
    for (it = subitems.begin(); it != subitems.end(); it++)
        delete (*it);
    subitems.clear();
}


void Item::delsubitem(Item* subitem) //удалить подэлемент subitem из узла
{
    std::list<Item*>::iterator it;
    for (it = subitems.begin(); it != subitems.end(); it++)
    {
        if ((*it) == subitem)
	{
	    delete (*it);
	    subitems.remove(*it);
	    break;
	}
    }
}


std::string Item::toxmlstring() //сериализация в строку
{
    static int indent;
    std::string result = "";
    std::string sindent = ""; //отступ
    for (int i  = 0; i < indent; i++)
	sindent = sindent + "    ";
    indent++;
    result = sindent + "<" + name + ">" + svalue + "\n";
    std::list<Item*>::iterator it;
    for (it=subitems.begin(); it!=subitems.end(); it++)
    {
	result = result + (*it)->toxmlstring();
    }
    result = result + sindent + "</" + name + /* " full="+  getfullname() +*/">\n";
    indent--;
    return result;
}
