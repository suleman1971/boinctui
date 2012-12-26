#include "nhline.h"


void NHLine::refresh()
{
    wbkgd(win,bgcolor);
    wattrset(win,getcolorpair(COLOR_WHITE,COLOR_BLACK));
    whline(win, ACS_HLINE, getwidth()-0);
    NView::refresh();
};