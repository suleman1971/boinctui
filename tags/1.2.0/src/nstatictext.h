#ifndef NSTATICTEXT_H
#define NSTATICTEXT_H


#include "nview.h"
#include "ncolorstring.h"


class NStaticText : public NView
{
  public:
    NStaticText(NRect rect) : NView(rect) { content = new NColorString(0,""); align = 0; setbgcolor(getcolorpair(COLOR_WHITE,COLOR_BLACK));};
    virtual ~NStaticText() { delete content; };
    void appendstring(int attr, const char* fmt, ...);
    void setstring(int attr, const char* fmt, ...);
    void setbgcolor(int colorpair) { bgcolor = colorpair; };
    void setalign(int align) { this->align = align; }; //0-левое 2-правое 1-центр
    virtual void refresh();
  protected:
    NColorString* 	content;
    int			align;   //вравнивание
    int			bgcolor; //цвет фона
};

#endif //NSTATICTEXT_H