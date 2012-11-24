#include "ngroup.h"


NGroup::~NGroup()
{
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++)
    {
	delete (*it);
    }
}


void NGroup::setneedrefresh()
{
    NView::setneedrefresh();
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перерисовать все подэлементы
    {
	(*it)->setneedrefresh();
    }
};


void NGroup::refresh()
{
    NView::refresh(); //предок
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //перерисовать все подэлементы
    {
	(*it)->refresh();
    }
}


void NGroup::eventhandle(NEvent* ev)	//обработчик событий
{
    NView::eventhandle(ev); //предок
    std::list<NView*>::iterator it;
    for(it = items.begin(); it != items.end(); it++) //отправить всем подэлементам
    {
	(*it)->eventhandle(ev);
	if (ev->done)
	    break; //прекращаем если событие обработано
    }
}
