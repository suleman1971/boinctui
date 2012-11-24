// * для ведения логов

#ifndef KCLOG_H_
#define KCLOG_H_

#include <stdio.h>

/// открывает лог файл на запись
void kLogOpen(const char* filename = NULL);

/// закрывает лог файл
void kLogClose();

/// вывести информацию в лог 
void kLogPrintf(const char* fmt, ...);

#endif /*KCLOG_H_*/
