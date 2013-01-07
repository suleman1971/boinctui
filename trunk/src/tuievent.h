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
    evADDACCMGR		//форма акк менеджера
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
    TuiEvent(TuiEventType type ,int ncolumn, bool enable) : NEvent(evPROG, type) //событие переключения видимости колонки
    {
	this->idata1 = ncolumn;
	this->bdata1 = enable;
    };
    Srv*		srv;
    std::string		sdata1; //произвольная строка
    bool		bdata1; //произаольная bool переменная
    int			idata1; //произвольное целое
};


#endif //TUIEVENT_H