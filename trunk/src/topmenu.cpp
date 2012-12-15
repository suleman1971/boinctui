#include <string.h>
#include "topmenu.h"
#include "kclog.h"

//Названия пунктов верхнего меню
#define M_FILE 				"File"
#define M_PROJECTS			"Projects"
#define M_TASKS				"Tasks"
#define M_ACTIVITY			"Activity"
#define M_HELP				"Help"
//Названия пунктов меню "File"
#define M_NEXT_HOST			"Next BOINC host"
#define M_CONFIG_HOSTS			"Configure host list"
#define M_QUIT				"Quit boinctui"
//Названия пунктов меню "Projects"
#define M_UPDATE_PROJECT		"Update project"
#define M_SUSPEND_PROJECT		"Suspend project"
#define M_RESUME_PROJECT		"Resume project"
#define M_NO_NEW_TASKS_PROJECT		"No new tasks"
#define M_ALLOW_NEW_TASKS_PROJECT	"Allow new tasks"
#define M_RESET_PROJECT			"Reset project"
//Названия пунктов меню "Tasks"
#define M_SUSPEND_TASK			"Suspend task"
#define M_RESUME_TASK			"Resume task"
#define M_ABORT_TASK			"Abort task"
//Названия пунктов меню "Activity"
#define M_ACTIVITY_ALWAYS		"Run always"
#define M_ACTIVITY_AUTO			"Run based on preferences"
#define M_ACTIVITY_NEVER		"Suspend"
//Названия пунктов меню "Help"
#define M_ABOUT				"About"
#define M_KEY_BINDINGS			"Hot keys list"


TopMenu::TopMenu() : NMenu(NRect(1,getmaxx(stdscr),0,0))
{
    setserver(NULL);
    unpost_menu(menu);
    mitems = (ITEM**)realloc(mitems,6*sizeof(ITEM*));
    mitems[0] = new_item(M_FILE,"");
    mitems[1] = new_item(M_PROJECTS,"");
    mitems[2] = new_item(M_TASKS,"");
    mitems[3] = new_item(M_ACTIVITY,"");
    mitems[4] = new_item(M_HELP,"");
    mitems[5] = NULL; 
    set_menu_items(menu, mitems);
    menu_opts_off(menu, O_ROWMAJOR);
    menu_opts_off(menu, O_SHOWDESC);
    set_menu_mark(menu, "  ");
    setbackground(getcolorpair(COLOR_WHITE,COLOR_GREEN)|A_BOLD);
    enableflag = true;
    disable();
    set_menu_win(menu, win);
    set_menu_format(menu, 1, menu->nitems);
    post_menu(menu);
}


bool TopMenu::action() //открыть субменю
{
    bool result = false;
    if (!items.empty()) //если уже открыто выходим
	return false;
    //создаем подменю
    int begincol = (menu->itemlen+menu->spc_cols)*item_index(current_item(menu)) + menu->spc_cols; //смещение на экране по горизонтали
    if ( strcmp(item_name(current_item(menu)),M_FILE) == 0 )
    {
	insert(new FileSubMenu(NRect(5,25,1, begincol)));
	result = true;
    }
    if ( strcmp(menu->curitem->name.str,M_PROJECTS) == 0 )
    {
	insert(new ProjectsSubMenu(NRect(5,25,1, begincol), srv));
	result = true;
    }
    if ( strcmp(item_name(current_item(menu)),M_TASKS) == 0 )
    {
	insert(new TasksSubMenu(NRect(5,25,1, begincol)));
	result = true;
    }
    if ( strcmp(item_name(current_item(menu)),M_ACTIVITY) == 0 )
    {
	insert(new ActivitySubMenu(NRect(5,25,1, begincol), srv));
	result = true;
    }
    if ( strcmp(item_name(current_item(menu)),M_HELP) == 0 )
    {
	insert(new HelpSubMenu(NRect(5,25,1, begincol)));
	result = true;
    }
    return result;
}


void TopMenu::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( !isenable() )
	return; //не обрабатывать если меню не активно
    if ( ev->done )
	return; //не обрабатывать если уже обработано кем-то ранее
    //отправляем событие всем подменю
    NMenu::eventhandle(ev); //предок
    if ( ev->done )
	return; //выход если какое-то подменю отработало событие
    //пытаемся обработать самостоятельно
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_ENTER:
	    case '\n':
		if (!createsubmenu())
		    ev->done = false; //пусть обрабатывает владелец
		break;
	    case KEY_UP: 	//открыть подменю по стрелкевверх
	    case KEY_DOWN: 	//открыть подменю по стрелкевниз
		action();
		break;
	    case 27:
		if ( !items.empty() )
		    destroysubmenu();
		else
		    ev->done = false; //пусть обрабатывает владелец
		break;
	    case KEY_RIGHT:
		menu_driver(menu, REQ_RIGHT_ITEM);
		if ( !items.empty() ) //подменю открыто
		{
		    destroysubmenu(); //закрыть старое
		    action(); //открыть новое
		}
		break;
	    case KEY_LEFT:
		menu_driver(menu, REQ_LEFT_ITEM);
		if ( !items.empty() ) //подменю открыто
		{
		    destroysubmenu(); //закрыть старое
		    action(); //открыть новое
		}
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}


