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
#include "nscrollbar.h"


class NScrollView : public NView //область со скроллингом
{
  public:
    NScrollView(NRect rect, NScrollBar* scrollbar = NULL) : NView(rect) { this->scrollbar=scrollbar; startindex = 0; autoscroll = false; needrefresh = true;};
    virtual ~NScrollView();
    void addstring(int attr, const char* fmt, ...);
    void addstring(NColorString* s) { content.push_back(s); needrefresh = true; }; //добавляет строку
    void eventhandle(NEvent* ev); //обработчик событий
    void clearcontent(); //очищает строковый буфер
    virtual void drawcontent(); //отрисовывает буфер строк
    void scrollto(int delta); //сдвинуть отображение на drlta строк вверх или вниз
    void setstartindex(int n); //установить отображение со строки n
    int  getstartindex() { return startindex; }; //получить номер первой видимой строки
    int  getmaxcontentwidth(); //вернет максимальную длину строки (в экранных символях) в content
    bool getautoscroll() { return autoscroll; }; //true если включен режим автоскроллинга
    void setautoscroll(bool b); //true чтобы включить автоскроллинг
    void setscrollbar(NScrollBar* scrollbar) { this->scrollbar = scrollbar; };
    virtual void resize(int rows, int cols);
    virtual void refresh();
    int getstringcount() { return content.size();}; //число строк
  protected:
    std::vector<NColorString*> content; //массив строк с цветовыми аттрибутами
    int	startindex; //номер первой видимой строки
    bool autoscroll; //если true то будет прокручивать автоматом до конца
    NScrollBar* scrollbar; //скроллбар для этого эл-та (NULL если нет)
};

#endif //NSCROLLVIEW_H