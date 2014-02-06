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

#ifndef MSGWIN_H
#define MSGWIN_H

#include "nscrollview.h"
#include "resultdom.h"
#include "srvdata.h"


class MsgWin : public NScrollView
{
  public:
    MsgWin(NRect rect):NScrollView(rect) { };
    //virtual ~MsgWin() { };
    void	updatedata(); 	//обновить данные с сервера
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void* 	setserver(Srv* srv) { if (this->srv != srv) { this->srv = srv; clearcontent(); lastmsgno = 0; } };
  protected:
    Srv*	srv;		//текущий отображаемый сервер
    int 	lastmsgno; 	//номер последнего полученного сообщения
};


#endif //MSGWIN_H
