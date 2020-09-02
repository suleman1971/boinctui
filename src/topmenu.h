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

#ifndef TOPMENU_H
#define TOPMENU_H

#include "nmenu.h"
#include "srvdata.h"


class TopMenu : public NMenu //верхний уровень меню
{
  public:
    TopMenu(/*Config* cfg*/);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void enable()   { if (!enableflag) {enableflag = true;  set_menu_fore(menu, selectorattr); /*action();*/ } };
    void disable()  { if (enableflag) {enableflag = false; selectorattr = menu_fore(menu); set_menu_fore(menu, menu_back(menu)); destroysubmenu(); }; /*цвет указателя = цвет фона*/ };
    bool isenable() { return enableflag; };
    void setserver(Srv* srv) { this->srv = srv; }; //установить отображаемый сервер
    virtual bool action(); //открыть субменю
  protected:
    int		selectorattr; //цвет указателя
    bool	enableflag; //true если меню активно
    Srv*	srv; //текущий отображаемый сервер
//    Config*	cfg;
};


class ViewSubMenu : public NMenu //выпадающие меню "View"
{
  public:
    ViewSubMenu(NRect rect/*, Config* cfg*/);
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    bool iscolenable(/*Config* cfg,*/ int n);
};


class ProjectsSubMenu : public NMenu //выпадающие меню "Projects"
{
  public:
    ProjectsSubMenu(NRect rect, Srv* srv);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
    std::string	accmgrurl; //url акк менеджера если есть
};


class TasksSubMenu : public NMenu //выпадающие меню "Tasks"
{
  public:
    TasksSubMenu(NRect rect);
  protected:
    virtual bool action(); //вызывается при нажатии Enter
};


class FileSubMenu : public NMenu //выпадающие меню "File"
{
  public:
    FileSubMenu(NRect rect);
  protected:
    virtual bool action(); //вызывается при нажатии Enter
};


class ProjectListSubMenu : public NMenu //выпадающие меню второго уровня Список проектов
{
  public:
    ProjectListSubMenu(NRect rect, Srv* srv, std::string projname);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
    std::string	projname;  //имя проекта
};


class ProjectAllListSubMenu : public NMenu //выпадающие меню второго уровня Список ВСЕХ проектов
{
  public:
    ProjectAllListSubMenu(NRect rect, Srv* srv);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
};


class ProjectAccMgrSubMenu : public NMenu //выпадающие меню второго уровня Список Аккаунт менеджеров
{
  public:
    ProjectAccMgrSubMenu(NRect rect, Srv* srv);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
};


class ProjectUserExistSubMenu : public NMenu //выпадающие меню третего New/Exist user
{
  public:
    ProjectUserExistSubMenu(NRect rect, Srv* srv, const char* prjname, bool byurl);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
    std::string	prjname; //имя выбранного проекта
    bool byurl = false; //true если добавляем по url
};


class ActivitySubMenu : public NMenu //выпадающие меню второго уровня Activity
{
  public:
    ActivitySubMenu(NRect rect, Srv* srv);
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
};

class HelpSubMenu : public NMenu //выпадающие меню "Help"
{
  public:
    HelpSubMenu(NRect rect);
  protected:
    virtual bool action(); //вызывается при нажатии Enter
};


#endif //TOPMENU_H