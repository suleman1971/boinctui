#ifndef CFGFORM_H
#define CFGFORM_H

#include <string.h>
#include <stdlib.h>
#include "nform.h"
#include "cfg.h"
#include "nstatictext.h"


class CfgForm : public NForm
{
  public:
    CfgForm(int lines, int rows, Config* cfg);
    void genfields(bool extfields); //создаст массив полей (extfields если нужно добавить хост)
    virtual void eventhandle(NEvent* ev); 	//обработчик событий
  protected:
    void	updatecfg(); //сохраняет данные из формы в cfg
    Config* 	cfg;
    bool	extfields; //true если поле для доп хоста видимо
    int		nhost; //макс номер хоста с 0го (включаа дополнительный если есть)
};


#endif //CFGFORM_H