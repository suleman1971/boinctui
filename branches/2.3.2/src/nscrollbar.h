// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013,2014 Sergey Suslov
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

#ifndef NSCROLLBAR_H
#define NSCROLLBAR_H


#include "nview.h"


class NScrollBar : public NView
{
  public:
    NScrollBar(NRect rect, chtype chtop = 0, chtype chbottom = 0, chtype chinactive = ACS_VLINE);
    virtual	~NScrollBar() { };
    void	setbgcolor(int colorpair) { bgcolor = colorpair; };
    virtual void refresh();
    void	setpos(int vmin, int vmax, int vpos1, int vpos2);
    void	setvisible(bool value) { visible = value; };
  protected:
    int		bgcolor; //цвет фона
    chtype	chtop;	//символ вверху
    chtype	chbottom;//символ внизу
    chtype	chinactive; //символ которым заполняется когда неактивный
    int		vmin;	//минимальное значение
    int		vmax;	//максимальное значение
    int		vpos1;	//минимальное видимое значение
    int		vpos2;	//максимальное видимое значение
    bool	visible;//true если скроллер видимый
};

#endif //NSCROLLBAR_H