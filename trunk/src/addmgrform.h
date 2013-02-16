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

#ifndef ADDMGRFORM_H
#define ADDMGRFORM_H

#include <string.h>
#include <stdlib.h>
#include "nform.h"
#include "nstatictext.h"
#include "srvdata.h"


class AddAccMgrForm : public NForm
{
  public:
    AddAccMgrForm(int lines, int rows, Srv* srv, const char* mgrname);
    void genfields(int& line, Item* project); //создаст массив полей
    virtual void eventhandle(NEvent* ev);	//обработчик событий
  protected:
    int		passwfield;
    int		errmsgfield;
    int		usernamefield;
    Srv*	srv;
    std::string mgrname;	//имя акк менеджера
    std::string mgrurl;		//url подключаемого менеджера
};


#endif //ADDMGRFORM_H