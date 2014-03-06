// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

#include <signal.h>
#include <unistd.h>
#include "nprogram.h"
#include "kclog.h"


bool NProgram::needresize;


NProgram::NProgram() : NGroup(NRect(getmaxy(stdscr), getmaxx(stdscr), 0, 0))
{
    NProgram::needresize = false;
    signal(SIGWINCH, NProgram::sig_winch); //обработчик сигнала ресайза терминала
    #ifdef EVENTTHREAD
    pthread_mutex_init(&mutex,NULL);
    stopflag = false;
    if ( 0 != pthread_create(&thread, NULL, evcreationthread, this))
	kLogPrintf("NProgram::NProgram() pthread_create() error\n");
    #endif
}


void NProgram::sig_winch(int signo) //вызывается при изменении размеров терминала
{
    NProgram::needresize = true;
}


void NProgram::putevent(NEvent* ev) //отправить событие по цепочке владельцев в очередь
{
    #ifdef EVENTTHREAD
    lock();
    #endif
    evqueue.push(ev); //поместить в очередь
    #ifdef EVENTTHREAD
    unlock();
    #endif
    #ifdef DEBUG
    kLogPrintf("NProgram::putevent(%s)\n",ev->tostring().c_str());
    #endif
}


#ifdef EVENTTHREAD
void* NProgram::evcreationthread(void* args) //трейд опрашивающий клавиатуру и мышь
{
    NProgram* me = (NProgram*)args;
    kLogPrintf("NProgram::evcreationthread started\n");
    time_t evtimertime; //time of last evTIMER
    time(&evtimertime);
    evtimertime=evtimertime-2; //костыль для уменьшения задержки первой отрисовки
    while(!me->stopflag)
    {
	//если настало время посылаем evTIMER
	if (time(NULL) - evtimertime > EVTIMERINTERVAL)
	{
	    NEvent* event = new NEvent(NEvent::evTIMER, 0); //создаем событие таймера
	    me->putevent(event); //отправить в очередь
	    time(&evtimertime);
	}
	//есть символ в буфере -> нужно создать событие
	int ic;
	if ( (ic = getch()) != ERR ) //символ(ы) есть?
	{
	    NEvent* event = NULL;
	    if (KEY_MOUSE == ic)
	    {
		// mouse event
		MEVENT mevent;
		if (OK == getmouse(&mevent))
		    event = new NMouseEvent(mevent.bstate, mevent.y, mevent.x); //создаем мышиное событие
		else
		    kLogPrintf("getmouse() err\n");
	    }
	    else // keyboard event
		event = new NEvent(NEvent::evKB, ic); //создаем клавиатурное событие
	    if (event != NULL)
		me->putevent(event); //отправить в очередь
	}
    }
    kLogPrintf("NProgram::evcreationthread stopped\n");
    pthread_exit(0);
}
#endif
