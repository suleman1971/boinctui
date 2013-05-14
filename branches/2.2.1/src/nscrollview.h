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

#ifndef NSCROLLVIEW_H
#define NSCROLLVIEW_H


#include <vector>
#include "nview.h"
#include "ncolorstring.h"


class NScrollView : public NView //область со скроллингом
{
  public:
    NScrollView(NRect rect) : NView(rect) { startindex = 0; autoscroll = false; needrefresh = true;};
    virtual ~NScrollView();
    void addstring(int attr, const char* fmt, ...);
    void addstring(NColorString* s) { content.push_back(s); needrefresh = true; }; //добавляет строку
    void clearcontent(); //очищает строковый буфер
    virtual void drawcontent(); //отрисовывает буфер строк
    void scrollto(int delta); //сдвинуть отображение на drlta строк вверх или вниз
    void setstartindex(int n) { startindex = n; }; //установить отображение со строки n
    bool getautoscroll() { return autoscroll; }; //true если включен режим автоскроллинга
    void setautoscroll(bool b); //true чтобы включить автоскроллинг
    virtual void resize(int rows, int cols);
    virtual void refresh();
  protected:
    std::vector<NColorString*> content; //массив строк с цветовыми аттрибутами
    int	startindex; //номер первой видимой строки
    bool autoscroll; //если true то будет прокручивать автоматом до конца
};

#endif //NSCROLLVIEW_H