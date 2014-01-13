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

#ifndef MAINWIN_H
#define MAINWIN_H

#include "ngroup.h"
#include "taskwin.h"
#include "msgwin.h"
#include "nstatictext.h"
#include "nhline.h"
#include "nvline.h"
#include "infopanel.h"
#include "srvdata.h"


class MainWin : public NGroup
{
  public:
    MainWin(NRect rect/*, Config* cfg*/);
    virtual void refresh();
    virtual void resize(int rows, int cols);
    void 	setserver(Srv* srv); //установить отображаемый сервер
//    virtual void move(int begrow, int begcol);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    TaskWin* 	wtask;		//окно задач
    MsgWin* 	wmsg;		//окно эвентов
    NStaticText* tablheader; 	//заголовок таблицы процессов
    InfoPanel*	panel1; 	//инф-ная панель
    NColorString* caption; 	//строка заголовка
    void updatecaption();
  protected:
    Srv*	srv;
    NHLine*	hline; 		//гориз линия завершающая таблицу процессов
    NVLine*	vline; 		//верт линия отделяющая инф панель
    std::vector<std::string>	colname; 	//список названий колонок
    void	setcoltitle();
};

#endif //MAINWIN_H