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

#ifndef TASKINFOWIN_H
#define TASKINFOWIN_H


#include <list>
#include <string>
#include "ngroup.h"
#include "nscrollview.h"
#include "tuievent.h"
#include "nscrollbar.h"


class TaskInfoWin : public NGroup  //стандартный диалог вида Ok/Cancel или Yes/No
{
  public:
    TaskInfoWin(const char* caption, Srv* srv, const char* projecturl, const char* taskname);
    void eventhandle(NEvent* ev);	//обработчик событий
    void updatedata();
  private:
    int maxlen1;
    int maxlen2;
    std::vector<std::pair<std::string, std::string> > ssbak; //прошлое значение
    std::string		caption;
    std::string		projecturl;
    std::string		taskname;
    Srv* srv;
    NScrollView*	content;
    NScrollBar*		scrollbar;
};


#endif //TASKINFOWIN_H