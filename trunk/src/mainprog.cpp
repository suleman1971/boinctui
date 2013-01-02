#include <sys/ioctl.h>
#include <signal.h>
#include "kclog.h"
#include "mainprog.h"
#include "tuievent.h"


MainProg::MainProg()
{
    done = false;
    cfg = new Config(".boinctui.cfg");
    gsrvlist = new SrvList(cfg);
    cfgform = NULL;
    about = NULL;
    help = NULL;
    addform = NULL;
    //основное окно
    wmain 	= new MainWin(NRect(getmaxy(stdscr)-2, getmaxx(stdscr), 1, 0)); //создаем основное окно
    insert(wmain);
    wmain->setserver(gsrvlist->getcursrv()); //отображать первый в списке сервер
    menu = new TopMenu();
    menu->setserver(gsrvlist->getcursrv()); //отображать первый в списке сервер
    insert(menu);
    setcaption();
    //статус строка
    wstatus 	= new NStaticText(NRect(1, getmaxx(stdscr), getmaxy(stdscr)-1, 0)); //создаем окно статуса
    insert(wstatus);
    wstatus->setbgcolor(getcolorpair(COLOR_WHITE,COLOR_GREEN));
    int attrYG = A_BOLD | getcolorpair(COLOR_YELLOW,COLOR_GREEN);
    int attrWG = A_BOLD | getcolorpair(COLOR_WHITE,COLOR_GREEN);
    wstatus->appendstring(attrYG, " PgUp/PgDn");
    wstatus->appendstring(attrWG, " Scroll Msg |");
    wstatus->appendstring(attrYG, " Up/Dn");
    wstatus->appendstring(attrWG, " Select |");
    wstatus->appendstring(attrYG, " S");
    wstatus->appendstring(attrWG, "uspend |");
    wstatus->appendstring(attrYG, " R");
    wstatus->appendstring(attrWG, "esume |");
    wstatus->appendstring(attrYG, " F9");
    wstatus->appendstring(attrWG, " Menu |");
}


void MainProg::setcaption()
{
    wmain->caption->clear();
    wmain->caption->append(getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD," Host %s:%s ",gsrvlist->getcursrv()->gethost(),gsrvlist->getcursrv()->getport());
    wmain->refresh();
}


void MainProg::smartresize()
{
    if (!MainProg::needresize)
	return;
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
    menu->resize(1, getmaxx(stdscr)); //ширина верхнего меню
    wmain->resize(getmaxy(stdscr)-2, getmaxx(stdscr));
    wstatus->resize(1, getmaxx(stdscr)); //ширина статус строки
    wstatus->move(getmaxy(stdscr)-1,0); //позиция статус строки
    if (cfgform != NULL)
	cfgform->move(getmaxy(stdscr)/2-cfgform->getheight()/2,getmaxx(stdscr)/2-cfgform->getwidth()/2); //окно конфигурации (если есть)
    if (about != NULL)
	about->move(getmaxy(stdscr)/2-about->getheight()/2,getmaxx(stdscr)/2-about->getwidth()/2); //окно About (если есть)
    if (help != NULL)
	help->move(getmaxy(stdscr)/2-help->getheight()/2,getmaxx(stdscr)/2-help->getwidth()/2); //окно About (если есть)
    if (addform != NULL)
	addform->move(getmaxy(stdscr)/2-addform->getheight()/2,getmaxx(stdscr)/2-addform->getwidth()/2); //окно Add Project (если есть)
    MainProg::needresize = false;
}


