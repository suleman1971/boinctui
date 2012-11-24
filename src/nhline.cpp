#include "nhline.h"


void NHLine::refresh()
{
    wbkgd(win,bgcolor);
    whline(win, ACS_HLINE, getwidth()-0);

/*
    std::list<NColorStringPart*>::iterator it;
    wmove(win,0,0);
    for (it = content->parts.begin(); it != content->parts.end(); it++) //цикл по частям тек строки
    {
	NColorStringPart* part = *it;
	wattrset(win,part->attr);
	wprintw(win,"%s",part->s.c_str());
	wattrset(win,0);
    }
    wclrtoeol(win); //очищаем до конца строки
*/
    NView::refresh();
};