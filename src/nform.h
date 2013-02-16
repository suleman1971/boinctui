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

#ifndef NFORM_H
#define NFORM_H

#include <string.h>
#include <stdlib.h>
#include <form.h>
#include "ngroup.h"


class NForm : public NGroup
{
  public:
    NForm(int lines, int rows);
    virtual ~NForm();
    //int eventloop();
    virtual void refresh();
    virtual void settitle(const char* title);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    virtual FIELD* addfield(FIELD* field);
    int 	getfieldcount() { return fieldcount; };
    virtual void delfields(); //удаляет все поля
  protected:
    char* title;	//заголовок
    FIELD**	fields;
    int		fieldcount; //число полей включая NULL
    FORM* frm; 		//форма ncurses
//    WINDOW* framewin; //исходное окно  (рамка)
};


#endif //NFORM_H