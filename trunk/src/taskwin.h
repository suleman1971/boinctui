#ifndef TASKWIN_H
#define TASKWIN_H

#include "nselectlist.h"
#include "resultdom.h"
#include "srvdata.h"


class TaskWin : public NSelectList
{
  public:
    TaskWin(NRect rect):NSelectList(rect) { setselectorbgcolor(COLOR_CYAN); };
    virtual ~TaskWin();
    void	updatedata(); 	//обновить данные с серверa
    void	clearcontent();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void* 	setserver(Srv* srv) { this->srv = srv; };
  protected:
    Srv*	srv; //текущий отображаемый сервер
    void	suspendresumetask(char op);
};


#endif //TASKWIN_H
