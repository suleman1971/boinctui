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

#ifndef NET_H
#define NET_H


#include <string.h>
#include <stdlib.h>


class TConnect //информация о соединении с boinc сервером
{
  public:
    TConnect(const char* shost, const char* sport) { this->shost = strdup(shost); this->sport = strdup(sport); hsock = -1; };
    virtual ~TConnect() { disconnect(); free(shost); free(sport); };
    int   getsock() {return hsock;};
    void  sendreq(const char* fmt, va_list vl); //отправить запрос на сервер
    void  sendreq(const char* fmt, ...); //отправить запрос на сервер
    char* waitresult(); //получить ответ на запрос
    char* gethost() { return shost; };
    char* getport() { return sport; };
    bool  isconnected() { return (hsock != -1); };
  protected:
    virtual void  createconnect (/*const char* shost, const char* sport*/);
    virtual void  disconnect();
    int   hsock; //дескриптор сокета
    char* shost;
    char* sport;
};


#endif // NET_H
