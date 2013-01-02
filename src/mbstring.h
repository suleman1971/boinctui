#ifndef MBSTRING_H
#define MBSTRING_H

int mbstrlen(const char* s); //вернет размер строки utf8 в СИМВОЛАХ (не байтах) без учета '\0'

char* mbstrtrunc(char* s, int slen); //обрезать до длины slen символов (не байтов) без учета \0

char* rtrim(char* s); //удалить завершающие пробелы в строке

#endif // MBSTRING_H
