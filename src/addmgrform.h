#ifndef ADDMGRFORM_H
#define ADDMGRFORM_H

#include <string.h>
#include <stdlib.h>
#include "nform.h"
#include "nstatictext.h"
#include "srvdata.h"


class AddAccMgrForm : public NForm
{
  public:
    AddAccMgrForm(int lines, int rows, Srv* srv, const char* mgrname);
    void genfields(int& line, Item* project); //создаст массив полей
    virtual void eventhandle(NEvent* ev);	//обработчик событий
  protected:
    int		passwfield;
    int		errmsgfield;
    int		usernamefield;
    Srv*	srv;
    std::string mgrname;	//имя акк менеджера
    std::string mgrurl;		//url подключаемого менеджера
};


#endif //ADDMGRFORM_H