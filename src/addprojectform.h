#ifndef ADDPROJECTFORM_H
#define ADDPROJECTFORM_H

#include <string.h>
#include <stdlib.h>
#include "nform.h"
#include "nstatictext.h"
#include "srvdata.h"


class AddProjectForm : public NForm
{
  public:
    AddProjectForm(int lines, int rows, Srv* srv, const char* projname, bool userexist);
    void genfields(int& line, Item* project); //создаст массив полей
    virtual void eventhandle(NEvent* ev);	//обработчик событий
  protected:
    int		emailfield;
    int		passwfield;
    int		errmsgfield;
    int		usernamefield;
    int		teamfield;
    Srv*	srv;
    std::string projname;	//имя подключаемого проекта
    std::string projurl;	//url подключаемого проекта
    bool	userexist;	//true если юзер уже создан
};


#endif //ADDPROJECTFORM_H