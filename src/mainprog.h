#ifndef MAINPROG_H
#define MAINPROG_H

#include "nprogram.h"
#include "msgwin.h"
#include "taskwin.h"
#include "mainwin.h"
#include "cfgform.h"
#include "cfg.h"
#include "srvdata.h"
#include "topmenu.h"
#include "about.h"
#include "helpwin.h"
#include "addprojectform.h"
#include "addmgrform.h"


class MainProg : public NProgram
{
  public:
    MainProg();
    void smartresize();
    bool mainloop();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    TopMenu*		menu;
    MainWin*		wmain;
    NStaticText*	wstatus;
    AboutWin*		about;
    HelpWin*		help;
    AddProjectForm*	addform; //форма добавления проекта
    AddAccMgrForm*	addmgrform; //форма аккаунт менеджера
    Config* cfg;
    CfgForm* cfgform; //окно конфигурации
    SrvList* gsrvlist; //глобальный список серверов
    bool	done; //флаг требования завершения
    void setcaption();
};


#endif //MAINPROG_H

