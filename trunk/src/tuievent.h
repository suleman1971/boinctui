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
    evADDPROJECT	//добавить проект
};


class TuiEvent : public NEvent //класс программных событий специфичных для boinctui
{
  public:
    TuiEvent(TuiEventType type) : NEvent(evPROG, type)	{};
    TuiEvent(Srv* srv, const char* prjname, bool userexist) : NEvent(evPROG, evADDPROJECT) //событие для добавления проекта
    {
	this->srv = srv;
	this->prjname = prjname;
	this->userexist = userexist;
    };
    Srv*		srv;
    std::string		prjname;
    bool		userexist;
};


#endif //TUIEVENT_H