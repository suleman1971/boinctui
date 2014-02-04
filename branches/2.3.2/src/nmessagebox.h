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

#ifndef NMESSAGEBOX_H
#define NMESSAGEBOX_H


#include <list>
#include "ngroup.h"
#include "nstatictext.h"
#include "tuievent.h"


class NMButton : public NStaticText //кнопка внутри NMessageBox
{
  public:
    NMButton(const char* text, NEvent* pevent, ...);
    ~NMButton() { if (pevent) delete pevent; };
    void eventhandle(NEvent* ev);	//обработчик событий
  private:
    NEvent* pevent; //евент генерируемый кнопкой при нажатии
    std::list<char>	keys; //на какие коды она реагирует
};


class NMessageBox : public NGroup  //стандартный диалог вида Ok/Cancel или Yes/No
{
  public:
    NMessageBox(const char* text);
    void eventhandle(NEvent* ev);	//обработчик событий
    void addbutton(NMButton* button); //добавить кнопку
  private:
    NStaticText* content;
};

#endif //NMESSAGEBOX_H