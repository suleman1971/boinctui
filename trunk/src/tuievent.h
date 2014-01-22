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

#ifndef TUIEVENT_H
#define TUIEVENT_H

#include "srvdata.h"
#include "nevent.h"


enum TuiEventType
{
    evCFGCH,		//конфигурация изменена
    evABORTRES,		//abort selected result
    evABOUT,		//открыть/закрыть откно About
    evKEYBIND,		//открыть/закрыть откно Key Bindings
    evBENCHMARK,	//run benchmark
    evADDPROJECT,	//форма добавить проект
    evCOLVIEWCH,	//переключение видимости колонки
    evVIEWMODECH,	//переключение режима видимости All/Hide done/Active tasks only
    evSORTMODECH,	//переключение режима сортировки списка задач 0-unsorted 1-state e.t.c.
    evADDACCMGR,	//форма акк менеджера
    evPROJECTOP,	//операции над проектом
    evTASKINFO,		//генерируется когда нужно открыть окно детальной информации о задаче
    evTASKSELECTORON,	//генерируется когда селектор задач становится видимым
    evTASKSELECTOROFF,	//генерируется когда селектор задач становится скрытым
    evASCIIMODECHANGE	//генерируется при переключении режима ASCII рамок
};


class TuiEvent : public NEvent //класс программных событий специфичных для boinctui
{
  public:
    TuiEvent(TuiEventType type) : NEvent(evPROG, type)	{};
    TuiEvent(TuiEventType type, Srv* srv, const char* prjname, bool userexist) : NEvent(evPROG, type) //событие для добавления проекта
    {
	this->srv = srv;
	this->sdata1 = prjname;
	this->bdata1 = userexist;
    };
    TuiEvent(TuiEventType type, Srv* srv, const char* mgrname) : NEvent(evPROG, type) //событие для подключения к менеджеру
    {
	this->srv = srv;
	this->sdata1 = mgrname;
    };
    TuiEvent(TuiEventType type, Srv* srv, const char* projname, const char* projop) : NEvent(evPROG, type) //событие для действий с проектами
    {
	this->srv = srv;
	this->sdata1 = projname;
	this->sdata2 = projop;
	this->bdata1 = false; //true - если получено подтверждение
    };

    TuiEvent(TuiEventType type ,int ncolumn, bool enable) : NEvent(evPROG, type) //событие переключения видимости колонки
    {
	this->idata1 = ncolumn;
	this->bdata1 = enable;
    };
    TuiEvent(TuiEventType type ,int mode) : NEvent(evPROG, type) //событие режим видимости задач
    {
	this->idata1 = mode;
    };
    virtual ~TuiEvent() { /*kLogPrintf("~TuiEvent()\n");*/ };
    Srv*		srv;
    std::string		sdata1; //произвольная строка
    std::string		sdata2; //произвольная строка
    bool		bdata1; //произаольная bool переменная
    int			idata1; //произвольное целое
};


#endif //TUIEVENT_H