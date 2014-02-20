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

#ifndef NSTATICTEXT_H
#define NSTATICTEXT_H


#include "nview.h"
#include "ncolorstring.h"


class NStaticText : public NView
{
  public:
    NStaticText(NRect rect) : NView(rect) { content = new NColorString(0,""); align = 0; setbgcolor(getcolorpair(COLOR_WHITE,COLOR_BLACK));};
    virtual ~NStaticText() { delete content; };
    void appendstring(int attr, const char* fmt, ...);
    void setstring(int attr, const char* fmt, ...);
    void setbgcolor(int colorpair) { bgcolor = colorpair; };
    void setalign(int align) { this->align = align; }; //0-левое 2-правое 1-центр
    virtual void refresh();
  protected:
    NColorString* 	content;
    int			align;   //вравнивание
    int			bgcolor; //цвет фона
};

#endif //NSTATICTEXT_H