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

#ifndef NMENU_H
#define NMENU_H

#include <menu.h>
#include <stdlib.h>
#include "ngroup.h"


class NMenu : public NGroup
{
  public:
    NMenu(NRect rect, bool horis = false);
    virtual ~NMenu();
    virtual void refresh();
    void	 additem(const char* name, const char* comment); //добавить эл-т в меню
    void 	 eventhandle(NEvent* ev); 	//обработчик событий
    void	 setbackground(int attr) { set_menu_back(menu, attr); wattrset(win, attr);  bgattr = attr; wbkgdset(win,attr); };
    void	 setforeground(int attr) { set_menu_fore(menu, attr); fgattr = attr; };
    void	 postmenu()   { if (!posted) {post_menu(menu); posted = true;} };
    void	 unpostmenu() { if (posted)  {unpost_menu(menu); posted = false;} };
    virtual bool createsubmenu() { return false; }; //открыть субменю
    virtual void destroysubmenu(); //закрыть субменю
    virtual bool action() { return false; }; //вызывается при нажатии Enter
  protected:
    MENU*	menu;
    ITEM** 	mitems; //масив элементов
    std::list<char*> itemnames; //имена эл-тов меню
    std::list<char*> itemcomments; //комментарии к эл-там меню
    int		bgattr; //цвет текста и фона невыделенного эл-та
    int		fgattr; //цвет текста и фона выделенного эл-та
    bool	ishoris;//true если меню горизонтальное
  private:
    bool	posted; //true после post_menu()
};

#endif //NMENU_H