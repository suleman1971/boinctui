#ifndef ABOUT_H
#define ABOUT_H

#include "ngroup.h"


class AboutWin : public NGroup
{
  public:
    AboutWin(int rows, int cols);
    ~AboutWin() { delete caption; };
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    char* caption; //строка заголовка
};

#endif //ABOUT_H