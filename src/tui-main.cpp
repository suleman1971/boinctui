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

#include <locale.h>
#if HAVE_LIBNCURSESW == 1 && NCURSESW_HAVE_SUBDIR == 1
    #include <ncursesw/curses.h>
#else
    #include <curses.h>
#endif
#include <malloc.h>
#include "kclog.h"
#include "mainprog.h"

std::string locale;


void initcurses()
{
    locale = setlocale(LC_ALL, NULL);
    setlocale(LC_ALL, "");
    setlocale(LC_MESSAGES, "");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr,true); //разрешаем стрелки и т.п.
    #ifdef ENABLEMOUSE
    mousemask(ALL_MOUSE_EVENTS, NULL); // Report all mouse events
    #endif
    timeout(100); //ожидание для getch() 100 милисекунд
    use_default_colors();
    start_color();
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
