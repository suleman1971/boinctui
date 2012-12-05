#ifndef TOPMENU_H
#define TOPMENU_H

#include "nmenu.h"
#include "srvdata.h"


class TopMenu : public NMenu //верхний уровень меню
{
  public:
    TopMenu();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void enable()   { if (!enableflag) {enableflag = true;  set_menu_fore(menu, selectorattr); action(); } };
    void disable()  { if (enableflag) {enableflag = false; selectorattr = menu_fore(menu); set_menu_fore(menu, menu_back(menu)); destroysubmenu(); }; /*цвет указателя = цвет фона*/ };
    bool isenable() { return enableflag; };
    void setserver(Srv* srv) { this->srv = srv; }; //установить отображаемый сервер
  protected:
    virtual bool action(); //открыть субменю
    int		selectorattr; //цвет указателя
    bool	enableflag; //true если меню активно
    Srv*	srv; //текущий отображаемый сервер
};


class ProjectsSubMenu : public NMenu //выпадающие меню "Projects"
{
  public:
    ProjectsSubMenu(NRect rect, Srv* srv);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    Srv*	srv; //текущий отображаемый сервер
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
    ProjectListSubMenu(NRect rect, Srv* srv, char op);
    ~ProjectListSubMenu();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    virtual bool action(); //вызывается при нажатии Enter
    std::list<char*> projectnames;
    std::list<char*> projectstatus;
    Srv*	srv; //текущий отображаемый сервер
    char	op;  //выполняемая операция 'S','R',e.t.c.
};


class HelpSubMenu : public NMenu //выпадающие меню "Help"
{
  public:
    HelpSubMenu(NRect rect);
  protected:
    virtual bool action(); //вызывается при нажатии Enter
};


#endif //TOPMENU_H