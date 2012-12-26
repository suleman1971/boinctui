#ifndef CFG_H
#define CFG_H

#include "resultdom.h"

class Config
{
  public:
    Config(const char* filename);
    ~Config();
    void  load();
    void  save();
    void  generatedefault();
    Item* getcfgptr() { if ( root!= NULL) return root->findItem("boinctui_cfg"); else return NULL; };
    void  addhost(const char* host, const char* port, const char* pwd);
  protected:
    char* fullname;	//полное имя файла
    Item* root;  	//корень дерева конфига
};


#endif //CFG_H