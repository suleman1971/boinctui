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

#ifndef STATWIN_H
#define STATWIN_H


#include <list>
#include <string>
#include "ngroup.h"
#include "nscrollview.h"
#include "tuievent.h"
#include "nscrollbar.h"


struct DayStat //статистика за один день проекта
{
    time_t		day;		//день
    unsigned long int	scorehost;
    unsigned long int	scoreuser;
};


struct ProjStat
{
    std::string		name;	//имя проекта
    std::list<DayStat>  days;	//дни
};


class StatWin : public NGroup //окно общей стстистики
{
  public:
    StatWin(const char* caption, Srv* srv);
    void eventhandle(NEvent* ev);	//обработчик событий
    void updatedata();
  private:
    int maxlen1;
    int maxlen2;
    std::vector<std::pair<std::string, std::string> > ssbak; //прошлое значение
    std::string		caption;
    Srv* srv;
    NScrollView*	content;
    NScrollBar*		scrollbar;
    std::vector<ProjStat> projects; //матрица статистики
};


#endif //STATWIN_H