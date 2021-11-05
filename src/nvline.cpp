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

#include "nvline.h"


void NVLine::refresh()
{
    wbkgd(win,bgcolor);
    wattrset(win,getcolorpair(COLOR_WHITE,getbgcolor()));
    if (asciilinedraw == 1)
	wvline(win, '|', getheight()-0);
    else
	wvline(win, ACS_VLINE, getheight()-0);
    NView::refresh();
};