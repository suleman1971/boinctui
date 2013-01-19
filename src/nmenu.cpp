#include <string.h>
#include "nmenu.h"
#include "kclog.h"


NMenu::NMenu(NRect rect, bool horis) : NGroup(rect)
{
    mitems = NULL;
    ishoris = horis;
    menu = new_menu(mitems);
    setbackground(getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    setforeground(getcolorpair(COLOR_BLACK,COLOR_WHITE));
    set_menu_grey(menu, getcolorpair(COLOR_RED,COLOR_BLACK)); //цвет разделителей
    set_menu_win(menu, win);
    postmenu();
}


NMenu::~NMenu()
{
    kLogPrintf("NMenu::~NMenu()\n");
    unpostmenu();
    free_menu(menu);
    //освобождаем строки
    std::list<char*>::iterator it;
    for (it = itemnames.begin(); it != itemnames.end(); it++)
    {
	delete (*it);
    }
    for (it = itemcomments.begin(); it != itemcomments.end(); it++)
    {
	delete (*it);
    }
    //массив эл-тов
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
	delete items.front();
	remove (items.front());
    }
}


void NMenu::additem(const char* name, const char* comment) //добавить эл-т в меню
{
    unpostmenu();
    mitems = (ITEM**)realloc(mitems,(itemnames.size()+1)*sizeof(ITEM*));
    if (name == NULL) //финализация списка
    {
	mitems[itemnames.size()] = NULL;
	set_menu_items(menu, mitems);
	set_menu_mark(menu, " ");
	if ( !ishoris ) //для вертикальных есть рамка
	{
	    int lines = itemnames.size(); //видимых эл-тов
	    if (lines + getbegrow() > getmaxy(stdscr) - 8)
		lines = getmaxy(stdscr)-getbegrow() - 8;
	    set_menu_format(menu, lines, 1);
	    resize(lines+2,menu->width+3); //изменяем размер под кол-во эл-тов
	    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
	    box(win,0,0); //рамка
	}
	else //горизонтальное
	{
	    set_menu_format(menu, 1, itemnames.size());
	    menu_opts_off(menu, O_ROWMAJOR);
	    menu_opts_off(menu, O_SHOWDESC);
	    set_menu_mark(menu, "  ");
	}
	//set_menu_win(menu, win);
	menu_opts_off(menu,O_SHOWMATCH);
	postmenu();
    }
    else
    {
	if (strlen(name) > 0)
	{
	    itemnames.push_back(strdup(name));
	    itemcomments.push_back(strdup(comment));
	    mitems[itemnames.size()-1] = new_item(itemnames.back(),itemcomments.back());
	}
	else
	{
	    itemnames.push_back(strdup(" "));
	    itemcomments.push_back(strdup(" "));
	    mitems[itemnames.size()-1] = new_item(itemnames.back(),itemcomments.back());
	    item_opts_off(mitems[itemnames.size()-1], O_SELECTABLE);
	}
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
	//wattrset(win,0);
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
		{
		    if (item_index(current_item(menu)) > 0) //элемент не первый
		    {
			ITEM* preditem = mitems[item_index(current_item(menu)) - 1]; //предыдущий
			menu_driver(menu, REQ_UP_ITEM);
			if ( (item_opts(preditem) & O_SELECTABLE) == 0 )
			    menu_driver(menu, REQ_UP_ITEM); //чтобы пропустить разделитель
		    }
		}
		else
		    ev->done = false;
		break;
	    case KEY_DOWN:
		if ( y > 1 ) //вертикальное
		{
		    ITEM* nextitem = mitems[item_index(current_item(menu)) + 1]; //какой следующий
		    if (nextitem != NULL) //элемент не последний
		    {
			menu_driver(menu, REQ_DOWN_ITEM);
			if ( (item_opts(nextitem) & O_SELECTABLE) == 0 )
			    menu_driver(menu, REQ_DOWN_ITEM); //чтобы пропустить разделитель
		    }
		}
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


