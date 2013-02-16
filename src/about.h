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

#ifndef ABOUT_H
#define ABOUT_H

#include "ngroup.h"


class AboutWin : public NGroup
{
  public:
    AboutWin(int rows, int cols);
    ~AboutWin() { delete caption; };
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    char* caption; //строка заголовка
};

#endif //ABOUT_H