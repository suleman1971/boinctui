#ifndef MAINPROG_H
#define MAINPROG_H

#include "nprogram.h"
#include "msgwin.h"
#include "taskwin.h"
#include "mainwin.h"
#include "cfgform.h"
#include "cfg.h"
#include "srvdata.h"


class MainProg : public NProgram
{
  public:
    MainProg();
    void smartresize();
    bool mainloop();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    NStaticText*	wtitle;
    MainWin*		wmain;
    NStaticText*	wstatus;
    Config* cfg;
    CfgForm* cfgform; //окно конфигурации
    SrvList* gsrvlist; //глобальный список серверов
    bool	done; //флаг требования завершения
};


#endif //MAINPROG_H

