#ifndef NSELECTLIST_H
#define NSELECTLIST_H


#include "nscrollview.h"


class NSelectList : public NScrollView //список со скроллингом и селектором
{
  public:
    NSelectList(NRect rect) : NScrollView(rect) { selectedindex = -1; setselectorbgcolor(COLOR_WHITE); };
    void addstring(void* userobj, int attr, const char* fmt, ...); //userobj произвольные данные связанные со строкой
    void addstring(void* userobj, NColorString* sctring);
    virtual void drawcontent();
    virtual void refresh();
    virtual void selectorup();
    virtual void selectordown();
    void* getselectedobj(); //вернет указатель или NULL
    void setselectorbgcolor(short color) { selectorbgcolor = color; };
    //virtual bool objcmpeqv(void* obj1, void* obj2) { return obj1==obj2; };
  protected:
    //void*	selectedobj; 		//выделенный объект
    int selectedindex; //номер выделенной строки
    std::vector<void*> 	objects; 	//объекты, ассоциированные с визуальными строками
    short	selectorbgcolor;	//номер цвета фона выделеной строки
};


#endif //NSELECTLIST_H