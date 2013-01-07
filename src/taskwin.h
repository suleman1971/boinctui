#ifndef TASKWIN_H
#define TASKWIN_H

#include "nselectlist.h"
#include "resultdom.h"
#include "srvdata.h"


class TaskWin : public NSelectList
{
  public:
    TaskWin(NRect rect, Config* cfg);
    virtual ~TaskWin();
    void	updatedata(); 	//обновить данные с серверa
    void	clearcontent();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    void*	setserver(Srv* srv) { this->srv = srv; startindex = 0; clearcontent(); selectedindex = -1; };
    bool iscolvisible(int n) { return ((1 << n)&columnmask); }; //true если n-ная колонка видимая
    void coldisable(int n) { columnmask = columnmask & (~(1 << n)); savemasktoconfig();}; //установить флаг невидимости n-ной колонки
    void colenable(int n) { columnmask = columnmask | (1 << n); savemasktoconfig();}; //установить флаг видимости n-ной колонки
    void savemasktoconfig(); //сохранить маску в дереве конфига
    //unsigned int getcolumnmask() { return columnmask; };
  protected:
    Srv*	srv; //текущий отображаемый сервер
    void	optask(char op);
    unsigned int columnmask; //маска видмсости колонок
    Config*	cfg;
};


#endif //TASKWIN_H
