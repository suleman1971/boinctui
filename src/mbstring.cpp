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

#include <stdlib.h>
#include <string.h>
#include "mbstring.h"


int mbstrlen(const char* s) //вернет размер строки utf8 в СИМВОЛАХ (не байтах) без учета '\0'
{
    int bsize = strlen(s); //количество байт
    int result = 0; //подсчитанное кол-во символов
    int nbytes = 0; //просмотренное кол-во байтов
    const char* p = s;
    do
    {
	int symlen = mblen(p,bsize-nbytes);
	if (symlen <= 0) 
	    break;
	nbytes = nbytes + symlen;
	result++;
	p = p + symlen; //адрес начала след символа
    }
    while ( (*p != 0)&&(nbytes < bsize) );
    return result;
}


char* mbstrtrunc(char* s, int slen) //обрезать до длины slen символов (не байтов) без учета \0
{
    if (mbstrlen(s)<=slen) //обрезать не надо и так короткая
	return s;
    int bsize = strlen(s); //количество байт
    int nbytes = 0; //просмотренное кол-во байтов
    int ns = 0; //просмотренное кол-во символов
    char* p = s;
    do
    {
	int symlen = mblen(p,bsize-nbytes);
	nbytes = nbytes + symlen;
	ns++;
	p = p + symlen;
    }
    while ( (*p != '\0')&&(nbytes < bsize)&&(ns < slen) );
    *p = '\0'; //обрезаем
    return s;
}


char* rtrim(char* s) //удалить завершающие пробелы в строке
{
    if (s == NULL)
	return NULL;
    if (strlen(s) == 0)
	return s;
    char* p = s + strlen(s) - 1;
    while ( p >= s)
    {
        if (*p == ' ')
	    *p = 0;
	else
	    break;
	p--;
    }
    return s;
}
