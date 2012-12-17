#include "nvline.h"


void NVLine::refresh()
{
    wbkgd(win,bgcolor);
    wattrset(win,getcolorpair(COLOR_WHITE,COLOR_BLACK));
    wvline(win, ACS_VLINE, getheight()-0);
    NView::refresh();
};