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

#ifndef NEVENT_H
#define NEVENT_H


class NEvent //класс описывающий событие создаваемое например при нажатии клавиш
{
  public:
    enum 	Type 		{ evKB, evMOUSE, evPROG }; //evPROG событие генерируемое самой прораммой
    NEvent(NEvent::Type type, int keycode)	{ this->type = type; this->done = false; this->keycode = keycode;};
    NEvent::Type	type;
    bool		done;		//true если обработано
    union
    {
	int		keycode;	//код клавиатуры
	int		cmdcode;	//произвольный код команды
    };
};


#endif //NEVENT_H