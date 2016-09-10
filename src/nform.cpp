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
#include "kclog.h"


NForm::NForm(int rows, int cols) : NGroup(NRect(rows,cols,0,0))
{
    fields = NULL;
    fieldcount = 0;
    modalflag = true;
    frm = new_form(NULL);
    scale_form(frm,&rows,&cols);
    addfield(NULL);
    set_form_win(frm, win);
    set_form_sub(frm, derwin(win, rows-1, cols-1, 0,0));
    wattrset(win,getcolorpair(COLOR_WHITE, COLOR_BLACK) | A_BOLD);
    title = NULL;
    needrefresh = true;
    //перемещаем в центр экрана
    //this->move(getmaxy(stdscr)/2-getheight()/2,getmaxx(stdscr)/2-getwidth()/2);
    curs_set(1); //курсор
}


NForm::~NForm()
{
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
    if (field == NULL)
    	set_form_fields(frm, fields);
    return field;
}


void NForm::delfields()
{
    if (fields != NULL)
    {
	set_form_fields(frm, NULL);
	int n = fieldcount - 1; //последний NULL
	for (int i = 0; i < n; i++)
	{
	    if (fields[i] != NULL)
	    {
		int retcode = free_field(fields[i]);
		if (retcode != E_OK)
		    kLogPrintf("NForm::delfields(): free_field(%p) retcode=%d\n", fields[i], retcode);
	    }
	}
	free(fields);
	fields = NULL;
	fieldcount = 0;
    }
}


void NForm::refresh()
{
    if(asciilinedraw == 1)
	wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    else
	box(frm->win, ACS_VLINE, ACS_HLINE);
    if(title != NULL)
    {
	mvwprintw(frm->win,0,getwidth()/2-mbstrlen(title)/2,"%s",title); //посередине
    }
pos_form_cursor(frm); //восстановить позицию курсора ( после mvprintw() )
    NGroup::refresh();

}


bool NForm::clickatfield(int mrow, int mcol, FIELD* f) //true если клик внутри этого поля
{
    bool result = true;
    int absbegrow = getabsbegrow();
    int absbegcol = getabsbegcol();
    kLogPrintf("frow=%d fcol=%d rows=%d cols=%d\n",f->frow,f->fcol,f->rows,f->cols);
    if ((mrow < absbegrow + f->frow)||(mcol < absbegcol + f->fcol))
	result = false;
    if ((mrow > absbegrow + f->frow + f->rows - 1)||(mcol > absbegcol + f->fcol + f->cols - 1))
	result = false;
    return result;
}


void NForm::eventhandle(NEvent* ev)	//обработчик событий
{
    NGroup::eventhandle(ev); //предок
    if ( ev->done )
	return;

    NMouseEvent* mevent = (NMouseEvent*)ev;
    //одиночный или двойной клик
    if ( ev->type == NEvent::evMOUSE )
	ev->done = true;
    if (( ev->type == NEvent::evMOUSE ) && (((mevent->cmdcode & BUTTON1_CLICKED))||((mevent->cmdcode & BUTTON1_DOUBLE_CLICKED))))
    {
	if (isinside(mevent->row, mevent->col))
	{
	    if (fields != NULL)
	    {
		int n = field_count(frm);
		for (int i = 0; i < n; i++)
		{
		    if (clickatfield(mevent->row, mevent->col, fields[i]))
		    {
			set_current_field(frm, fields[i]);
			form_driver(frm, REQ_END_LINE);
			break;
		    }
		}
	    }
	}
    }
    //клавиатура
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

