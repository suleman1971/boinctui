#include "mainwin.h"


#define INFPANWIDTH  20 //ширина инф панели


MainWin::MainWin(NRect rect) : NGroup(rect)
{
    tablheader = new NStaticText(NRect(1, rect.cols -2-(INFPANWIDTH)-1, 1, 1));
    tablheader->setstring(getcolorpair(COLOR_CYAN,COLOR_BLACK) | A_BOLD,"  #  state    done%%  project               est d/l   task %20s","");
    wtask = new TaskWin(NRect(getheight()/2, getwidth()-2-(INFPANWIDTH)-1, 2, 1)); //создаем окно процессов внутри wmain
    wmsg = new MsgWin(NRect(getheight()-wtask->getheight()-4, getwidth()-2-(INFPANWIDTH+1), wtask->getheight()+3, 1)); //создаем окно евентов
    hline = new NHLine(NRect(1, getwidth()-2-(INFPANWIDTH+1), wtask->getheight()+2, 1), NULL); //горизонтальная линия
    vline = new NVLine(NRect(getheight()-2, 1, 1 , getwidth()-INFPANWIDTH-2), NULL); //вертикальная линия

    panel1 = new InfoPanel(NRect(getheight()-2,INFPANWIDTH,1,getwidth()-INFPANWIDTH-1));
    caption = new NColorString(0,"");
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
    tablheader->resize(1, getwidth()-2-(INFPANWIDTH)-1);
    wtask->resize(getheight()/2, getwidth()-2-(INFPANWIDTH)-1); //размер окна задач
    wmsg->resize(getheight()-wtask->getheight()-4, getwidth()-2-(INFPANWIDTH+1));
    wmsg->move(wtask->getheight()+3, 1);
    hline->resize(1, getwidth()-2-(INFPANWIDTH+1)); //горизонтальная линия
    hline->move(wtask->getheight()+2, 1);
    vline->resize(getheight()-2, 1);
    vline->move(1 , getwidth()-INFPANWIDTH-2);
    panel1->resize(getheight()-2,INFPANWIDTH);
    panel1->move(1,getwidth()-INFPANWIDTH-1);
}


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
    //рисуем заголовок
    wmove(win,0,(getwidth()/2)-(caption->getlen()+1)/2);
    std::list<NColorStringPart*>::iterator it;
    for (it = caption->parts.begin(); it != caption->parts.end(); it++) //цикл по частям тек строки
    {
	NColorStringPart* part = *it;
	wattrset(win,part->attr);
	wprintw(win,"%s",part->s.c_str());
	wattrset(win,0);
    }
    //wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wattroff(win,A_BOLD);
    NGroup::refresh();
}

