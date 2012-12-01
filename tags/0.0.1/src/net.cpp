#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <list>
#include <algorithm>
#include "net.h"
#include "kclog.h"

/*
class TReq //информация о запросе
{
  public:
    TReq(const char* rawreq) {this->rawreq = rawreq;};
  protected:
    const char* rawreq; //xml строка запроса
};
*/


void TConnect::createconnect(/*const char* shost, const char* sport*/)
{
    //this->shost = strdup(shost); 
    //this->sport = strdup(sport); 

    struct sockaddr_in boincaddr;
    unsigned short port = atoi(sport);
    int	hsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&boincaddr,0, sizeof(boincaddr));
    boincaddr.sin_family = AF_INET;
    boincaddr.sin_addr.s_addr = inet_addr(shost);
    boincaddr.sin_port = htons(port);
    //connection
    if (connect(hsock, (struct sockaddr *) &boincaddr, sizeof(boincaddr)) < 0)
    {
        kLogPrintf("connect %s:%s failed!\n",shost,sport);
	this->hsock = -1;
    }
    else
    {
	this->hsock = hsock;
    }
}


void TConnect::disconnect()
{
    if (hsock != -1)
	close(hsock);
    hsock = -1;
}


void TConnect::sendreq(const char* fmt, ...) //отправить запрос на сервер
{
    va_list	args;
    va_start(args, fmt);
    sendreq(fmt,args);
    va_end(args);
}


void TConnect::sendreq(const char* fmt, va_list vl) //отправить запрос на сервер
{
    //формируем строку запроса
    char req[1024];
    vsnprintf(req, sizeof(req), fmt, vl);
    kLogPrintf("[%s]\n",req);
    //конектимся (если соединения еще нет)
    if (hsock == -1)
	createconnect(/*shost,sport*/);
    //отправляем запрос
    if (send(hsock, req, strlen(req), 0) != strlen(req))
    {
	kLogPrintf("send request %s:%s error\n",shost,sport);
	disconnect();
    }
}


char* TConnect::waitresult() //получить ответ от сервера (потом нужно освобождать память извне)
{
    if (hsock == -1)
	createconnect(/*shost,sport*/);
    //чтение частями из сокета
    char* answbuf = NULL;
    int totalBytesRcvd = 0;
    int answbuflen = 0;
    char bufpart[1024]; //фрагмент
    int bytesRcvd;
    do
    {
        if ((bytesRcvd = recv(hsock, bufpart, sizeof(bufpart), 0)) <= 0 )
	{
	    kLogPrintf("received filed %s:%s\n",shost,sport);
	    disconnect();
	    return NULL;
	}
	else
	//printf("allbytes= %d received %d bytes\n",totalBytesRcvd,bytesRcvd);
	//копируем в суммарный буфер
	answbuflen = answbuflen+bytesRcvd;
	answbuf = (char*)realloc(answbuf,answbuflen);
	memcpy(&answbuf[totalBytesRcvd], bufpart, bytesRcvd);
        totalBytesRcvd += bytesRcvd;
        if (answbuf[totalBytesRcvd-1] == '\003')
	{
	    answbuf = (char*)realloc(answbuf,answbuflen + 1);
	    answbuf[totalBytesRcvd-1] = '\0'; //003 -> 0
	    answbuf[totalBytesRcvd] = '\0'; //терминирующий 0
	    break;
	}
    }
    while (true);
    //    printf("'%s' \nlen=%d\n", answbuf, strlen(answbuf));
    return answbuf;
}

