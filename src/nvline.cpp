#include "nvline.h"


void NVLine::refresh()
{
    wbkgd(win,bgcolor);
    wvline(win, ACS_VLINE, getheight()-0);
    NView::refresh();
};