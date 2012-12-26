#include <locale.h>
#include <curses.h>
#include <malloc.h>
#include "kclog.h"
#include "mainprog.h"

std::string locale;


void initcurses()
{
    locale = setlocale(LC_ALL, NULL);
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr,true); //разрешаем стрелки и т.п.
    timeout(250); //ожидание для getch() 250 милисекунд
    start_color();
    initcolorpairs();
}


void donecurses()
{
    clear();
    refresh();
    endwin();
    setlocale(LC_ALL, locale.c_str());
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
