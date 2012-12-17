#include "nmenu.h"
#include "kclog.h"


NMenu::NMenu(NRect rect) : NGroup(rect)
{
    mitems = NULL;
    menu = new_menu(mitems);
    setbackground(getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    setforeground(getcolorpair(COLOR_BLACK,COLOR_WHITE));
    set_menu_win(menu, win);
    post_menu(menu);
}


NMenu::~NMenu()
{
    kLogPrintf("NMenu::~NMenu()\n");
    unpost_menu(menu);
    free_menu(menu);
    if (mitems != NULL)
    {
	int i = 0;
	while (mitems[i] != NULL)
	    free_item(mitems[i++]);
	free(mitems);
    }
}


void NMenu::destroysubmenu() //закрыть субменю
{
    while (!items.empty())
    {
	delete items.front(); //кастинг поскольку в списке только NMenu*
	remove (items.front());
    }
}


void NMenu::refresh()
{
    //int rows, cols;
    //menu_format(menu, &rows, &cols);
    if (getheight() == 1) //только для горизонтального меню
    {
	//закрашиваем фоном правую часть строки
	wattrset(win,bgattr);
	wmove(win,0,menu->width);
	//clrtoeol();
	int x,y;
	do
	{
	    getyx(win, y, x);
	    wprintw(win," ");
	}
	while (x < getwidth() - 1);
	wattrset(win,0);
    }
    NGroup::refresh();
}


void NMenu::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( ev->done )
	return; //событие уже обработано кем-то ранее
    //отправляем всем подменю
    NGroup::eventhandle(ev);
    if ( ev->done )
	return; //выходим если какое-то подменю обработало событие
    //пытаемся обработать самостоятельно
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
	int y,x;
	menu_format(menu, &y, &x);
        switch(ev->keycode)
	{
	    case KEY_ENTER:
	    case '\n':
		action();
		break;
	    /*
	    case KEY_RIGHT:
		if ( x > 1)
		    menu_driver(menu, REQ_RIGHT_ITEM);
		else
		    ev->done = false;
		break;
	    case KEY_LEFT:
		if ( x > 1 )
		    menu_driver(menu, REQ_LEFT_ITEM);
		else
		    ev->done = false;
		break;
	    */
	    case KEY_UP:
		if ( y > 1 )
		    menu_driver(menu, REQ_UP_ITEM);
		else
		    ev->done = false;
		break;
	    case KEY_DOWN:
		if ( y > 1 )
		    menu_driver(menu, REQ_DOWN_ITEM);
		else
		    ev->done = false;
		break;
	    default:
		//если событие нам неизвестно, то отказываемся от его
		//обработки (снимаем флаг done)
		ev->done = false;
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}


