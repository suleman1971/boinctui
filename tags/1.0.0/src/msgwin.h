#ifndef MSGWIN_H
#define MSGWIN_H

#include "nscrollview.h"
#include "resultdom.h"
#include "srvdata.h"


class MsgWin : public NScrollView
{
  public:
    MsgWin(NRect rect/*, void* hconnect*/):NScrollView(rect) { /*this->hconnect = hconnect; lastmsgno = 0;*/  };
    virtual ~MsgWin() { /*if (domtree!=NULL) delete domtree;*/ }
    void	updatedata(); 	//обновить данные с сервера
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void* 	setserver(Srv* srv) { if (this->srv != srv) { this->srv = srv; lastmsgno = 0; } };
  protected:
    Srv*	srv;		//текущий отображаемый сервер
    int 	lastmsgno; 	//номер последнего полученного сообщения
//    Item*	domtree;	//структура данных ответ на get_messages
};


#endif //MSGWIN_H
