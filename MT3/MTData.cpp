//---------------------------------------------------------------------------
//
//	MadTracker Data
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifdef _WIN32
#	include <windows.h>
#	include <iostream.h>
#	include <shlobj.h>
#else
#	include <sys/stat.h>
#	include <stdlib.h>
#	include <unistd.h>
#endif
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include "MTData.h"
#include "MTXSystem2.h"
//---------------------------------------------------------------------------
MTPreferences prefs;
MTModule *module[16];
WaveOutput *output;
MTUser cuser = {0,0,"Unregistered User","","","","",""};
MTConfigFile *globalconf,*userconf;
MTFile *outmsg;
MTHash *confs;
bool wantreset = false;
bool exitasap = false;
//---------------------------------------------------------------------------
int newmodule()
//
//	Create a new module
//
{
	int x;
	
	ENTER("newmodule");
	for (x=0;x<16;x++){
		if (module[x]==0){
			module[x] = (MTModule*)oi->newobject(MTO_MODULE,0,x);
			LEAVE();
			return x;
		};
	};
	LEAVE();
	return -1;
}
//---------------------------------------------------------------------------
void startbackup(char *filename,bool save,int &time)
//
//	First part of a file backuping
//
{
	MTFile *f;
	char backup[512],backup2[512];
	
	FENTER3("startbackup(%s,%d,&%d)",filename,save,time);
	strcpy(backup,filename);
	strcat(backup,".bak");
	if (si->fileexists(filename)){
		f = si->fileopen(filename,MTF_READ);
		if (f==0){
			LEAVE();
			return;
		};
		f->gettime(&time,0);
		si->fileclose(f);
		if (si->fileexists(backup)){
			if (save){
				strcpy(backup2,backup);
				*strrchr(backup2,'k') = '~';
				si->filerename(backup,backup2);
			}
			else{
				si->filedelete(backup);
			};
		};
		si->filerename(filename,backup);
	};
	LEAVE();
}

void finishbackup(char *filename,bool revert,int &time)
//
//	Second part of a file backuping
//
{
	MTFile *f;
	char backup[512],backup2[512];
	
	FENTER3("finishbackup(%s,%d,&%d)",filename,revert,time);
	strcpy(backup,filename);
	strcat(backup,".bak");
	strcpy(backup2,backup);
	*strrchr(backup2,'k') = '~';
	if (revert){
		si->filedelete(filename);
		si->filerename(backup,filename);
		si->filerename(backup2,backup);
	}
	else{
		si->filedelete(backup2);
	};
	f = si->fileopen(filename,MTF_READ);
	if (f==0){
		LEAVE();
		return;
	};
	f->settime(&time,0);
	si->fileclose(f);
	LEAVE();
}

void initConsole()
{
	if (!si) return;
#	ifdef _WIN32
		outmsg = si->fileopen("mem://",MTF_READ|MTF_WRITE);
#	else
		outmsg = si->fileopen("stdout",MTF_READ|MTF_WRITE);
#	endif
}

void uninitConsole()
{
	if ((si) && (outmsg)) si->fileclose(outmsg);
}

