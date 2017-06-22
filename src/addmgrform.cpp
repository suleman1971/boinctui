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

#include <ctype.h>
#include "addmgrform.h"
#include "mbstring.h"
#include "kclog.h"
#include "tuievent.h"


char* strlowcase(char* s); //в нижний регистр


AddAccMgrForm::AddAccMgrForm(int rows, int cols,  Srv* srv, const char* mgrname) : NForm(rows,cols)
{
    this->srv = srv;
    settitle(mgrname);
    this->mgrname = mgrname;
    Item* account_manager = NULL;
    if (srv !=NULL)
	account_manager = srv->findaccountmanager(mgrname);
    //поля
    int row = 0;
    genfields(row,account_manager);
    //пересчитываем высоту формы, чтобы влезли все поля и центрируем
    int r,c =0;
    scale_form(frm, &r, &c);
    kLogPrintf("field_count=%d scale_form()->%d,%d\n", field_count(frm), r, c);
    resize(r+3,c+2);

    set_current_field(frm, fields[0]); //фокус на поле

    post_form(frm);
    this->refresh();
}


void AddAccMgrForm::genfields(int& line, Item* mgr) //создаст массив полей
{
    FIELD* f;
    delfields();
    //сообщение об ошибке
    errmsgfield = getfieldcount();
    f = addfield(new_field(1, getwidth()-2, line++, 0, 0, 0));
    set_field_buffer(f, 0, "Errr");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_RED) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    field_opts_off(f, O_VISIBLE); //по умолчанию невидима
    //получить url и имя менеджера (или из конфига или от boinc клиента
    if (mgr != NULL)
    {
        Item* url = mgr->findItem("url");
        if (url !=NULL)
	    mgrurl = url->getsvalue();
    }
    else
    {
        //взять url из конфига (если есть)
	Item* boinctui_cfg = gCfg->getcfgptr();
	if (boinctui_cfg != NULL)
	{
	    std::vector<Item*> mgrlist = boinctui_cfg->getItems("accmgr");
	    std::vector<Item*>::iterator it;
	    for (it = mgrlist.begin(); it != mgrlist.end(); it++)
	    {
		Item* name = (*it)->findItem("name");
		if (name != NULL)
		    if (name->getsvalue() == mgrname)
		    {
			Item* url = (*it)->findItem("url");
			if (url != NULL)
			{
			    mgrurl = url->getsvalue();
			    break;
			}
		    }
	    }
	}
    }
    //имя менеджера
    f = addfield(new_field(1, getwidth()-4, line, 2, 0, 0));
    set_field_buffer(f, 0, "Description  ");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    namefield = getfieldcount();
    f = addfield(new_field(1, 40, line++, 15, 0, 0));
    if (mgr != NULL)
    {
	field_opts_off(f, O_STATIC);
	field_opts_off(f, O_ACTIVE); //статический текст
    }
    else
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_CYAN) | A_BOLD);
    field_opts_off(f, O_AUTOSKIP);
    set_max_field(f,128); //max width 128
    char buf[129];
    strncpy(buf, gettitle(), 128);
    buf[128] = '\0';
    char* p;
    p = ltrim(buf);
    rtrim(buf);
    set_field_buffer(f, 0, p);
    //url
    line++;
    f = addfield(new_field(1, getwidth()-4, line, 2, 0, 0));
    set_field_buffer(f, 0, "URL          ");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    urlfield = getfieldcount();
    f = addfield(new_field(1, 40, line++, 15, 0, 0));
    if (mgr != NULL)
    {
	field_opts_off(f, O_STATIC);
	field_opts_off(f, O_ACTIVE); //статический текст
    }
    else
	set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_CYAN) | A_BOLD);
    field_opts_off(f, O_AUTOSKIP);
    set_max_field(f,128); //max width 128
    set_field_buffer(f, 0, mgrurl.c_str());
    //help
    line++;
    f = addfield(new_field(3, getwidth()-4, line++, 2, 0, 0));
    set_field_buffer(f, 0,  "If you have not yet registered with this account manager" \
    			"     please do so before proceeding.");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    line = line + 2;
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
    //подсказки
    line++;
    f = addfield(new_field(1, getwidth()-25, line++, 20 , 0, 0));
    set_field_buffer(f, 0, "Enter-Ok    Esc-Cancel");
    set_field_back(f, getcolorpair(COLOR_WHITE,COLOR_BLACK) | A_BOLD);
    field_opts_off(f, O_ACTIVE); //статический текст
    //финализация списка полей
    addfield(NULL);
}


