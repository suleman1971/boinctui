#ifndef HELPWIN_H
#define HELPWIN_H

#include "ngroup.h"
#include "nstatictext.h"


class HelpWin : public NGroup
{
  public:
    HelpWin(int rows, int cols);
    ~HelpWin() { delete caption; };
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    char* caption; //строка заголовка
    NStaticText* text1;
};


#endif //HELPWIN_H