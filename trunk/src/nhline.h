#ifndef NHLINE_H
#define NHLINE_H


#include "nview.h"


class NHLine : public NView
{
  public:
    NHLine(NRect rect, const char* s) : NView(rect) { };
    ~NHLine() { };
    void setbgcolor(int colorpair) { bgcolor = colorpair; };
    virtual void refresh();
  protected:
    int		bgcolor; //цвет фона
    const char*	mbsym; //символ
};

#endif //NHLINE_H