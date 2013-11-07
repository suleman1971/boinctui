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

#ifndef NSELECTLIST_H
#define NSELECTLIST_H


#include "nscrollview.h"


class NSelectList : public NScrollView //список со скроллингом и селектором
{
  public:
    NSelectList(NRect rect) : NScrollView(rect) { selectedindex = -1; setselectorenable(true); setselectorbgcolor(COLOR_WHITE); };
    void addstring(void* userobj, int attr, const char* fmt, ...); //userobj произвольные данные связанные со строкой
    void addstring(void* userobj, NColorString* sctring);
    virtual void drawcontent();
    virtual void refresh();
    virtual void selectorup();
    virtual void selectordown();
    virtual void setselectorpos(int n) { selectedindex = n; needrefresh = true; }; //передвинуть селектор на строку n
    void* getselectedobj(); //вернет указатель или NULL
    void setselectorbgcolor(short color) { selectorbgcolor = color; };
    //virtual bool objcmpeqv(void* obj1, void* obj2) { return obj1==obj2; };
    void setselectorenable(bool b) { selectorenable = b; }; //изменить видимость селектора
  protected:
    //void*	selectedobj; 		//выделенный объект
    int selectedindex; //номер выделенной строки
    bool selectorenable; //true если видимость селектора разрешена (не значит что он видим)
    std::vector<void*> 	objects; 	//объекты, ассоциированные с визуальными строками
    short	selectorbgcolor;	//номер цвета фона выделеной строки
};


#endif //NSELECTLIST_H