void MainProg::eventhandle(NEvent* ev)	//обработчик событий КОРНЕВОЙ!
{
    NProgram::eventhandle(ev);
    if (ev->done) //если событие не обработано обработать здесь
	return;
    if (ev->type == NEvent::evKB) //клавиатурные
    {
        switch(ev->keycode)
	{
	    case 'q':
	    case 'Q':
		done = true; //выходим
		break;
	    case 'n':
	    case 'N':
		menu->disable();
		gsrvlist->nextserver();
		wmain->setserver(gsrvlist->getcursrv());
		menu->setserver(gsrvlist->getcursrv());
		setcaption();
		break;
	    case 'c':
	    case 'C':
		if (cfgform == NULL)
		{
		    menu->disable();
		    cfgform = new CfgForm(15,54,cfg);
		    insert(cfgform);
		    cfgform->settitle("Configuration");
		    cfgform->refresh();
		}
		break;
	    case 27:
		menu->disable();
		//деструктим форму
		if (cfgform != NULL)
		{
		    remove(cfgform);
		    delete cfgform;
		    cfgform = NULL;
		}
		break;
	    case KEY_F(9):
		if (!menu->isenable())
		    menu->enable();
		else
		    menu->disable();
		break;
	    default:
		kLogPrintf("KEYCODE=%d\n", ev->keycode);
		break;
	} //switch
    }
    if (ev->type == NEvent::evPROG) //прграммные
    {
	switch(ev->cmdcode)
	{
	    case evCFGCH: //событие при изменении конфига
	    {
		menu->disable();
		//деструктим форму
		if (cfgform != NULL)
		{
		    remove(cfgform);
		    delete cfgform;
		    cfgform = NULL;
		}
		//реакция на изменение конфига
		gsrvlist->refreshcfg();
		wmain->setserver(gsrvlist->getcursrv()); //отображать первый в списке сервер
		menu->setserver(gsrvlist->getcursrv()); //отображать первый в списке сервер
		setcaption();
		break;
	    }
	    case evABOUT: //событие About win
	    {
		if (about != NULL)
		{
		    remove(about);
		    delete about;
		    about = NULL;
		}
		else
		{
		    about = new AboutWin(2,40);
		    insert(about);
		    about->move(getmaxy(stdscr)/2-about->getheight()/2,getmaxx(stdscr)/2-about->getwidth()/2); //центрируем
		}
		break;
	    }
	    case evKEYBIND: //событие KeyBinding win
	    {
		if (help != NULL)
		{
		    remove(help);
		    delete help;
		    help = NULL;
		}
		else
		{
		    help = new HelpWin(2,40);
		    insert(help);
		    help->move(getmaxy(stdscr)/2-help->getheight()/2,getmaxx(stdscr)/2-help->getwidth()/2); //центрируем
		}
		break;
	    }
	    case evBENCHMARK: //запустить бенчмарк
	    {
		Srv* srv = gsrvlist->getcursrv();
		if (srv != NULL)
		    srv->runbenchmarks();
		break;
	    }
	    case evADDPROJECT: //добавить проект
	    {
		if (addform != NULL)
		{
		    remove(addform);
		    delete addform;
		    addform = NULL;
		}
		else
		{
		    TuiEvent* ev1 = (TuiEvent*)ev;
		    Srv* srv = gsrvlist->getcursrv();
		    if (ev1->srv != NULL)
		    {
			addform = new AddProjectForm(30,65,ev1->srv,ev1->prjname.c_str(),ev1->userexist);
			insert(addform);
			addform->move(getmaxy(stdscr)/2-addform->getheight()/2,getmaxx(stdscr)/2-addform->getwidth()/2); //центрируем
		    }
		}
		break;
	    }
	} //switch
    }
}


bool MainProg::mainloop() //основной цикл порождающий события
{
    int takt = 0; //номер оборота цикла
    time_t updatetime; //время последней отрисовки
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGWINCH); //маска для сигнала 
    do
    {
	//блокировка сигнала изменения окна SIGWINCH на время отрисовки (из-за нереентерабельности курсес)
	sigprocmask(SIG_BLOCK, &newset, 0);
	//если нужен ресайз - перерисовать полностью
	if (MainProg::needresize)
	{
	    smartresize();
	    refresh();
	    menu->refresh();
	    //wmain->erase();
	    wstatus->erase();
	    wmain->refresh();
	    wstatus->refresh();
	}
	//есть символ в буфере
	int ic;
	if ( (ic = getch()) != ERR ) //символ(ы) есть?
	{
	    NEvent* event = new NEvent(NEvent::evKB, ic); //создаем событие
	    putevent(event); //отправить в очередь
	}
	//нет событий в очереди
	if (evqueue.empty())
	{
	    if (time(NULL)-updatetime > 2) //перерисовывались меньше 2х секунд назад
	    {
		wmain->wtask->updatedata(); 	//запросить данные с сервера
		wmain->wtask->refresh();		//перерисовать окно
		//обновить окно сообщений
		//if ( (takt % 2) == 0 )
		//{
		    wmain->wmsg->updatedata(); 	//запросить данные с сервера
		    wmain->wmsg->refresh(); 		//перерисовать окно
		//}
		//обновить инф панель
		//if ( (takt % 2) == 0 )
		//{
		    wmain->panel1->updatedata(); 	//сформировать данные
		    wmain->panel1->refresh(); 		//перерисовать окно
		//}
		time(&updatetime);
	    }
	    //takt++;
	}
	else
	{
	    NEvent* event = evqueue.front(); //получить первое событие из очереди
	    evqueue.pop();
	    this->eventhandle(event); //отправить событие обработчику
	    delete event; //удаляем отработанное событие
	    //обновляем экран
	}
	update_panels();
	doupdate(); //физически выполняет перерисовку
	//разблокируем SIGWINCH
	sigprocmask(SIG_UNBLOCK, &newset, 0); 
    }
    while(!done);
}
