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

#include "nform.h"
#include "mbstring.h"


NForm::NForm(int rows, int cols) : NGroup(NRect(rows,cols,0,0))
{
    fields = NULL;
    fieldcount = 0;
    frm = new_form(NULL);
    scale_form(frm,&rows,&cols);
    addfield(NULL);
    set_form_fields(frm, fields);
    set_form_win(frm, win);
    set_form_sub(frm, derwin(win, rows, cols, 2,2));
    wattrset(win,getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD);
//    framewin = win;
//    win = frm->sub;
//post_form(frm);
    title = NULL;
    needrefresh = true;
    //перемещаем в центр экрана
    this->move(getmaxy(stdscr)/2-getheight()/2,getmaxx(stdscr)/2-getwidth()/2);
    curs_set(1); //курсор
}


NForm::~NForm()
{
//    win = framewin; //востанавливаем для правильной деструкции
    unpost_form(frm);
    free_form(frm);
    delfields();
    if (title != NULL)
	free(title);
    curs_set(0); //курсор
}


void NForm::settitle(const char* title)
{
    this->title = (char*)malloc(strlen(title)+3);
    snprintf(this->title, strlen(title)+3," %s ",title);
}


FIELD* NForm::addfield(FIELD* field)
{
    fields = (FIELD**)realloc(fields, (fieldcount+1)*sizeof(FIELD*)); //выделяем память под массив полей
    fields[fieldcount] = field;
    fieldcount++;
    return field;
}


void NForm::delfields()
{
    if (fields != NULL)
    {
	int n = field_count(frm);
	for (int i = 0; i < n; i++)
	    free_field(fields[i]);
	free(fields);
	fields = NULL;
	fieldcount = 0;
	set_form_fields(frm, NULL);
    }
}


void NForm::refresh()
{
    box(frm->win, ACS_VLINE, ACS_HLINE);
    if(title != NULL)
    {
	mvwprintw(frm->win,0,getwidth()/2-mbstrlen(title)/2,"%s",title); //посередине
    }
pos_form_cursor(frm); //восстановить позицию курсора ( после mvprintw() )
    NGroup::refresh();

}


void NForm::eventhandle(NEvent* ev) 	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;
    if ( ev->type == NEvent::evKB )
    {
	ev->done = true;
        switch(ev->keycode)
	{
	    case '\t':
		form_driver(frm, REQ_NEXT_FIELD);
		form_driver(frm, REQ_END_LINE);
		break;
	    case KEY_BACKSPACE:
		form_driver(frm, REQ_DEL_PREV);
		break;
	    case KEY_LEFT:
		form_driver(frm, REQ_PREV_CHAR);
		break;
	    case KEY_RIGHT:
		form_driver(frm, REQ_NEXT_CHAR);
		break;
	    case KEY_DC:
		form_driver(frm, REQ_DEL_CHAR);
		break;
	    default:
		form_driver(frm,ev->keycode); //передаем в форму
		//ev->done = false; //нет реакции на этот код
	} //switch
	if (ev->done) //если обработали, то нужно перерисоваться
	    refresh();
    }
}

