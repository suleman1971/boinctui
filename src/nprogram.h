#ifndef NPROGRAM_H
#define NPROGRAM_H

#include <queue>
#include "ngroup.h"


class NProgram : public NGroup
{
  public:
    static bool	needresize; 	//true если произошло изменение размеров терминала
    NProgram();
    virtual void putevent(NEvent* ev); //отправить событие по цепочке владельцев в очередь
    static void sig_winch(int signo); //вызывается при изменении размеров терминала
  protected:
    std::queue<NEvent*> evqueue; //очередь событий
};

#endif //NPROGRAM_H