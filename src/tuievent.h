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
    evADDACCMGR		//форма акк менеджера
};


class TuiEvent : public NEvent //класс программных событий специфичных для boinctui
{
  public:
    TuiEvent(TuiEventType type) : NEvent(evPROG, type)	{};
    TuiEvent(Srv* srv, const char* prjname, bool userexist) : NEvent(evPROG, evADDPROJECT) //событие для добавления проекта
    {
	this->srv = srv;
	this->sdata1 = prjname;
	this->bdata1 = userexist;
    };
    TuiEvent(Srv* srv, const char* mgrname) : NEvent(evPROG, evADDACCMGR) //событие для подключения к менеджеру
    {
	this->srv = srv;
	this->sdata1 = mgrname;
    };
    Srv*		srv;
    std::string		sdata1; //произвольная строка
    bool		bdata1; //произаольная bool переменная
};


#endif //TUIEVENT_H