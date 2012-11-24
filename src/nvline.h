#ifndef NVLINE_H
#define NVLINE_H


#include "nview.h"


class NVLine : public NView
{
  public:
    NVLine(NRect rect, const char* s) : NView(rect) { };
    ~NVLine() { };
    void setbgcolor(int colorpair) { bgcolor = colorpair; };
    virtual void refresh();
  protected:
    int		bgcolor; //цвет фона
    const char*	mbsym; //символ
};

#endif //NVLINE_H