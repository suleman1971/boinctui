// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

#include "mainwin.h"
#include "tuievent.h"
#include "kclog.h"


#define INFPANWIDTH  20 //ширина инф панели


MainWin::MainWin(NRect rect/*, Config* cfg*/) : NGroup(rect)
{
    //читаем опции из конфига если нет то создаем
    wtaskheightpercent = 5000;
    if (gCfg != NULL)
    {
	Item* rootcfg = gCfg->getcfgptr();
	if (rootcfg != NULL)
	{
	    Item* wtask_height_percent = rootcfg->findItem("wtask_height_percent");
	    if (wtask_height_percent == NULL) //создать
	    {
		wtask_height_percent = new Item("wtask_height_percent");
		wtask_height_percent->setivalue(wtaskheightpercent);
		rootcfg->addsubitem(wtask_height_percent);
	    }
	    wtaskheightpercent = wtask_height_percent->getivalue();
	}
    }
    colname.push_back("  #  ");
    colname.push_back("state ");
    colname.push_back("   done%%");
    colname.push_back("  project             ");
    colname.push_back("  est");
    colname.push_back("  rcv");
    colname.push_back("  dl ");
    colname.push_back("  application                   ");
    colname.push_back(" swap  ");
    colname.push_back("  task");
    tablheader = new NStaticText(NRect(1, rect.cols -2-(INFPANWIDTH)-1, 1, 1));
    tablheader->setstring(getcolorpair(COLOR_CYAN,getbgcolor()) | A_BOLD,"  #  state    done%%  project               est d/l   task");
    int wtaskheight = getheight() * wtaskheightpercent / 10000.0;
    if (wtaskheight < 5)
	wtaskheight = 5;
    if (wtaskheight > getheight() - 10)
	wtaskheight = getheight() - 10;
    wtask = new TaskWin(NRect(wtaskheight/*getheight()/2*/, getwidth()-2-(INFPANWIDTH)-1, 2, 1)); //создаем окно процессов внутри wmain
    setcoltitle();
    taskscrollbar = new NScrollBar(NRect(wtask->getheight()+2,1, wtask->getbegrow()-2, getwidth()-INFPANWIDTH-2), ACS_TTEE | A_BOLD, 0, ACS_VLINE | A_BOLD); //скроллбар панели задач
    wtask->setscrollbar(taskscrollbar);
    wmsg = new MsgWin(NRect(getheight()-wtask->getheight()-4, getwidth()-2-(INFPANWIDTH+1), wtask->getheight()+3, 1)); //создаем окно евентов
    hline = new NHLine(NRect(1, getwidth()-2-(INFPANWIDTH+1), wtask->getheight()+2, 1), NULL); //горизонтальная линия
//    vline = new NVLine(NRect(wtask->getheight()+1/*getheight()-2*/, 1, 1, getwidth()-INFPANWIDTH-2), NULL); //вертикальная линия
    msgscrollbar = new NScrollBar(NRect(wmsg->getheight()+2,1, wmsg->getbegrow()-1, getwidth()-INFPANWIDTH-2/*vline->getbegcol()*/),/*ACS_RTEE*/ACS_VLINE | A_BOLD,ACS_BTEE | A_BOLD, ACS_VLINE | A_BOLD); //скроллбар панели сообщений
    wmsg->setscrollbar(msgscrollbar);
    panel1 = new InfoPanel(NRect(getheight()-2,INFPANWIDTH,1,getwidth()-INFPANWIDTH-1));
    caption = new NColorString(0,"");
    insert(tablheader);
    insert(wtask);
    insert(wmsg);
    insert(hline);
//    insert(vline);
    insert(taskscrollbar);
    insert(msgscrollbar);
    insert(panel1);
}


void MainWin::resize(int rows, int cols)
{
    NGroup::resize(rows, cols);
    tablheader->resize(1, getwidth()-2-(INFPANWIDTH)-1);

    int wtaskheight = getheight() * wtaskheightpercent / 10000.0;
    if (wtaskheight < 5)
	wtaskheight = 5;
    if (wtaskheight > getheight() - 10)
	wtaskheight = getheight() - 10;
    wtask->resize(wtaskheight/*getheight()/2*/, getwidth()-2-(INFPANWIDTH)-1); //размер окна задач

    wmsg->resize(getheight()-wtask->getheight()-4, getwidth()-2-(INFPANWIDTH+1));
    wmsg->move(wtask->getheight()+3, 1);
    hline->resize(1, getwidth()-2-(INFPANWIDTH+1)); //горизонтальная линия
    hline->move(wtask->getheight()+2, 1);
//    vline->resize(wtask->getheight()+1/*getheight()-2*/, 1);
//    vline->move(1 , getwidth()-INFPANWIDTH-2);
    msgscrollbar->resize(wmsg->getheight()+2,1);
    msgscrollbar->move(wmsg->getbegrow()-1, getwidth()-INFPANWIDTH-2/*vline->getbegcol()*/);
    taskscrollbar->resize(wtask->getheight()+2,1);
    taskscrollbar->move(wtask->getbegrow()-2, getwidth()-INFPANWIDTH-2);
    panel1->resize(getheight()-2,INFPANWIDTH);
    panel1->move(1,getwidth()-INFPANWIDTH-1);
}


