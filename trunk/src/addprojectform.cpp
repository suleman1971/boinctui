#include <ctype.h>
#include "addprojectform.h"
#include "mbstring.h"
#include "kclog.h"
#include "tuievent.h"


char* strupcase(char* s) //в верхний регистр
{
    char	*p;
    for (p = s; *p != '\0'; p++)
    *p = (char) toupper(*p);
    return s;
}


char* strlowcase(char* s) //в нижний регистр
{
    char	*p;
    for (p = s; *p != '\0'; p++)
    *p = (char) tolower(*p);
    return s;
}


AddProjectForm::AddProjectForm(int rows, int cols,  Srv* srv, const char* projname, bool userexist) : NForm(rows,cols)
{
    this->srv = srv;
    settitle(projname);
    this->projname = projname;
    this->userexist = userexist;
    Item* project = NULL;
    if (srv !=NULL)
	project = srv->findprojectbynamefromall(projname);
    int row = 1;
    //поля
    genfields(row,project);
    //пересчитываем высоту формы, чтобы влезли все поля и центрируем
    resize(row + 2,getwidth());
    move(getmaxy(stdscr)/2-getheight()/2,getmaxx(stdscr)/2-getwidth()/2);

    set_form_fields(frm, fields);
    set_current_field(frm, fields[0]); //фокус на поле

    post_form(frm);
    this->refresh();
}


void AddProjectForm::genfields(int& line, Item* project) //создаст массив полей
{
    FIELD* f;
    delfields();
    if (project != NULL)
    {
	//сообщение об ошибке
	errmsgfield = getfieldcount();
	f = addfield(new_field(1, getwidth()-2, line++, 1, 0, 0));
	set_field_buffer(f, 0, "Errr");
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_RED) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст
	field_opts_off(f, O_VISIBLE); //по умолчанию невидима
	//url
	Item* url = project->findItem("url");
	std::string s = "url          : ";
	if (url !=NULL)
	    projurl = url->getsvalue();
	s = s + projurl;
	f = addfield(new_field(1, getwidth()-4, line++, 2, 0, 0));
	set_field_buffer(f, 0, s.c_str());
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст

	//area
	Item* general_area = project->findItem("general_area");
	s = "General area : ";
	if (general_area !=NULL)
	    s = s + general_area->getsvalue();
	f = addfield(new_field(1, getwidth()-4, line++, 2, 0, 0));
	set_field_buffer(f, 0, s.c_str());
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст

	//specific area
	Item* specific_area = project->findItem("specific_area");
	s = "Specific area: ";
	if (specific_area !=NULL)
	    s = s + specific_area->getsvalue();
	f = addfield(new_field(1, getwidth()-4, line++, 2, 0, 0));
	set_field_buffer(f, 0, s.c_str());
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст

	//home
	s = "Home         : ";
	Item* home = project->findItem("home");
	if (home !=NULL)
	    s = s + home->getsvalue();
	f = addfield(new_field(1, getwidth()-4, line++, 2, 0, 0));
	set_field_buffer(f, 0, s.c_str());
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст

	//description
	s = "";
	line++;
	Item* description = project->findItem("description");
	if (description !=NULL)
	    s = s + description->getsvalue();
	f = addfield(new_field(3, getwidth()-4, line++, 2, 0, 0));
	set_field_buffer(f, 0, s.c_str());
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст
	line = line + 3;

	//platforms
	Item* platforms = project->findItem("platforms");
	if (platforms !=NULL)
	{
	    std::vector<Item*> namelist = platforms->getItems("name"); //список названий платформ
	    std::vector<Item*>::iterator it;
	    for (it = namelist.begin(); it!=namelist.end(); it++)
	    {
		Item* name = (*it)->findItem("name");
		//NStaticText* text6 = new NStaticText(NRect(1, getwidth()-4, row++, 2));
		s = name->getsvalue();
		f = addfield(new_field(1, getwidth()-4, line++, 2, 0, 0));
		set_field_buffer(f, 0, s.c_str());
		set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
		field_opts_off(f, O_ACTIVE); //статический текст
	    }
	}
    }
    //email
    line++;
    f = addfield(new_field(1, 10, line, 2 , 0, 0));
    set_field_buffer(f, 0, "email");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    emailfield = getfieldcount();
    f = addfield(new_field(1, 40, line++, 15, 0, 0));
    field_opts_off(f, O_AUTOSKIP);
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_CYAN) | A_BOLD);
    //password
    line++;
    f = addfield(new_field(1, 10, line, 2 , 0, 0));
    set_field_buffer(f, 0, "password");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    passwfield = getfieldcount();
    f = addfield(new_field(1, 40, line++, 15, 0, 0));
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_CYAN) | A_BOLD);
    field_opts_off(f, O_AUTOSKIP);
    if (!userexist)
    {
	//user name
	line++;
	f = addfield(new_field(1, 10, line, 2 , 0, 0));
	set_field_buffer(f, 0, "username");
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст
	usernamefield = getfieldcount();
	f = addfield(new_field(1, 40, line++, 15, 0, 0));
	field_opts_off(f, O_AUTOSKIP);
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_CYAN) | A_BOLD);
	//team name
	line++;
	f = addfield(new_field(1, 10, line, 2 , 0, 0));
	set_field_buffer(f, 0, "teamname");
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
	field_opts_off(f, O_ACTIVE); //статический текст
	teamfield = getfieldcount();
	f = addfield(new_field(1, 40, line++, 15, 0, 0));
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_CYAN) | A_BOLD);
	field_opts_off(f, O_AUTOSKIP);
    }
    //подсказки
    line++;
    f = addfield(new_field(1, getwidth()-25, line++, 20 , 0, 0));
    set_field_buffer(f, 0, "Enter-Ok    Esc-Cancel");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    //финализация списка полей
    addfield(NULL);
}


