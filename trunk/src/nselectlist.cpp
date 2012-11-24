#include <stdlib.h>
#include <string.h>
#include "nselectlist.h"


void NSelectList::addstring(void* userobj, int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    NColorString* cs = new NColorString(attr, fmt, args);
    va_end(args);
    NScrollView::addstring(cs);
    objects.push_back(userobj);
}


void NSelectList::addstring(void* userobj, NColorString* cstring)
{
    NScrollView::addstring(cstring);
    objects.push_back(userobj);
}


void NSelectList::drawcontent() //отрисовывает буфер строк
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
		if (startindex + line != selectedindex)
		    wattrset(win,part->attr); //включаем атрибут
		else
		    wattrset(win,A_REVERSE | part->attr); //включаем атрибут
		wprintw(win,"%s",part->s.c_str());
		//wattrset(win,0); //отключаем атрибут
	    }
	    if (startindex + line != selectedindex)
	        wclrtoeol(win); //очищаем до конца строки
	    else
	    {
		//забиваем пробелами чтобы был фон
		int line,col;
		getyx(win,line,col);
		int l = getwidth()-col;
		char* buf = (char*)malloc(l+1);
		memset(buf,' ',l);
		buf[l] = '\0';
		//int a = winch(win) & A_ATTRIBUTES;
		//wattrset(win,a | A_REVERSE); //включаем атрибут
		wprintw(win,"%s",buf);
		wattrset(win,0); //отключаем атрибут
		free(buf);
	    }
	    wattrset(win,0); //отключаем атрибут
	}
	else //очищаем нижнюю незанятую часть окна (если есть)
	{
	    wmove(win,line,0);
	    wclrtoeol(win); //очищаем до конца строки
	}
    }
}


void NSelectList::refresh()
{
    if (needrefresh)
    {
	drawcontent();
	NView::refresh(); //Именно так!!!
    }
}


void* NSelectList::getselectedobj()
{
    if ((selectedindex >= 0)&&(selectedindex < objects.size()))
    {
	return objects[selectedindex];
    }
    else
	return NULL;
}


void NSelectList::selectorup()
{
    if (selectedindex >= 0)
    {
	selectedindex--;
	needrefresh = true;
	//проверяем не нужно ли скролить
	if (selectedindex - startindex < 4) //видимых строк выше серлектора осталось 
	{
	    scrollto(-1);
	}
    }
}


void NSelectList::selectordown()
{
    if (selectedindex < (int)content.size())
    {
	selectedindex++;
	needrefresh = true;
	//проверяем не нужно ли скролить
	if (startindex + getheight() - selectedindex < 4) //видимых строк ниже серлектора осталось 
	{
	    scrollto(1);
	}
    }
}
