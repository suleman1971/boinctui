#ifndef RESULTDOM_H
#define RESULTDOM_H

#include <string>
#include <vector>
#include <list>
#include <stdlib.h>


class Item //элемент дерева данных (например <disk_usage>1129827635.000000</disk_usage>)
{
  public:
    Item(const char* name)		{ this->name = name; parent = NULL; };
    virtual ~Item();
    bool isnode() 			{ return !subitems.empty(); }; 	//true если имеет подэлементы
    void addsubitem(Item* item)		{ item->parent = this; subitems.push_back(item); }; 	//добавляет подэлемент
    void appendvalue(const char* svalue) { this->svalue = this->svalue + svalue; }; //дополняет строку значения
    void setsvalue(const char* svalue) { this->svalue = svalue; }; //устанавливает строку значения
    void setivalue(int ivalue); //присвоить целочисленное значение
    void mergetree(Item* tree); //объединяет tree с текущим эл-том
    void clearsubitems(); //удалить все подэл-ты текущего узла
    void delsubitem(Item* subitem); //удалить подэлемент subitem из узла
    const char* getsvalue()		{ return svalue.c_str(); }; 	//получить строку значения
    int		getivalue()		{ return atoi(svalue.c_str());};//получить целочисленное
    double	getdvalue();		//получить значение double
    const char* getname()		{ return name.c_str(); }; 	//получить имя эл-та
    std::string getfullname()		{ if (parent == NULL) return name; else return parent->getfullname()+"/"+name; }; //получить имя вместе со всеми владельщами
    Item* findItem(const char* name);					//возвращает подэлемент с именем name или NULL (поиск на всю глубину)
    std::vector<Item*> getItems(const char* name);			//получить из текущего эл-та список подэлементов с именем name (поиска в глубину НЕТ)
    std::string toxmlstring(); 						//сериализация в строку
    Item*	getparent() {return parent;};
  protected:
    std::string		name; 		//имя эл-та "disk_usage"
    std::string		svalue;		//строка значения "1129827635.000000"
    std::list<Item*> 	subitems; 	//список вложенных эл-тов (если они есть)
    Item*		parent;
};

#endif //RESULTDOM_H