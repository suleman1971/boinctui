#include "nview.h"
#include "ngroup.h"
#include "kclog.h"


void initcolorpairs()
{
    int npair; //номер генерируемой пары
    for ( int b = 0; b < COLORS; b++ ) //фон
    {
	for ( int f = 0; f < COLORS; f++ ) //текст
	{
	    npair = 1 + b * COLORS + f;
	    if ( npair <= COLOR_PAIRS )
		init_pair(npair, f, b);
	}
    }
}


int getcolorpair(int fcolor, int bcolor) //получить пару для комбинации цветов
{
    int npair = 1 + bcolor * COLORS + fcolor;
    if ( npair <= COLOR_PAIRS)
	return COLOR_PAIR(npair);
    else
	return COLOR_PAIR(0);
}


NView::NView(NRect rect)
{
    this->rect = rect;
    win = newwin(rect.rows, rect.cols, rect.begrow, rect.begcol); //создаем окно curses
    pan = new_panel(win);
    scrollok(win,false);
    needrefresh = true;
    owner = NULL;
    #ifdef DEBUG
    refreshcount = 0; //счетчик обновлений
    #endif

}


void NView::setowner(NGroup* owner)
{
    this->owner = owner;
    //позиционировать относительно овнера
    move(getbegrow(),getbegcol());
}


void NView::resize(int rows, int cols)
{
    wresize(win, rows, cols);
    rect.rows = rows;
    rect.cols = cols;
    needrefresh = true;
}


void NView::refresh() //перерисовать
{
    if (needrefresh)
    {
	#ifdef DEBUG
	refreshcount++;
	mvwprintw(win,0,getmaxx(win)-10,"r=%d",refreshcount);
	#endif
	update_panels();
	//doupdate();
	needrefresh = false;
    }
}


int NView::getabsbegrow() //получить начальную строку (на экране)
{
    if (owner == NULL)
	return getbegrow();
    else
	return getbegrow() + owner->getabsbegrow();
}


int NView::getabsbegcol() //получить начальный столбец (на экране)
{
    if (owner == NULL)
	return getbegcol();
    else
	return getbegcol() + owner->getabsbegcol();
}


void NView::move(int begrow, int begcol)
{
    rect.begrow = begrow;
    rect.begcol = begcol;
    move_panel(pan, getabsbegrow(), getabsbegcol());
}


void NView::putevent(NEvent* ev) //отправить событие по цепочке владельцев в очередь
{
    if (owner != NULL)
	owner->putevent(ev);
}

