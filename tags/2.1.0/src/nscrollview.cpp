#include "nscrollview.h"

NScrollView::~NScrollView()
{
    clearcontent();
}

void NScrollView::addstring(int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    NColorString* cs = new NColorString(attr, fmt, args);
    va_end(args);
    addstring(cs);
}


void NScrollView::clearcontent()
{
    std::vector<NColorString*>::iterator it;
    for (it = content.begin(); it != content.end(); it++)
    {
	delete (*it);
    }
    content.clear();
    needrefresh = true;
}


void NScrollView::drawcontent() //отрисовывает буфер строк
{
    //выводим строки начиная со startindex
    for (int line = 0; line < getheight(); line++) //цикл по экранным строкам
    {
	if (startindex+line < content.size())
	{
	    NColorString* cstring = content[startindex + line]; 
	    std::list<NColorStringPart*>::iterator it;
	    wmove(win,line,0);
	    for (it = cstring->parts.begin(); it != cstring->parts.end(); it++) //цикл по частям тек строки
	    {
		NColorStringPart* part = *it;
		wattrset(win,part->attr); //включаем атрибут
		wprintw(win,"%s",part->s.c_str());
//		wattrset(win,0); //отключаем атрибут
	    }
	    wclrtoeol(win); //очищаем до конца строки
	}
	else //очищаем нижнюю незанятую часть окна (если есть)
	{
	    wmove(win,line,0);
	    wclrtoeol(win); //очищаем до конца строки
	}
    }
}


void NScrollView::refresh()
{
    if (needrefresh)
    {
	drawcontent();
	NView::refresh();
    }
}


void NScrollView::resize(int rows, int cols)
{
    NView::resize(rows, cols);
    if (autoscroll)
    {
	setautoscroll(true); //костыль чтобы при ресайзе переустановилась позиция при автоскроле
    }
}


void NScrollView::scrollto(int delta)//сдвинуть отображение на drlta строк вверх или вниз
{
    int oldstartindex = startindex;
    startindex = startindex + delta;
    if ( startindex < 0 )
	startindex = 0;
    if ( startindex > content.size()-getheight() )
	startindex = content.size()-getheight()/* + 1*/; //+1 чтобы оставлять пустую строку
    if (oldstartindex != startindex) //позиция изменилась нужно перерисовываться
	needrefresh = true;
};


void NScrollView::setautoscroll(bool b) //true чтобы включить автоскроллинг
{
    int oldstartindex = startindex;
    autoscroll = b;
    if (b)
	startindex = content.size()-getheight();
    if (oldstartindex != startindex) //позиция изменилась нужно перерисовываться
	needrefresh = true;
};

