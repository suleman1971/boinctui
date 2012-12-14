#ifndef CFGFORM_H
#define CFGFORM_H

#include <string.h>
#include <stdlib.h>
#include <form.h>
#include "nform.h"
#include "cfg.h"
#include "nstatictext.h"


class CfgForm : public NForm
{
  public:
    CfgForm(int lines, int rows, Config* cfg);
    virtual ~CfgForm();
    void genfields(bool extfields); //создаст массив полей (extfields если нужно добавить хост)
    void delfields(); //удаляет все поля
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
//    virtual void refresh();
  protected:
    void	updatecfg(); //сохраняет данные из формы в cfg
    FIELD**  fields;
    Config* cfg;
    bool	extfields; //true если поле для доп хоста видимо
    int		nhost; //макс номер хоста с 0го (включаа дополнительный если есть)
//    NStaticText* text1;
};


#endif //CFGFORM_H