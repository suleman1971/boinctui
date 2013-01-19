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
    void coldisable(int n) { columnmask = columnmask & (~(1 << n)); saveopttoconfig();}; //установить флаг невидимости n-ной колонки
    void colenable(int n) { columnmask = columnmask | (1 << n); saveopttoconfig();}; //установить флаг видимости n-ной колонки
    void saveopttoconfig(); //сохранить маску и т.д. в дереве конфига
    //unsigned int getcolumnmask() { return columnmask; };
  protected:
    Srv*	srv; //текущий отображаемый сервер
    void	optask(char op);
    unsigned int columnmask; //маска видмсости колонок
    int		taskslistmode; //0 все задачи 1 кроме завершенных 2 только активные
    int		taskssortmode; //0 нет сортировки 1 state 2 done 3 project 4 est 5 d/l 6 app 7 task
    Config*	cfg;
};


#endif //TASKWIN_H
