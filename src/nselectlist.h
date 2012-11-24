#ifndef NSELECTLIST_H
#define NSELECTLIST_H


#include "nscrollview.h"


class NSelectList : public NScrollView //список со скроллингом и селектором
{
  public:
    NSelectList(NRect rect) : NScrollView(rect) { /*selectedobj = NULL;*/ selectedindex = -1;};
    void addstring(void* userobj, int attr, const char* fmt, ...); //userobj произвольные данные связанные со строкой
    void addstring(void* userobj, NColorString* sctring);
    virtual void drawcontent();
    virtual void refresh();
    virtual void selectorup();
    virtual void selectordown();
    void* getselectedobj(); //вернет указатель или NULL
    //virtual bool objcmpeqv(void* obj1, void* obj2) { return obj1==obj2; };
  protected:
    //void*	selectedobj; 		//выделенный объект
    int selectedindex; //номер выделенной строки
    std::vector<void*> 	objects; 	//объекты, ассоциированные с визуальными строками
};


#endif //NSELECTLIST_H