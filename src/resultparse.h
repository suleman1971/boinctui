#ifndef RESULTPARSE_H
#define RESULTPARSE_H

#include "resultdom.h"

Item* xmlparse(const char* xml, int len); //xml строка с xml len ее размер в байтах

char* stripinvalidtag(char* xml, int len);	//ГРЯЗНЫЙ ХАК нужен чтобы до парсинга удалить кривые теги
						//в сообщениях вида <a href=.. </a> иначе будет ошибка парсинга

#endif // RESULTPARSE_H