#include "nstatictext.h"
#include "kclog.h"


void NStaticText::appendstring(int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    content->vappend(attr, fmt, args);
    va_end(args);
    needrefresh = true;
    this->refresh();
}


void NStaticText::setstring(int attr, const char* fmt, ...)
{
    va_list	args;
    va_start(args, fmt);
    delete content;
    content = new NColorString(attr,fmt,args);
    va_end(args);
    needrefresh = true;
    this->refresh();
}


void NStaticText::refresh()
{
    wbkgd(win,bgcolor);
    std::list<NColorStringPart*>::iterator it;
    werase(win);
    wmove(win,0,0);
    if (align == 1) //центрирование
    {
	wmove(win,0,(getwidth()/2)-(content->getlen()+1)/2);
    }
    if (align == 2) //правое
    {
	wmove(win,0,getwidth()-content->getlen()/*+1*/);
    }
    for (it = content->parts.begin(); it != content->parts.end(); it++) //цикл по частям тек строки
    {
	NColorStringPart* part = *it;
	//kLogPrintf("[%d] %s\n", part->attr, part->s.c_str());
	wattrset(win,part->attr);
	wprintw(win,"%s",part->s.c_str());
	//wattrset(win,0);
    }
    wbkgdset(win,bgcolor);
    wclrtoeol(win); //очищаем до конца строки
    NView::refresh();
}
