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

#ifndef MBSTRING_H
#define MBSTRING_H

int mbstrlen(const char* s); //вернет размер строки utf8 в СИМВОЛАХ (не байтах) без учета '\0'

char* mbstrtrunc(char* s, int slen); //обрезать до длины slen символов (не байтов) без учета \0

char* rtrim(char* s); //удалить завершающие пробелы в строке

char* ltrim(char* s); //удалить начальные пробелы в строке
#endif // MBSTRING_H
