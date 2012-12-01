#include <signal.h>
#include "nprogram.h"
#include "kclog.h"


bool NProgram::needresize;


NProgram::NProgram() : NGroup(NRect(getmaxy(stdscr), getmaxx(stdscr), 0, 0))
{
    NProgram::needresize = false;
    signal(SIGWINCH, NProgram::sig_winch); //обработчик сигнала ресайза терминала
}

void NProgram::sig_winch(int signo) //вызывается при изменении размеров терминала
{
    NProgram::needresize = true;
}

void NProgram::putevent(NEvent* ev) //отправить событие по цепочке владельцев в очередь
{
    evqueue.push(ev); //поместить в очередь
    kLogPrintf("NProgram::putevent(NEvent* ev)\n");
}
