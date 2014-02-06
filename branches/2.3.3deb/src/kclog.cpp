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

// * для ведения логов

#ifdef LINUX
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include "kclog.h"


#define K_NO_THREAD_COLOR_LOG //отключаем раскраску тредов ибо в этом проекте нахрен не нужно


#ifdef DEBUG
FILE* kLog = NULL;
/// ANSI цвета
const char* colors[7] = {"\033[0;0;31m","\033[0;0;32m","\033[0;0;33m","\033[0;0;34m","\033[0;0;35m","\033[0;0;36m","\033[0;0;37m"};
#endif

/// открывает лог файл на запись
void kLogOpen(const char* filename)
{
	#ifdef DEBUG
	if (filename == NULL)
	{
		kLog = stdout;	
	}
	else
	{
		char* tmpfile = (char*) malloc(256);
		#ifdef WIN32
		sprintf(tmpfile,"%s/%s",getenv("TEMP"),filename); //для win32 используем юзерский temp
		//const char* tmpdir = getenv("TEMP"); 
		#else //для линукса используем фиксированный путь
		sprintf(tmpfile,"/tmp/%s",filename);
		#endif
		if ((kLog = fopen(tmpfile,"a")) == NULL)
		{
			//kLogPrintf("ERROR: can't create log file %s\n",tmpfile.c_str());
		}
		else
		{
			kLogPrintf("\nINFO: log opened success\n");
		}
		free(tmpfile);
	}
	#endif
}


/// закрывает лог файл
void kLogClose()
{
	#ifdef DEBUG
	kLogPrintf("\nINFO: log close\n");
	fclose(kLog);
	#endif
}


/// вывести информацию в лог заданным цветом
void kCLogPrintf(char* color, char* fmt, ...)
{
	#ifdef DEBUG
	if (kLog == NULL)
		kLogOpen();
	fprintf(kLog,"%s",color);
	va_list args;
	va_start(args, fmt);
	vfprintf(kLog, fmt, args);
	va_end(args);
	fprintf(kLog,"\033[0m");
	fflush(kLog);
	#endif	
}


/// вывести информацию в лог 
void kLogPrintf(const char* fmt, ...)
{
	#ifdef DEBUG
	if (kLog == NULL)
		kLogOpen();
	#ifndef K_NO_THREAD_COLOR_LOG
	fprintf(kLog,"%s",colors[pthread_self() % (sizeof(colors)/sizeof(colors[0]))]);
	#endif
	va_list args;
	va_start(args, fmt);
	vfprintf(kLog, fmt, args);
	va_end(args);
	#ifndef K_NO_THREAD_COLOR_LOG
	fprintf(kLog,"\033[0m");
	#endif
	fflush(kLog);
	#endif
}

