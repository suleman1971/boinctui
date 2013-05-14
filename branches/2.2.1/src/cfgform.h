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