//=============================================================================================


FileSubMenu::FileSubMenu(NRect rect) : NMenu(rect)
{
    unpost_menu(menu);
    mitems = (ITEM**)realloc(mitems,4*sizeof(ITEM*));
    mitems[0] = new_item(M_NEXT_HOST,"  N");
    mitems[1] = new_item(M_CONFIG_HOSTS,"  C");
    mitems[2] = new_item(M_QUIT,"  Q");
    mitems[3] = NULL;
    set_menu_items(menu, mitems);
    resize(menu->nitems+2, menu->width+3); //изменяем размер под кол-во эл-тов
    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
    setbackground(A_BOLD);
    box(win,0,0); //рамка
    set_menu_mark(menu, " ");
    menu_opts_off(menu,O_SHOWMATCH);
    set_menu_win(menu, win);
    set_menu_format(menu, menu->nitems, 1);
    post_menu(menu);
}


bool FileSubMenu::action()
{
    putevent(new NEvent(NEvent::evKB, KEY_F(9))); //закрыть осн меню
    if ( strcmp(item_name(current_item(menu)),M_NEXT_HOST) == 0 )
	putevent(new NEvent(NEvent::evKB, 'N')); //создаем событие иммитирующее нажатие 'N'
    if ( strcmp(item_name(current_item(menu)),M_CONFIG_HOSTS) == 0 )
	putevent(new NEvent(NEvent::evKB, 'C')); //создаем событие иммитирующее нажатие 'C'
    if ( strcmp(item_name(current_item(menu)),M_QUIT) == 0 )
	putevent(new NEvent(NEvent::evKB, 'Q')); //создаем событие иммитирующее нажатие 'Q'
    return true;
}


//=============================================================================================


HelpSubMenu::HelpSubMenu(NRect rect) : NMenu(rect)
{
    unpost_menu(menu);
    mitems = (ITEM**)realloc(mitems,3*sizeof(ITEM*));
    mitems[0] = new_item(M_ABOUT,"");
    mitems[1] = new_item(M_KEY_BINDINGS,"");
    mitems[2] = NULL;
    set_menu_items(menu, mitems);
    resize(menu->nitems+2, menu->width+3); //изменяем размер под кол-во эл-тов
    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
    setbackground(A_BOLD);
    box(win,0,0); //рамка
    set_menu_mark(menu, " ");
    menu_opts_off(menu,O_SHOWMATCH);
    set_menu_win(menu, win);
    set_menu_format(menu, menu->nitems, 1);
    post_menu(menu);
}


bool HelpSubMenu::action()
{
    putevent(new NEvent(NEvent::evKB, KEY_F(9))); //закрыть осн меню
    if ( strcmp(item_name(current_item(menu)),M_ABOUT) == 0 )
	putevent(new NEvent(NEvent::evPROG, 3)); //создаем событие с кодом 3 "окно About"
    if ( strcmp(item_name(current_item(menu)),M_KEY_BINDINGS) == 0 )
	putevent(new NEvent(NEvent::evPROG, 4)); //создаем событие с кодом 4 "окно Key Bindings"
    return true;
}


//=============================================================================================

ProjectsSubMenu::ProjectsSubMenu(NRect rect, Srv* srv) : NMenu(rect)
{
    this->srv = srv;
    unpost_menu(menu);
    mitems = (ITEM**)realloc(mitems,7*sizeof(ITEM*));
    mitems[0] = new_item(M_UPDATE_PROJECT,"...");
    mitems[1] = new_item(M_SUSPEND_PROJECT ,"...");
    mitems[2] = new_item(M_RESUME_PROJECT,"...");
    mitems[3] = new_item(M_NO_NEW_TASKS_PROJECT,"...");
    mitems[4] = new_item(M_ALLOW_NEW_TASKS_PROJECT,"...");
    mitems[5] = new_item(M_RESET_PROJECT,"...");
    mitems[6] = NULL;
    set_menu_items(menu, mitems);
    resize(menu->nitems+2, menu->width+3); //изменяем размер под кол-во эл-тов
    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
    setbackground(A_BOLD);
    box(win,0,0); //рамка
    set_menu_mark(menu, " ");
    menu_opts_off(menu,O_SHOWMATCH);
    set_menu_win(menu, win);
    set_menu_format(menu, menu->nitems, 1);
    post_menu(menu);
}


