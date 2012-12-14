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
    virtual void settitle(const char* title) { this->title = strdup(title); };
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    char* title;	//заголовок
//    FIELD *field[3];
    FORM* frm; 		//форма ncurses
//    WINDOW* framewin; //исходное окно  (рамка)
};


#endif //NFORM_H