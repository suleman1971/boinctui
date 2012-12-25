#ifndef MSGWIN_H
#define MSGWIN_H

#include "nscrollview.h"
#include "resultdom.h"
#include "srvdata.h"


class MsgWin : public NScrollView
{
  public:
    MsgWin(NRect rect):NScrollView(rect) { };
    //virtual ~MsgWin() { };
    void	updatedata(); 	//обновить данные с сервера
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void* 	setserver(Srv* srv) { if (this->srv != srv) { this->srv = srv; clearcontent(); lastmsgno = 0; } };
  protected:
    Srv*	srv;		//текущий отображаемый сервер
    int 	lastmsgno; 	//номер последнего полученного сообщения
};


#endif //MSGWIN_H
