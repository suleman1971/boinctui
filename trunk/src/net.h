#ifndef NET_H
#define NET_H


#include <string.h>
#include <stdlib.h>


class TConnect //информация о соединении с boinc сервером
{
  public:
    TConnect(const char* shost, const char* sport) { this->shost = strdup(shost); this->sport = strdup(sport); hsock = -1; /*createconnect(shost,sport);*/ };
    ~TConnect() { disconnect(); free(shost); free(sport); };
    int   getsock() {return hsock;};
    void  sendreq(const char* fmt, va_list vl); //отправить запрос на сервер
    void  sendreq(const char* fmt, ...); //отправить запрос на сервер
    char* waitresult(); //получить ответ на запрос
    char* gethost() { return shost; };
    char* getport() { return sport; };
  protected:
    void  createconnect (const char* shost, const char* sport);
    void  disconnect();
    int   hsock; //дескриптор сокета
    char* shost;
    char* sport;
};


#endif // NET_H
