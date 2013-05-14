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

#ifndef RESULTPARSE_H
#define RESULTPARSE_H

#include "resultdom.h"

Item* xmlparse(const char* xml, int len); //xml строка с xml len ее размер в байтах

char* stripinvalidtag(char* xml, int len);	//ГРЯЗНЫЙ ХАК нужен чтобы до парсинга удалить кривые теги
						//в сообщениях вида <a href=.. </a> иначе будет ошибка парсинга

#endif // RESULTPARSE_H