bool processCommandline(const char *cmd)
{
	static char pcmd[1024];
	char *e,*e2,*p,*v;
	MTArray *pa;
	int x,y;
	bool showhelp = false;
	const char *usage1 = {
"MadTracker 3 - Copyright (C) 1998-2006 Yannick Delwiche."NL\
""NL\
"Usage: mt3.exe [OPTION]... [FILE]..."NL\
""NL\
"  -h, --help        Display this help and exit"NL};
	const char *usage2 = {NL"Visit http://www.madtracker.org for more information."NL};

	if (!si) return false;
	strncpy(pcmd,cmd,sizeof(pcmd));
	pcmd[sizeof(pcmd)-1] = 0;
	e = pcmd;
	pa = si->arraycreate(8,sizeof(MTCLParam));
	while ((e) && (*e)){
		while (((*e==' ') || (*e=='\t')) && (*e)) e++;
		p = e;
		if (*p=='-'){
			if (*++p=='-') p++;
			e = strchr(p,' ');
			v = strchr(p,'=');
			if ((v) && ((v<e) || (!e))){
				*v++ = 0;
				if (*v=='"'){
					*v++ = 0;
					e2 = strchr(v,'"');
					if (e2) e = e2;
				};
				if (e) *e++ = 0;
				MTCLParam np = {p,v};
				pa->push(&np);
			}
			else{
				if (e) *e++ = 0;
				MTCLParam np = {p,0};
				pa->push(&np);
			};
		}
		else{
			v = p;
			e = strchr(v,' ');
			if (e) *e++ = 0;
			MTCLParam np = {0,v};
			pa->push(&np);
		};
	};
	if (pa->nitems>0){
		MTCLParam *np;
		pa->reset();
		while ((np = (MTCLParam*)pa->next())){
			if (!np->name) continue;
			if ((strcmp(np->name,"h")==0) || (strcmp(np->name,"help")==0)){
				showhelp = true;
				break;
			};
		};
		if (showhelp){
			if (outmsg) outmsg->write(usage1,strlen(usage1));
		};
		if ((!showhelp) || (outmsg)){
			for (x=0;x<next;x++){
				MTExtension &cext = *ext[x];
				for (y=cext.i->ninterfaces-1;y>=0;y--){
					MTXInterface *ci = cext.i->interfaces[y];
					if (showhelp) ci->showusage(outmsg);
					else ci->processcmdline(pa);
				};
			};
		};
		if ((showhelp) && (outmsg)) outmsg->write(usage2,strlen(usage2));
	};
	si->arraydelete(pa);
	return showhelp;
}
//---------------------------------------------------------------------------
void MTCT ConfDelete(void *item,void *param)
{
	si->configclose(((_MTConf*)item)->conf);
}
//---------------------------------------------------------------------------
bool init()
//
//	Main initialization
//
{
	char *cmd,*e;
	char applpath[512],userpath[512];
	
	prefs.syspath[SP_ROOT] = (char*)malloc(512);
	prefs.syspath[SP_USER] = (char*)malloc(512);
	prefs.syspath[SP_CONFIG] = (char*)malloc(512);
	prefs.syspath[SP_USERCONFIG] = (char*)malloc(512);
	prefs.syspath[SP_EXTENSIONS] = (char*)malloc(512);
	prefs.syspath[SP_SKINS] = (char*)malloc(512);
	prefs.syspath[SP_INTERFACE] = (char*)malloc(512);
	prefs.syspath[SP_HELP] = (char*)malloc(512);

	prefs.path[UP_TEMP] = (char*)malloc(512);
	prefs.path[UP_CACHE] = (char*)malloc(512);

#	ifdef _WIN32
		bool quote = false;
		cmd = GetCommandLine();
		if (*cmd=='"'){
			cmd++;
			quote = true;
		};
		if ((cmd[0]!='/') && (cmd[0]!='\\') && (cmd[1]!=':')){
			e = applpath+GetCurrentDirectory(sizeof(applpath),applpath);
			*e++ = '\\';
			strcpy(e,cmd);
		}
		else{
			strcpy(applpath,cmd);
		};
		if (quote){
			cmd = strchr(applpath,'"');
			if (cmd) cmd++;
		}
		else{
			cmd = strchr(applpath,' ');
		};
		e = strchr(applpath,':');
		if (e!=strrchr(applpath,':')){
			e++;
			e = strchr(e,':');
		}
		else{
			e = strchr(applpath,0);
		};
		if ((cmd) && (e>=cmd)) e = cmd-1;
		while ((e>applpath) && (*e!='\\') && (*e!='/')) *e-- = 0;
		SHGetSpecialFolderPath(0,userpath,CSIDL_APPDATA,false);
		e = strchr(userpath,0);
		if (*(e-1)!='\\'){
			*e++ = '\\';
			*e = 0;
		};
		strcpy(e,"MadTracker/");
#	else
		extern const char *argv0;
		extern char *cmdline;
		const char *binpath = getenv("_");
		char *b1 = strrchr(binpath,'/')+1;
		int l;
		if (!b1) binpath = argv0;
		else if (!strstr(argv0,b1)) binpath = argv0;
		l = readlink(binpath,applpath,sizeof(applpath)-1);
		if (l==-1) strcpy(applpath,binpath);
		else applpath[l] = 0;
		e = strchr(applpath,0)-1;
		while (*e!='/') *e-- = 0;
		cmd = cmdline;
		strcpy(userpath,getenv("HOME"));
		e = strchr(userpath,0);
		if (*(e-1)!='/'){
			*e++ = '/';
			*e = 0;
		};
		strcpy(e,".MadTracker/");
#	endif
	strcpy(prefs.syspath[SP_ROOT],applpath);
	strcpy(prefs.syspath[SP_USER],userpath);
	strcpy(prefs.syspath[SP_CONFIG],applpath);
	strcpy(prefs.syspath[SP_USERCONFIG],userpath);
	strcpy(prefs.syspath[SP_EXTENSIONS],applpath);
	strcpy(prefs.syspath[SP_INTERFACE],applpath);
	strcpy(prefs.syspath[SP_SKINS],applpath);
	strcpy(prefs.syspath[SP_HELP],applpath);
	strcat(prefs.syspath[SP_CONFIG],"Conf/System/");
	strcat(prefs.syspath[SP_USERCONFIG],"Conf/");
/*
	strcat(prefs.syspath[SP_USERCONFIG],cuser.name);
	strcat(prefs.syspath[SP_USERCONFIG],"/");
*/
	strcat(prefs.syspath[SP_EXTENSIONS],"Extensions/");
	strcat(prefs.syspath[SP_INTERFACE],"Interface/");
	strcat(prefs.syspath[SP_SKINS],"Skins/");
	strcat(prefs.syspath[SP_HELP],"Help/");

	strcpy(prefs.path[UP_TEMP],applpath);
	strcpy(prefs.path[UP_CACHE],prefs.syspath[SP_USERCONFIG]);
	strcat(prefs.path[UP_TEMP],"Temp/");
	strcat(prefs.path[UP_CACHE],"Cache/");

#	ifdef _WIN32
		CreateDirectory(prefs.syspath[SP_USER],0);
		CreateDirectory(prefs.syspath[SP_USERCONFIG],0);
#	else
		mkdir(prefs.syspath[SP_USER],0700);
		mkdir(prefs.syspath[SP_USERCONFIG],0700);
#	endif

	loadExtensions();
	if (!initSystem()){
#		ifdef _WIN32
			MessageBox(0,"Cannot initialize the MTSystem extension!","System Error",MB_ICONEXCLAMATION|MB_OK);
#		else
			fputs("Cannot initialize the MTSystem extension!"NL,stderr);
#		endif
		return false;
	};
	confs = si->hashcreate(4);
	initConsole();
	if ((cmd) && (processCommandline(cmd))){
#		ifdef _WIN32
			exitasap = true;
#		else
			return false;
#		endif
	};

	LOGD("%s - Initializing..."NL);
	if (!initExtensions()){
		LOGD("%s - [MT3] ERROR: Failed to initialize!"NL);
		return false;
	};
	startExtensions();
	LOGD("%s - Initialized."NL);
	oi->deleteobject(oi->newobject(MTO_MODULE,0,0,0));
	return true;
}

