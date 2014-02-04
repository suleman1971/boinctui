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

#ifndef NEVENT_H
#define NEVENT_H

#ifdef DEBUG
    #include <string>
    #include <sstream>
#endif

//#include "kclog.h"


class NEvent //класс описывающий событие создаваемое например при нажатии клавиш
{
  public:
    enum	Type		{ evKB, evMOUSE, evPROG, evTIMER }; //evPROG событие генерируемое самой прораммой
    NEvent(NEvent::Type type, int keycode)	{ this->type = type; this->done = false; this->keycode = keycode;};
    virtual ~NEvent() { /*kLogPrintf("~NEvent()\n");*/ };
    NEvent::Type	type;
    bool		done;		//true если обработано
    union
    {
	int		keycode;	//код клавиатуры
	int		cmdcode;	//произвольный код команды
    };
    #ifdef DEBUG
    virtual std::string tostring()
    {
	std::stringstream s;
	s << this << " " << type << "(";
	switch (type)
	{
	    case evKB:
		s << "evKB keycode=" << keycode;
		break;
	    case evMOUSE:
		s << "evMOUSE";
		break;
	    case evPROG:
		s << "evPROG";
		break;
	    case evTIMER:
		s << "evTIMER";
		break;
	};
	s << ")";
	return s.str();
    };
    #endif
};


class NMouseEvent : public NEvent
{
  public:
    NMouseEvent(int bstate, int row, int col) : NEvent(evMOUSE, bstate) //bstate (see mmask_t ncurses.h)
    {
	this->col = col;
	this->row = row;
    };
    virtual ~NMouseEvent() { /*kLogPrintf("~NMouseEvent()\n");*/ };
    int row;
    int col;
    #ifdef DEBUG
    virtual std::string tostring()
    {
	std::stringstream s;
	s << NEvent::tostring() << "{row=" << row << ",col=" << col << ",bstate=" << std::hex << cmdcode;
	s << "}";
	return s.str();
    };
    #endif

};

#endif //NEVENT_H