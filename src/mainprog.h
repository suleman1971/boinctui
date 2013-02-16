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

#ifndef MAINPROG_H
#define MAINPROG_H

#include "nprogram.h"
#include "msgwin.h"
#include "taskwin.h"
#include "mainwin.h"
#include "cfgform.h"
#include "cfg.h"
#include "srvdata.h"
#include "topmenu.h"
#include "about.h"
#include "helpwin.h"
#include "addprojectform.h"
#include "addmgrform.h"


class MainProg : public NProgram
{
  public:
    MainProg();
    ~MainProg();
    void smartresize();
    bool mainloop();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    TopMenu*		menu;
    MainWin*		wmain;
    NStaticText*	wstatus;
    AboutWin*		about;
    HelpWin*		help;
    AddProjectForm*	addform; //форма добавления проекта
    AddAccMgrForm*	addmgrform; //форма аккаунт менеджера
    Config* cfg;
    CfgForm* cfgform; //окно конфигурации
    SrvList* gsrvlist; //глобальный список серверов
    bool	done; //флаг требования завершения
    void setcaption();
    time_t updatetime; //время последней отрисовки
};


#endif //MAINPROG_H

