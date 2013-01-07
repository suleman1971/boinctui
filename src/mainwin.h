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
    MainWin(NRect rect, Config* cfg);
    virtual void refresh();
    virtual void resize(int rows, int cols);
    void 	setserver(Srv* srv); //установить отображаемый сервер
//    virtual void move(int begrow, int begcol);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    TaskWin* 	wtask;		//окно задач
    MsgWin* 	wmsg;		//окно эвентов
    NStaticText* tablheader; 	//заголовок таблицы процессов
    InfoPanel*	panel1; 	//инф-ная панель
    NColorString* caption; 	//строка заголовка
  protected:
    NHLine*	hline; 		//гориз линия завершающая таблицу процессов
    NVLine*	vline; 		//верт линия отделяющая инф панель
    std::vector<std::string>	colname; 	//список названий колонок
    void	setcoltitle();
};

#endif //MAINWIN_H