void AddProjectForm::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_ENTER:
	    case '\n': //ENTER
	    {
		form_driver(frm, REQ_NEXT_FIELD); //костыль чтобы текущее поле не потеряло значение
		char* email = strlowcase(rtrim(field_buffer(fields[emailfield],0)));
		char* passw = rtrim(field_buffer(fields[passwfield],0));
		kLogPrintf("AddProjectForm OK name=[%s] url=[%s] email=[%s]\n passw=[%s]\n", projname.c_str(), projurl.c_str(), email, passw);
		if (srv!=NULL)
		{
		    std::string errmsg;
		    bool success = true;
		    if (!userexist) //если аккаунта еще нет то создаем
		    {
			char* username = strlowcase(rtrim(field_buffer(fields[usernamefield],0)));
			char* teamname = rtrim(field_buffer(fields[teamfield],0));
			success = srv->createaccount(projurl.c_str(),email,passw, username, teamname, errmsg);
		    }
		    if (success)
			success = srv->projectattach(projurl.c_str(), projname.c_str(), email, passw, errmsg); //подключить проект
		    if (success)
			putevent(new TuiEvent(evADDPROJECT)); //создаем событие чтобы закрыть форму
		    else
		    {
			//СООБЩЕНИЕ ОБ ОШИБКЕ
			errmsg = " Error: " + errmsg;
			set_field_buffer(fields[errmsgfield], 0, errmsg.c_str());
			field_opts_on(fields[errmsgfield], O_VISIBLE); //делаем видимой строку ошибки
			this->refresh();
		    }
		}
		break;
	    }
	    case 27:
		putevent(new TuiEvent(evADDPROJECT)); //код закрытия окна
		break;
	    default:
		kLogPrintf("AddProjectForm::KEYCODE=%d\n", ev->keycode);
		ev->done = false;
		NForm::eventhandle(ev); //предок
		break;
	} //switch
    }
}
