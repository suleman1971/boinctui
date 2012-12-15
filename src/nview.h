#ifndef NVIEW_H
#define NVIEW_H

#include <panel.h>
#include "nrect.h"
#include "nevent.h"

class NGroup;


void initcolorpairs();
int getcolorpair(int fcolor, int bcolor); //получить пару для комбинации цветов


class NView //базовый визуальный класс
{
  public:
    NView(NRect rect);
    virtual ~NView() { del_panel(pan); delwin(win); };
    //virtual void draw() {};
    virtual void resize(int rows, int cols);
    virtual void move(int begrow, int begcol);
    PANEL* getpan() 	{ return pan; }; //!!!!!!!!!!!!!!!!!!!!
    int getwidth()  	{ return rect.cols; }; 	//ширина в символах
    int getheight() 	{ return rect.rows; }; 	//высота в строках
    int getbegrow()	{ return rect.begrow; }; //начальная строка
    int getbegcol()	{ return rect.begcol; }; //начальный столбец
    void erase() 	{ werase(win); }; 	//очистить
    virtual void refresh(); 	//перерисовать
    virtual void setneedrefresh() { needrefresh = true; };
    virtual void eventhandle(NEvent* ev) {/*EMPTY*/}; 	//обработчик событий
    virtual void putevent(NEvent* ev); //отправить событие по цепочке владельцев в очередь
    void setowner(NGroup* owner);
  protected:
    NGroup*	owner;	//владелец эл-та
    WINDOW* 	win;	//окно curses
    PANEL*	pan;	//панель curses на основе окна win
    bool needrefresh; //устанавливается в true когда нужна перерисовка
    #ifdef DEBUG
    int		refreshcount; //счетчик обновлений
    #endif
  private:
    int getabsbegrow(); //получить начальную строку (абсолютную на экране)
    int getabsbegcol(); //получить начальный столбец (абсолютный на экране)
    NRect	rect;	//координаты и размер

};

#endif //NVIEW_H