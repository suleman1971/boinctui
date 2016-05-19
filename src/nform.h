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

#ifndef NFORM_H
#define NFORM_H

#include <string.h>
#include <stdlib.h>
#ifdef HAVE_LIBNCURSESW
    #include <ncursesw/form.h>
#else
    #include <form.h>
#endif
#include "ngroup.h"


class NForm : public NGroup
{
  public:
    NForm(int lines, int rows);
    virtual ~NForm();
    virtual void refresh();
    virtual void settitle(const char* title);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    virtual FIELD* addfield(FIELD* field);
    int 	getfieldcount() { return fieldcount; };
    virtual void delfields(); //удаляет все поля
  protected:
    bool clickatfield(int mrow, int mcol, FIELD* f); //true если клик внутри этого поля
    char* title;	//заголовок
    FIELD**	fields;
    int		fieldcount; //число полей включая NULL
    FORM* frm; 		//форма ncurses
};


#endif //NFORM_H