void AddAccMgrForm::eventhandle(NEvent* ev) 	//обработчик событий
{
    if ( ev->done )
	return;
    NMouseEvent* mevent = (NMouseEvent*)ev;
    if ( ev->type == NEvent::evMOUSE)
    {
	NForm::eventhandle(ev); //предок
    }
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case KEY_ENTER:
	    case '\n': //ENTER
	    {
		form_driver(frm, REQ_NEXT_FIELD); //костыль чтобы текущее поле не потеряло значение
		char* username = rtrim(field_buffer(fields[usernamefield],0));
		char* passw = rtrim(field_buffer(fields[passwfield],0));
		mgrurl = rtrim(field_buffer(fields[urlfield],0));
		char* mgrname = rtrim(field_buffer(fields[namefield],0));
		kLogPrintf("AddAccMgrForm OK username=[%s] passw=[%s]\n", username, passw);
		if (srv!=NULL)
		{
		    std::string errmsg;
		    bool success = srv->accountmanager(mgrurl.c_str(), username, passw, false, errmsg);
		    if (success)
		    {
			Item* account_manager = NULL;
			if (srv !=NULL)
			    account_manager = srv->findaccountmanager(mgrname);
			if (account_manager == NULL) //для кастомных менеджеров сохраняем в конфигах
			{
			    //проверить есть-ли уже в конфиге такой аккаунт менеджер
			    //то обновляем существующую запись, иначе добавляем новую
			    bool exist = false;
			    Item* boinctui_cfg = gCfg->getcfgptr();
			    if (boinctui_cfg != NULL)
			    {
				std::vector<Item*> mgrlist = boinctui_cfg->getItems("accmgr");
				std::vector<Item*>::iterator it;
				for (it = mgrlist.begin(); it != mgrlist.end(); it++)
				{
				    Item* namecfg = (*it)->findItem("name");
				    Item* urlcfg =  (*it)->findItem("url");
				    if (namecfg != NULL)
					if (strcmp(namecfg->getsvalue(),mgrname) == 0)
					{
					    exist = true;
					    //обновить значение url в конфиге
					    Item* urlcfg = (*it)->findItem("url");
					    if (urlcfg != NULL)
						urlcfg->setsvalue(mgrurl.c_str());
					}
				    if (urlcfg != NULL)
				    {
					if (strcmp(urlcfg->getsvalue(),mgrurl.c_str()) == 0)
					{
					    exist = true;
					    //обновить значение имени в конфиге
					    Item* namecfg = (*it)->findItem("name");
					    if (namecfg != NULL)
						namecfg->setsvalue(mgrname);
					}
				    }
				    if (exist)
					break;
				}
				if (!exist)
				{
				    //записать в конфиг как новый
				    Item* accmgr  = new Item("accmgr");
				    boinctui_cfg->addsubitem(accmgr);
				    Item* name  = new Item("name");
				    name->setsvalue(mgrname);
				    Item* url  = new Item("url");
				    url->setsvalue(mgrurl.c_str());
				    accmgr->addsubitem(name);
				    accmgr->addsubitem(url);
				}
			    }
			}
			putevent(new TuiEvent(evADDACCMGR)); //создаем событие чтобы закрыть форму
		    }
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
		putevent(new TuiEvent(evADDACCMGR, srv, mgrname.c_str())); //код закрытия окна
		break;
	    default:
		kLogPrintf("AddAccMgrForm::KEYCODE=%d\n", ev->keycode);
		ev->done = false;
		NForm::eventhandle(ev); //предок
		break;
	} //switch
    }
}
