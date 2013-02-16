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

#ifndef NGROUP_H
#define NGROUP_H


#include <list>
#include "nview.h"


class NGroup : public NView
{
  public:
    NGroup(NRect rect) : NView(rect) {};
    virtual ~NGroup();
    void insert(NView* view) { view->setowner(this); items.push_back(view); };
    void remove(NView* view) { items.remove(view); };
    virtual void refresh();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    virtual void setneedrefresh();
    virtual void move(int begrow, int begcol);
  protected:
    std::list<NView*> items; //список вложенных элементов
};

#endif // NGROUP_H
