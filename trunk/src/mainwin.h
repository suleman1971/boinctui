#ifndef MAINWIN_H
#define MAINWIN_H

#include "ngroup.h"
#include "taskwin.h"
#include "msgwin.h"
#include "nstatictext.h"
#include "nhline.h"
#include "nvline.h"
#include "infopanel.h"
#include "srvdata.h"


class MainWin : public NGroup
{
  public:
    MainWin(NRect rect/*, void* hconnect*/);
    virtual void refresh();
    virtual void resize(int rows, int cols);
    void 	setserver(Srv* srv); //установить отображаемый сервер
//    virtual void move(int begrow, int begcol);
    TaskWin* 	wtask;		//окно задач
    MsgWin* 	wmsg;		//окно эвентов
    NStaticText* tablheader; 	//заголовок таблицы процессов
    NHLine*	hline; //гориз линия завершающая таблицу процессов
    InfoPanel*	panel1; //инф-ная панель
    NVLine*	vline; //верт линия отделяющая инф панель
};

#endif //MAINWIN_H