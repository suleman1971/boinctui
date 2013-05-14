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

#ifndef NCOLORSTRING_H
#define NCOLORSTRING_H

#include <stdarg.h>
#include <string>
#include <list>
#include "mbstring.h"


class NColorStringPart
{
  public:
    NColorStringPart(int attr, const char* s) { this->s = s; this->attr = attr;};
    NColorStringPart(int attr, const char* fmt, va_list vl);
    int getlen() { return mbstrlen(s.c_str()); }; //вернет длинну в ЭКРАННЫХ СИМВОЛАХ
    std::string s;
    int	attr;
};


class NColorString
{
  public:
    NColorString(int attr, const char* fmt, ...);
    NColorString(int attr, const char* fmt, va_list vl);
    ~NColorString();
    void append(int attr, const char* fmt, ...);
    void vappend(int attr, const char* fmt, va_list vl);
    std::list<NColorStringPart*> parts;
    int getlen(); //вернет длинну в ЭКРАННЫХ СИМВОЛАХ
    void clear() { while(!parts.empty()) { delete parts.front(); parts.remove(parts.front());} }; //очищаем строку
  protected:
    void append(NColorStringPart* part) { parts.push_back(part); }; //добавить подстроку к строке
};

#endif //NCOLORSTRING_H