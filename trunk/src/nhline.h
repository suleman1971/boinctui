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

#ifndef NHLINE_H
#define NHLINE_H


#include "nview.h"


class NHLine : public NView
{
  public:
    NHLine(NRect rect, const char* s) : NView(rect) { bgcolor = 0; };
    virtual ~NHLine() { };
    void setbgcolor(int colorpair) { bgcolor = colorpair; };
    virtual void refresh();
  protected:
    int		bgcolor; //цвет фона
    const char*	mbsym; //символ
};

#endif //NHLINE_H