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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "cfg.h"
#include "resultparse.h"
#include "kclog.h"
#include "commondef.h"


Config* gCfg;


Config::Config(const char* filename)
{
    root = NULL;
    //полный путь к конфигу
    if (filename == NULL)
	fullname = NULL;
    else
    {
	//хоум каталог
	const char* homepath = getenv("HOME");
	//полный путь
	fullname = (char*)malloc(strlen(homepath)+strlen(filename)+2);
	sprintf(fullname,"%s/%s",homepath,filename);
	kLogPrintf("FULLCFGPATH=%s\n",fullname); fflush(stdout);
    }
    //загружаем если файл уже есть или генерируем дефолтный
    load();
    asciilinedraw = getivalue("line_draw_mode");
    transparentbg = getivalue("transparent_background");
}


Config::~Config()
{
    save();
    if (root != NULL)
	delete root;
    if (fullname == NULL)
	free(fullname);
}


void Config::load()
{
    isdefault = true;
    if (fullname == NULL)
    {
	    generatedefault();
	    return;
    }

    struct stat st;
    int retcode = lstat(fullname, &st);
    if (retcode != 0) //файла нет?
    {
	    //делаем дефолтный
	    generatedefault();
	    return;
    }
    //читаем файл
    FILE* pfile;
    pfile = fopen(fullname,"r");
    if (pfile!=NULL)
    {
        kLogPrintf("SIZE=%ld\n",st.st_size);
	    char* buf = (char*)malloc(st.st_size + 1);
	    size_t n = fread (buf,1,st.st_size,pfile);
	    buf[n]=0;
	    kLogPrintf("%s\n",buf);
	    root = xmlparse(buf, st.st_size, errmsg);
        if (!errmsg.empty())
            errmsg = fullname + std::string("\n") +  errmsg;
	    fclose (pfile);
        isdefault = false;
    }
    //upgrade config version
    Item* boinctui_cfg=getcfgptr();
    if(boinctui_cfg!=NULL)
    {
        std::string vercfg="";
        std::string verprog=XSTR(VERSION);
        Item* v=boinctui_cfg->findItem("version");
        if(v!=NULL)
        {
            vercfg=v->getsvalue();
        }
        else
        {
            Item* version=new Item("version");
            version->setsvalue(XSTR(VERSION));
		    boinctui_cfg->addsubitem(version);
        }
        if(vercfg!=verprog)
            kLogPrintf("prog ver='%s' config ver='%s' need upgrade\n",verprog.c_str(),vercfg.c_str());
        if((vercfg=="")&&(verprog=="2.6.0"))
        {
            //column_view_mask
            Item* column_view_mask = boinctui_cfg->findItem("column_view_mask");
	        if (column_view_mask == NULL) //создать
	        {
		        column_view_mask = new Item("column_view_mask");
		        column_view_mask->setivalue(-1);
		        boinctui_cfg->addsubitem(column_view_mask);
	        }
            unsigned int columnmask = column_view_mask->getivalue();
	        unsigned int x1 = 0x001F & columnmask; //# state done% project est
            unsigned int x2 = 0x0060 & columnmask; //dl application
            unsigned int x3 = 0x0080 & columnmask; //task
            columnmask = x1 | (0x1 << 5) | (x2 << 1) | (0x1 << 8) | (x3 << 2);
	        column_view_mask->setivalue(columnmask);
            //tasks_sort_mode
            Item* tasks_sort_mode = boinctui_cfg->findItem("tasks_sort_mode");
	        if (tasks_sort_mode == NULL) //создать
	        {
		        tasks_sort_mode = new Item("tasks_sort_mode");
		        tasks_sort_mode->setivalue(-1);
		        tasks_sort_mode->addsubitem(tasks_sort_mode);
	        }
            unsigned int taskssortmode = tasks_sort_mode->getivalue();            
            switch (taskssortmode)
            {
                case 5:
                    taskssortmode=6;
                    break;
                case 7:
                    taskssortmode=9;
                    break;
            }
	        tasks_sort_mode->setivalue(taskssortmode);
        }
    }
}


void Config::save()
{
    if (!errmsg.empty())
        return; //если была ошибка при загрузке то файл не перезаписываем
    if (fullname == NULL)
	return;
    if (root == NULL)
	return;
    kLogPrintf("Save Config to FULLCFGPATH=%s\n",fullname);
    Item* cfg = root->findItem("boinctui_cfg");
    if (cfg == NULL)
	return;
    std::string configxml = cfg->toxmlstring();
    kLogPrintf("\n%s\n",cfg->toxmlstring().c_str());
    FILE* pfile = fopen(fullname,"w");
    if (pfile!=NULL)
    {
	fwrite(cfg->toxmlstring().c_str(),1, strlen(cfg->toxmlstring().c_str()),pfile);
	fclose (pfile);
    }
}


void Config::addhost(const char* shost, const char* sport, const char* spwd, const char* shostid)
{
    if ( (strlen(shost) == 0)||(strlen(sport) == 0) )
	return; //пустые не заносим

    Item* boinctui_cfg = getcfgptr();
    if (boinctui_cfg == NULL)
	return;

    Item* srv  = new Item("server");

    Item* host   = new Item("host");
    host->appendvalue(shost);
    srv->addsubitem(host);

    Item* port = new Item("port");
    port->appendvalue(sport);
    srv->addsubitem(port);

    if (strlen(spwd) > 0)
    {
	Item* pwd = new Item("pwd");
	pwd->appendvalue(spwd);
	srv->addsubitem(pwd);
    }
    Item* hostid = new Item("hostid");
    hostid->appendvalue(shostid);
    srv->addsubitem(hostid);

    boinctui_cfg->addsubitem(srv);
}


void Config::generatedefault()
{
    kLogPrintf("generatedafault()\n");
    root       = new Item(""); //корневой
    Item* cfg  = new Item("boinctui_cfg");
    root->addsubitem(cfg);
    addhost("127.0.0.1","31416","","Local Server");
}


int Config::getivalue(Item* node, const char* name) //ищет name начиная с node
{
    int result = 0;
    if (node != NULL)
    {
	Item* item = node->findItem(name);
	if (item != NULL)
	    result = item->getivalue();
    }
    return result;
}


void Config::setivalue(Item* node, const char* name, int value) //создаст в node подэл-т name со значением value
{
    Item* basenode = node;
    if (basenode == NULL)
	basenode = root; //если узел не указан используем корневой
    if (basenode == NULL)
	return; //ничего не делаем
    Item* item = basenode->findItem(name);
    //эл-та нет - нужно создать
    if (item == NULL)
    {
	item = new Item(name);
	basenode->addsubitem(item);
    }
    //устнавливаем значение
    item->setivalue(value);
}
