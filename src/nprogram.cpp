// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

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
