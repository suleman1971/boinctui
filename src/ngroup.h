#ifndef NGROUP_H
#define NGROUP_H


#include <list>
#include "nview.h"


class NGroup : public NView
{
  public:
    NGroup(NRect rect) : NView(rect) {};
    virtual ~NGroup();
    void insert(NView* view) { view->setowner(this); items.push_back(view); };
    void remove(NView* view) { items.remove(view); };
    virtual void refresh();
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    virtual void setneedrefresh();
  protected:
    std::list<NView*> items; //список вложенных элементов
};

#endif // NGROUP_H
