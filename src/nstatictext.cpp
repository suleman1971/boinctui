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

#include "nstatictext.h"
#include "kclog.h"


void NStaticText::appendstring(int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    content->vappend(attr, fmt, args);
    va_end(args);
    needrefresh = true;
    this->refresh();
}


void NStaticText::setstring(int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    delete content;
    content = new NColorString(attr,fmt,args);
    va_end(args);
    needrefresh = true;
    this->refresh();
}


void NStaticText::refresh()
{
    wbkgd(win,bgcolor);
    std::list<NColorStringPart*>::iterator it;
    werase(win);
    wmove(win,0,0);
    if (align == 1) //центрирование
    {
	wmove(win,0,(getwidth()/2)-(content->getlen()+1)/2);
    }
    if (align == 2) //правое
    {
	wmove(win,0,getwidth()-content->getlen()/*+1*/);
    }
    for (it = content->parts.begin(); it != content->parts.end(); it++) //цикл по частям тек строки
    {
	NColorStringPart* part = *it;
	//kLogPrintf("[%d] %s\n", part->attr, part->s.c_str());
	wattrset(win,part->attr);
	wprintw(win,"%s",part->s.c_str());
	//wattrset(win,0);
    }
    wbkgdset(win,bgcolor);
    wclrtoeol(win); //очищаем до конца строки
    NView::refresh();
}
