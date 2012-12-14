#ifndef NCOLORSTRING_H
#define NCOLORSTRING_H

#include <stdarg.h>
#include <string>
#include <list>
#include "mbstring.h"


class NColorStringPart
{
  public:
    NColorStringPart(int attr, const char* s) { this->s = s; this->attr = attr;};
    NColorStringPart(int attr, const char* fmt, va_list vl);
    int getlen() { return mbstrlen(s.c_str()); }; //вернет длинну в ЭКРАННЫХ СИМВОЛАХ
    std::string s;
    int	attr;
};


class NColorString
{
  public:
    NColorString(int attr, const char* fmt, ...);
    NColorString(int attr, const char* fmt, va_list vl);
    ~NColorString();
    void append(int attr, const char* fmt, ...);
    void vappend(int attr, const char* fmt, va_list vl);
    std::list<NColorStringPart*> parts;
    int getlen(); //вернет длинну в ЭКРАННЫХ СИМВОЛАХ
    void clear() { while(!parts.empty()) { delete parts.front(); parts.remove(parts.front());} }; //очищаем строку
  protected:
    void append(NColorStringPart* part) { parts.push_back(part); }; //добавить подстроку к строке
};

#endif //NCOLORSTRING_H