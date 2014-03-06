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

#ifndef NPROGRAM_H
#define NPROGRAM_H

#ifdef EVENTTHREAD
#include <pthread.h>
#endif
#include <queue>
#include "ngroup.h"


#define EVTIMERINTERVAL 2 //число секунд через которые генерируется evTIMER


class NProgram : public NGroup
{
  public:
    static bool	needresize; 	//true если произошло изменение размеров терминала
    NProgram();
    #ifdef EVENTTHREAD
    ~NProgram() { stopflag=true; pthread_join(thread, NULL); pthread_mutex_destroy(&mutex); };
    void lock() { pthread_mutex_lock(&mutex); };
    void unlock() { pthread_mutex_unlock(&mutex); };
    bool evqueueempty() { bool result; lock(); result=evqueue.empty(); unlock(); return result; };
    NEvent* popevent() { NEvent* result; lock(); result=evqueue.front(); evqueue.pop(); unlock(); return result; };
    #else
    bool evqueueempty() { return evqueue.empty(); };
    NEvent* popevent() { NEvent* result=evqueue.front(); evqueue.pop(); return result; };
    #endif
    virtual void putevent(NEvent* ev); //отправить событие по цепочке владельцев в очередь
    static void sig_winch(int signo); //вызывается при изменении размеров терминала
  private:
    std::queue<NEvent*> evqueue; //очередь событий
    #ifdef EVENTTHREAD
    pthread_mutex_t	mutex;
    pthread_t		thread;
    bool		stopflag;
    static void*	evcreationthread(void* args); //трейд опрашивающий клавиатуру и мышь
    #endif
};

#endif //NPROGRAM_H