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
#include<netdb.h>
#include<arpa/inet.h>
#if HAVE_LIBNCURSESW == 1 && NCURSESW_HAVE_SUBDIR == 1
    #include <ncursesw/curses.h>
#else
    #include <curses.h>
#endif
#include <malloc.h>
#include <getopt.h>
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


void printhelp()
{
    printf
    (
        "\n\nusage: \n"
        "  boinctui [--help] [--boinchost=<boinc_host_ip>:<boinc_icp_port>] [--pwd=<boinc_password>]\n\n"
        "    -b, --boinchost      ip address and port of boinc server host\n"
        "    -p, --pwd            boinc password\n"
        "    -h, --help           print this help and exit\n"
        "\nexamples: \n"
        "  boinctui --boinchost=192.168.1.123 --pwd=my_password -default port 31416 is used\n"
        "  boinctui --boinchost=192.168.1.123:31416             -connect with empty password\n"
        "  boinctui --boinchost=localhost                       -connect to localhost with defaul port and\n"
        "                                                        password from /var/lib/boinc-client/gui_rpc_auth.cfg\n"
        "\n"
    );
}


std::string hostname2ip(std::string& hostname)
{
    std::string result="";
	struct hostent *he;
	struct in_addr **addr_list;
	if ((he=gethostbyname(hostname.c_str()))==NULL) 
	{
		printf("cat't resolve gethostbyname");
		return result;
	}
	addr_list = (struct in_addr **) he->h_addr_list;
	for(int i=0; addr_list[i]!=NULL; i++) 
	{
        char ip[100];
		strcpy(ip,inet_ntoa(*addr_list[i]));
        if(i>0)
            result+=",";
        result+=ip;
	}
	return result;
}


bool parseargs(int argc, char ** argv)
{
	bool result=true;
	int c;
	while (1)
	{
		static struct option long_options[] =
			{
				{"help", no_argument, 0, 'h'},
				{"boinchost",  required_argument, 0, 'b'},
				{"pwd",  required_argument, 0, 'p'},
				{0, 0, 0, 0}
			};
		int option_index = 0;
		c = getopt_long (argc, argv, "hb:p:",long_options, &option_index);
		if (c == -1)
			break;
		switch (c)
		{
			case 0:
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'h':
				printhelp();
                result=false;
				break;
			case 'b':
            {
                gCfg->cmdlinehost=optarg;
                int pos=gCfg->cmdlinehost.find_last_of(":");
                if(pos!=std::string::npos)
                {
                    gCfg->cmdlineport=gCfg->cmdlinehost.substr(pos+1);
                    gCfg->cmdlinehost.resize(pos);
                }
                char ip[100];
                std::string hostip=hostname2ip(gCfg->cmdlinehost);
                gCfg->cmdlocalhost=(hostip.find("127.0.0.1")!=std::string::npos);
				printf ("boinc server '%s' %s %s\n"
                        , gCfg->cmdlinehost.c_str()
                        ,(gCfg->cmdlinehost!=hostip)?hostip.c_str():""
                        ,(gCfg->cmdlocalhost)?"(local host)":""
                       );
				printf ("boinc server TCP port '%s'\n", gCfg->cmdlineport.c_str());
				break;
            }
			case 'p':
                gCfg->cmdlinepwd=optarg;
				break;
			case '?':
                result=false;
				break;
			default:
				break;
		}
	}
	return result;
}


int main(int argc, char ** argv)
{
    MainProg* mainprog;
    kLogOpen("boinctui.log");
    #ifdef DEBUG
    struct mallinfo minf1 = mallinfo();
    #endif
    gCfg = new Config(".boinctui.cfg");
    if(parseargs(argc,argv))
    {
        initcurses();
        mainprog = new MainProg();
        mainprog->refresh();
        mainprog->mainloop(); //запускаем осн. цикл событий
        delete mainprog;
        donecurses();
    }
    #ifdef DEBUG
    struct mallinfo minf2 = mallinfo();
    kLogPrintf("mallinfo.uordblks= %d-%d = %d (bytes leak)\n",minf1.uordblks,minf2.uordblks, minf2.uordblks-minf1.uordblks);
    //malloc_stats();
    #endif
    kLogClose();
    exit(EXIT_SUCCESS);
}
