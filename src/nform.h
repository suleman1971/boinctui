#ifndef NFORM_H
#define NFORM_H

#include <string.h>
#include <stdlib.h>
#include <form.h>
#include "ngroup.h"


class NForm : public NGroup
{
  public:
    NForm(int lines, int rows);
    virtual ~NForm();
    //int eventloop();
    virtual void refresh();
    virtual void settitle(const char* title);
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
    virtual FIELD* addfield(FIELD* field);
    int 	getfieldcount() { return fieldcount; };
    virtual void delfields(); //удаляет все поля
  protected:
    char* title;	//заголовок
    FIELD**	fields;
    int		fieldcount; //число полей включая NULL
    FORM* frm; 		//форма ncurses
//    WINDOW* framewin; //исходное окно  (рамка)
};


#endif //NFORM_H