bool ProjectsSubMenu::action()
{
    bool result = false;
    if (!items.empty()) //если уже открыто выходим
	return false;
    //формируем код операции для подменю
    char op = '?'; //код операции передаваемый в меню списка проектов
    if ( strcmp(item_name(current_item(menu)),M_SUSPEND_PROJECT) == 0 )
	op = 'S';
    if ( strcmp(item_name(current_item(menu)),M_RESUME_PROJECT) == 0 )
	op = 'R';
    if ( strcmp(item_name(current_item(menu)),M_UPDATE_PROJECT) == 0 )
	op = 'U';
    if ( strcmp(item_name(current_item(menu)),M_NO_NEW_TASKS_PROJECT) == 0 )
	op = 'N';
    if ( strcmp(item_name(current_item(menu)),M_ALLOW_NEW_TASKS_PROJECT) == 0 )
	op = 'A';
    if ( strcmp(item_name(current_item(menu)),M_RESET_PROJECT) == 0 )
	op = 'r';
    //создаем подменю
    int begincol = /*getbegcol()+*/getwidth() - 2; //смещение на экране по горизонтали
    int beginrow = 2 + item_index(current_item(menu)); //смещение на экране по вертикали
    insert(new ProjectListSubMenu(NRect(5,25,beginrow, begincol), srv, op));
    return true;
}


void ProjectsSubMenu::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( ev->done )
	return;
    NMenu::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case 27:
		if ( !items.empty() )
		    destroysubmenu();
		else
		    ev->done = false; //пусть обрабатывает владелец
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}


//=============================================================================================


TasksSubMenu::TasksSubMenu(NRect rect) : NMenu(rect)
{
    unpost_menu(menu);
    mitems = (ITEM**)realloc(mitems,4*sizeof(ITEM*));
    mitems[0] = new_item(M_SUSPEND_TASK,"  S");
    mitems[1] = new_item(M_RESUME_TASK ,"  R");
    mitems[2] = new_item(M_ABORT_TASK  ,"");
    mitems[3] = NULL;
    set_menu_items(menu, mitems);
    resize(menu->nitems+2, menu->width+3); //изменяем размер под кол-во эл-тов
    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
    setbackground(A_BOLD);
    box(win,0,0); //рамка
    set_menu_mark(menu, " ");
    menu_opts_off(menu,O_SHOWMATCH);
    set_menu_win(menu, win);
    set_menu_format(menu, menu->nitems, 1);
    post_menu(menu);
}


bool TasksSubMenu::action()
{
    putevent(new NEvent(NEvent::evKB, KEY_F(9))); //закрыть осн меню
    if ( strcmp(item_name(current_item(menu)),M_SUSPEND_TASK) == 0 )
	putevent(new NEvent(NEvent::evKB, 'S')); //создаем событие иммитирующее нажатие 'S'
    if ( strcmp(item_name(current_item(menu)),M_RESUME_TASK) == 0 )
	putevent(new NEvent(NEvent::evKB, 'R')); //создаем событие иммитирующее нажатие 'R'
    if ( strcmp(item_name(current_item(menu)),M_ABORT_TASK) == 0 )
	putevent(new NEvent(NEvent::evPROG, 2)); //создаем событие с кодом 2 "abort_result"
    return true;
}


//=============================================================================================


ActivitySubMenu::ActivitySubMenu(NRect rect, Srv* srv) : NMenu(rect)
{
    unpost_menu(menu);
    this->srv = srv;
    if (srv != NULL)
	srv->updateccstatus(); //обновить данные с boinc-а
    if ((srv != NULL)&&(srv->ccstatusdom != NULL))
    {
	Item* task_mode = srv->ccstatusdom->findItem("task_mode");
	mitems = (ITEM**)realloc(mitems,4*sizeof(ITEM*));
	mitems[0] = new_item(M_ACTIVITY_ALWAYS, ((task_mode!=NULL)&&(task_mode->getivalue() == 1)) ? "*" : ""); //1 always
	mitems[1] = new_item(M_ACTIVITY_AUTO,((task_mode!=NULL)&&(task_mode->getivalue() == 2)) ? "*" : ""); 	//2 pref
	mitems[2] = new_item(M_ACTIVITY_NEVER,((task_mode!=NULL)&&(task_mode->getivalue() == 3)) ? "*" : ""); 	//3 never
	mitems[3] = NULL;
	set_menu_items(menu, mitems);
    }
    resize(menu->nitems+2, menu->width+3); //изменяем размер под кол-во эл-тов
    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
    setbackground(A_BOLD);
    box(win,0,0); //рамка
    set_menu_mark(menu, " ");
    menu_opts_off(menu,O_SHOWMATCH);
    set_menu_win(menu, win);
    set_menu_format(menu, menu->nitems, 1);
    post_menu(menu);
}