void uninit()
//
//	Uninitialization
//
{
	int x;
	MTModule *del[16];
	
	if (!si) return;
	LOGD("%s - Uninitializing..."NL);
	mtmemzero(del,sizeof(del));
	uninitConsole();
	si->stop();
	stopExtensions();
	MTTRY
		if (output) output->lock->lock();
		for (x=0;x<16;x++){
			if (module[x]){
				del[x] = module[x];
				module[x] = 0;
			};
		};
	MTCATCH
	MTEND
	if (output) output->lock->unlock();
	MTTRY
		for (x=0;x<16;x++){
			if (del[x]){
				oi->deleteobject(del[x]);
			};
		};
	MTCATCH
	MTEND
	uninitExtensions();
	si->configclose(globalconf);
	si->configclose(userconf);
	if (confs){
		confs->clear(true,ConfDelete);
		si->hashdelete(confs);
		confs = 0;
	};
	globalconf = userconf = 0;
	uninitSystem();
	unloadExtensions();
	free(prefs.syspath[SP_ROOT]);
	free(prefs.syspath[SP_USER]);
	free(prefs.syspath[SP_CONFIG]);
	free(prefs.syspath[SP_USERCONFIG]);
	free(prefs.syspath[SP_EXTENSIONS]);
	free(prefs.syspath[SP_INTERFACE]);
	free(prefs.syspath[SP_SKINS]);
	free(prefs.syspath[SP_HELP]);
}
//---------------------------------------------------------------------------
