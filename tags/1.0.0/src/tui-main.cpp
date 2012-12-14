#include <locale.h>
#include <curses.h>
#include <malloc.h>
#include "kclog.h"
#include "mainprog.h"


void initcurses()
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr,true); //разрешаем стрелки и т.п.
    timeout(250); //ожидание для getch() 250 милисекунд
    start_color();
    initcolorpairs();
/*
    init_pair(1,COLOR_WHITE,COLOR_GREEN); //цвет для строки заголовка и статус строки
    init_pair(2,COLOR_YELLOW,COLOR_GREEN);
    init_pair(3,COLOR_YELLOW,COLOR_BLACK);
    init_pair(4,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(5,COLOR_CYAN,COLOR_BLACK);
    init_pair(6,COLOR_BLACK,COLOR_BLACK);
    init_pair(7,COLOR_RED,COLOR_BLACK);
    init_pair(8,COLOR_WHITE,COLOR_CYAN);
    init_pair(9,COLOR_WHITE,COLOR_RED);
*/
}


void donecurses()
{
    clear();
    refresh();
    endwin();
}


int main(int argc, char ** argv)
{
    MainProg* mainprog;
    kLogOpen("boinctui.log");
    initcurses();
    #ifdef DEBUG
    struct mallinfo minf1 = mallinfo();
    #endif
    mainprog = new MainProg();
    mainprog->refresh();
    mainprog->mainloop(); //запускаем осн. цикл событий
    delete mainprog;
    #ifdef DEBUG
    struct mallinfo minf2 = mallinfo();
    kLogPrintf("mallinfo.uordblks= %d-%d = %d (bytes leak)\n",minf1.uordblks,minf2.uordblks, minf2.uordblks-minf1.uordblks);
    //malloc_stats();
    #endif
    donecurses();
    kLogClose();
    exit(EXIT_SUCCESS);
}
