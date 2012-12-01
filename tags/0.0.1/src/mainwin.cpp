#include "mainwin.h"


#define INFPANWIDTH  20 //ширина инф панели


MainWin::MainWin(NRect rect/*, void* hconnect*/) : NGroup(rect)
{
    //gsrvlist = new SrvList(cfg);
    tablheader = new NStaticText(NRect(1, rect.cols -2-(INFPANWIDTH)-1, rect.begrow + 1, 1));
    tablheader->setstring(COLOR_PAIR(3)|A_BOLD,"  #  state    done%%        project         Est.     task name %20s","");
    wtask = new TaskWin(NRect(getheight()/2, getwidth()-2-(INFPANWIDTH)-1, 3, 1)/*, hconnect*/); //создаем окно процессов внутри wmain
    wmsg = new MsgWin(NRect(getheight()-wtask->getheight()-4, getwidth()-2-(INFPANWIDTH+1), wtask->getheight()+4, 1)/*, hconnect*/); //создаем окно евентов
    hline = new NHLine(NRect(1, getwidth()-2-(INFPANWIDTH+1), wtask->getheight()+3, 1), NULL); //горизонтальная линия
    vline = new NVLine(NRect(getheight()-2, 1, 2 , getwidth()-INFPANWIDTH-2), NULL); //горизонтальная линия

    panel1 = new InfoPanel(NRect(getheight()-2,INFPANWIDTH,2,getwidth()-INFPANWIDTH-1)/*, hconnect*/);
    insert(tablheader);
    insert(wtask);
    insert(wmsg);
    insert(hline);
    insert(vline);
    insert(panel1);
}


void MainWin::resize(int rows, int cols)
{
    NGroup::resize(rows, cols);
    tablheader->resize(1, rect.cols-2-(INFPANWIDTH)-1);
    wtask->resize(getheight()/2, getwidth()-2-(INFPANWIDTH)-1); //размер окна задач
    wmsg->resize(getheight()-wtask->getheight()-4, getwidth()-2-(INFPANWIDTH+1));
    wmsg->move(wtask->getheight()+4, 1);
    hline->resize(1, getwidth()-2-(INFPANWIDTH+1)); //горизонтальная линия
    hline->move(wtask->getheight()+3, 1);
    vline->resize(getheight()-2, 1);
    vline->move(2 , getwidth()-INFPANWIDTH-2);
    panel1->resize(getheight()-2,INFPANWIDTH);
    panel1->move(2,getwidth()-INFPANWIDTH-1);
}

/*
void MainWin::move(int begrow, int begcol)
{
    NGroup::move(begrow, begcol);
}
*/

void 	MainWin::setserver(Srv* srv) //установить отображаемый сервер
{
    wmsg->setserver(srv);
    wtask->setserver(srv);
    panel1->setserver(srv);
}


void MainWin::refresh()
{
    wattron(win,A_BOLD);
    box(win, ACS_VLINE, ACS_HLINE);
    //wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wattroff(win,A_BOLD);
    NGroup::refresh();
}