void MainWin::saveopttoconfig() //сохранить высоту списка задач и т.д. в дереве конфига
{
    //пишем в конфиг
    if (gCfg != NULL)
    {
	Item* rootcfg = gCfg->getcfgptr();
	if (rootcfg != NULL)
	{
	    Item* wtask_height_percent = rootcfg->findItem("wtask_height_percent");
	    if (wtask_height_percent != NULL)
		wtask_height_percent->setivalue(wtaskheightpercent);
	}
    }
}


void 	MainWin::setserver(Srv* srv) //установить отображаемый сервер
{
    this->srv = srv;
    wmsg->setserver(srv);
    wtask->setserver(srv);
    panel1->setserver(srv);
}


void MainWin::setcoltitle()
{
    std::string s = "";
    for (size_t i = 0; i < colname.size(); i++)
    {
	if (wtask->iscolvisible(i))
	    s = s + colname[i];
    }
    tablheader->setbgcolor(getcolorpair(COLOR_WHITE,getbgcolor()));
    tablheader->setstring(getcolorpair(COLOR_CYAN,getbgcolor()) | A_BOLD, s.c_str());
}


void MainWin::refresh()
{
    wattrset(win, getcolorpair(COLOR_WHITE,getbgcolor()) | A_BOLD);
    if(asciilinedraw == 1)
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    else
	box(win, ACS_VLINE, ACS_HLINE);
    //рисуем заголовок
    wmove(win,0,(getwidth()/2)-(caption->getlen()+1)/2);
    std::list<NColorStringPart*>::iterator it;
    for (it = caption->parts.begin(); it != caption->parts.end(); it++) //цикл по частям тек строки
    {
	NColorStringPart* part = *it;
	wattrset(win,part->attr);
	wprintw(win,"%s",part->s.c_str());
	//wattrset(win,0);
    }
    //wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    //wattroff(win, getcolorpair(COLOR_WHITE,getbgcolor()) | A_BOLD);
    setcoltitle();
    NGroup::refresh();
}


void MainWin::updatecaption()
{
    NColorString oldcaption = *caption;
    caption->clear();
    if (srv)
    {
        std::string shostid="";
        if(strlen(srv->hostid)>0)
            shostid="("+std::string(srv->hostid)+")";
        caption->append(getcolorpair(COLOR_WHITE,getbgcolor()) | A_BOLD," Host %s:%s%s ",srv->gethost(),srv->getport(),shostid.c_str());
        if (srv->loginfail)
            caption->append(getcolorpair(COLOR_WHITE,COLOR_RED) | A_BOLD,"unauthorized!");
        else
        {
    	    if (!srv->isconnected())
    	        caption->append(getcolorpair(COLOR_WHITE,COLOR_RED) | A_BOLD,"offline!");
        }
    }
    else
        caption->append(getcolorpair(COLOR_WHITE,COLOR_RED) | A_BOLD,"no servers");
    if (oldcaption != *caption)
	refresh();
}


void MainWin::eventhandle(NEvent* ev) 	//обработчик событий
{
    static int keycodebak = 0;
    static int keycodebakbak = 0;

    NGroup::eventhandle(ev); //предок

    if ( ev->done )
	return;
    if (ev->type == NEvent::evKB) //клавиатурные
    {
	//костыль для работы +- на нумпаде независимо от numlock (прямая интерпретация ESC последовательности)
	if ((keycodebakbak == 27) && (keycodebak == 79) && (ev->keycode == 107))
	    ev->keycode = '+';
	if ((keycodebakbak == 27) && (keycodebak == 79) && (ev->keycode == 109))
	    ev->keycode = '-';
        switch(ev->keycode)
	{
	    case '-': //меняем размер окна логов
	    case '+': //меняем размер окна логов
	    {
		int wtaskheight = getheight() * wtaskheightpercent / 10000.0; //высота в строках
		//игнорировать событие если дошли до ограничителей
		if ((ev->keycode == '+')&&(wtaskheight < 5))
		    break;
		if ((ev->keycode == '-')&&(wtaskheight > getheight() -10))
		    break;
		//расчитать новый процентный размер окна
		int delta = 10000.0/getheight();
		if (ev->keycode == '+')
		    delta*=-1;
		wtaskheightpercent+=delta;
		if (wtaskheightpercent > 10000)
		    wtaskheightpercent = 10000;
		wtaskheightpercent-=10;
		if (wtaskheightpercent < 0)
		    wtaskheightpercent = 0;
		//сохранить новое значение в конфиге
		saveopttoconfig();
		//ресайз и перерисовка
		resize(getheight(),getwidth());
		refresh();
		break;
	    }
	}
	keycodebakbak = keycodebak;
	keycodebak = ev->keycode;
    }
    if (ev->type == NEvent::evPROG) //прграммные
    {
	switch(ev->cmdcode)
	{
	    case evCOLVIEWCH: //изменился набор колонок
	    {
		setcoltitle();
		tablheader->refresh();
		wtask->refresh();
	    }
	} //switch
    }
    //событие таймера
    if (ev->type == NEvent::evTIMER) //таймер
    {
	updatecaption();
    }
}