bool ActivitySubMenu::action()
{
    putevent(new NEvent(NEvent::evKB, KEY_F(9))); //закрыть все меню
    if (srv != NULL)
    {
	if ( strcmp(item_name(current_item(menu)),M_ACTIVITY_ALWAYS) == 0 )
	    srv->opactivity("always");
	if ( strcmp(item_name(current_item(menu)),M_ACTIVITY_AUTO) == 0 )
	    srv->opactivity("auto");
	if ( strcmp(item_name(current_item(menu)),M_ACTIVITY_NEVER) == 0 )
	    srv->opactivity("never");
    }
    return true;
}


//=============================================================================================


ProjectListSubMenu::ProjectListSubMenu(NRect rect, Srv* srv, char op) : NMenu(rect)
{
    unpost_menu(menu);
    this->srv = srv;
    this->op = op;
    if ((srv != NULL)&&(srv->statedom != NULL))
    {
	srv->updatestate();
	Item* client_state = srv->statedom->findItem("client_state");
	if (client_state != NULL)
	{
	    std::vector<Item*> projects = client_state->getItems("project");
	    mitems = (ITEM**)realloc(mitems,(projects.size()+1)*sizeof(ITEM*));
	    for (int i = 0; i < projects.size(); i++)
	    {
		Item* project_name = projects[i]->findItem("project_name");
		if (project_name != NULL)
		{
		    projectnames.push_back(strdup(project_name->getsvalue())); //сохраняем копии названий проектов
		    std::string status = ""; //строка состояния
		    if (projects[i]->findItem("suspended_via_gui") != NULL)
			status = status + "[suspended via gui]";
		    if (projects[i]->findItem("dont_request_more_work") != NULL)
			status = status + "[dont request more work]";
		    projectstatus.push_back(strdup(status.c_str()));
		    mitems[i] = new_item(projectnames.back(),projectstatus.back());
		}
	    }
	    mitems[projects.size()] = NULL;
	}
    }
    set_menu_items(menu, mitems);
    resize(menu->nitems+2, menu->width+3); //изменяем размер под кол-во эл-тов
    set_menu_sub(menu,derwin(win,getheight()-2,getwidth()-2,1,1));
    setbackground(A_BOLD);
    box(win,0,0); //рамка
    set_menu_mark(menu, " ");
    menu_opts_off(menu,O_SHOWMATCH);
    set_menu_win(menu, win);
    set_menu_format(menu, menu->nitems, 1);
    post_menu(menu);
}


ProjectListSubMenu::~ProjectListSubMenu()
{
    std::list<char*>::iterator it;
    for (it = projectnames.begin(); it != projectnames.end(); it++)
    {
	delete (*it);
    }
    for (it = projectstatus.begin(); it != projectstatus.end(); it++)
    {
	delete (*it);
    }
}


bool ProjectListSubMenu::action()
{
    if (srv != NULL)
    {
	switch(op)
	{
	    case 'S': //Suspend project
		srv->opproject(item_name(current_item(menu)),"project_suspend");
		break;
	    case 'R': //Resume project
		srv->opproject(item_name(current_item(menu)),"project_resume");
		break;
	    case 'U': //Update project
		srv->opproject(item_name(current_item(menu)),"project_update");
		break;
	    case 'r': //Reset project
		srv->opproject(item_name(current_item(menu)),"project_reset");
		break;
	    case 'N': //No New Task project
		srv->opproject(item_name(current_item(menu)),"project_nomorework");
		break;
	    case 'A': //Allow More Work project
		srv->opproject(item_name(current_item(menu)),"project_allowmorework");
		break;
	    default:
		break;
	}
    }
    //создаем событие закрывающее меню
    putevent(new NEvent(NEvent::evKB, KEY_F(9)));
}


void ProjectListSubMenu::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( ev->done )
	return;
    NMenu::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_RIGHT: //блокируем стрелку вправо
		break;
	    case KEY_LEFT:
		putevent(new NEvent(NEvent::evKB, 27)); //закрыть это подменю
		break;
	    default:
		ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}
