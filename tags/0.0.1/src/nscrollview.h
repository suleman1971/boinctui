#ifndef NSCROLLVIEW_H
#define NSCROLLVIEW_H


#include <vector>
#include "nview.h"
#include "ncolorstring.h"


class NScrollView : public NView //область со скроллингом
{
  public:
    NScrollView(NRect rect) : NView(rect) { startindex = 0; autoscroll = false; needrefresh = true;};
    ~NScrollView();
    void addstring(int attr, const char* fmt, ...);
    void addstring(NColorString* s) { content.push_back(s); needrefresh = true; }; //добавляет строку
    void clearcontent(); //очищает строковый буфер
    virtual void drawcontent(); //отрисовывает буфер строк
    void scrollto(int delta); //сдвинуть отображение на drlta строк вверх или вниз
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