//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "MTFile.h"
#include "MTLocalFile.h"
#include "MTMemoryFile.h"
#include "MTHTTPFile.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
int tmpid = 0;
MTHash *hooks;
//---------------------------------------------------------------------------
void initFiles()
{
	hooks = new MTHash(4);
	hooks->additem("file",&localhook);
	hooks->additem("pipe",&localhook);
	hooks->additem("mem",&memoryhook);
	hooks->additem("http",&httphook);
//	hooks->additem("ftp",&ftphook);
	initHTTP();
}

void uninitFiles()
{
	uninitHTTP();
	delete hooks;
}
//---------------------------------------------------------------------------
MTFileHook* gethook(const char *url)
{
	MTFileHook *h = 0;
	char *e;
	char proto[16];

	if (!hooks) return 0;
	strncpy(proto,url,16);
	e = strchr(proto,':');
	if (e){
		*e = 0;
		h = (MTFileHook*)hooks->getitem(proto);
	};
	if (!h) return (MTFileHook*)hooks->getitem("file");
	return h;
}

MTFile *mtfileopen(char *url,int flags)
{
	MTFile *f;
	int error;

	FLOGD2("%s - [System] Opening file %s with access %X"NL,url,flags);
	MTFileHook *hook = gethook(url);
	if (!hook){
		LOG("    ERROR: Cannot handle that protocol!"NL);
		return 0;
	};
	f = hook->fileopen(url,flags);
	error = mtgetlasterror();
	if ((error) || (!f)){
		FLOG1("    WARNING: Cannot open file! (Error %d)"NL,error);
		if (f) delete f;
		return 0;
	};
	f->url = (char*)mtmemalloc(strlen(url)+1);
	strcpy(f->url,url);
	return f;
}

void mtfileclose(MTFile *file)
{
	if (!file) return;
	if (file->url){
		mtmemfree(file->url);
		file->url = 0;
	};
	delete file;
}

MTFolder *mtfolderopen(char *url)
{
	FLOGD1("%s - [System] Opening folder %s"NL,url);
	MTFileHook *hook = gethook(url);
	if (!hook){
		LOG("    ERROR: Cannot handle that protocol!"NL);
		return 0;
	};
	return hook->folderopen(url);
}

void mtfolderclose(MTFolder *folder)
{
	delete folder;
}

bool mtfileexists(char *url)
{
	MTFile *f;
	int error;

	MTFileHook *hook = gethook(url);
	if (!hook) return false;
	f = hook->fileopen(url,MTF_READ|MTF_SHARE);
	error = mtgetlasterror();
	if (f) delete f;
	if ((error) || (!f)) return false;
	return true;
}

bool mtfilecopy(char *source,char *destination)
{
	MTFileHook *s = gethook(source);
	MTFileHook *d = gethook(destination);
	MTFile *fs,*fd;
	int l;
	void *buf;
	
	if ((!s) || (!d)) return false;
	if (s==d) return s->filecopy(source,destination);
	mtsetlasterror(0);
	fs = s->fileopen(source,MTF_READ|MTF_SHARE);
	fd = d->fileopen(destination,MTF_WRITE|MTF_CREATE);
	if (mtgetlasterror()){
		delete fs;
		delete fd;
		return false;
	};
	buf = mtmemalloc(262144);
	while (true){
		l = fs->read(buf,262144);
		if (l<=0) break;
		fd->write(buf,l);
	};
	delete fs;
	delete fd;
	return true;
}

bool mtfiledelete(char *filename)
{
	MTFileHook *hook = gethook(filename);
	if (!hook) return false;
	return hook->filedelete(filename);
}

bool mtfilerename(char *filename,char *newname)
{
	MTFileHook *s = gethook(filename);
	MTFileHook *d = gethook(filename);
	if ((!s) || (!d) || (s!=d)) return false;
	return s->filerename(filename,newname);
}

void mtfiletype(char *filename,char *filetype,int length)
{
	MTFileHook *hook = gethook(filename);
	filetype[0] = 0;
	if (hook) hook->filetype(filename,filetype,length);
}

void mtfilemaketemp(char *filename,int length)
{
	int x,y;
	char *e;
	char ext[8];
	char path[512];
	
	GetTempPath(260,path);
	strcat(path,filename);
	x = y = '0';
	strcpy(ext,".tmp");
	while (mtfileexists(path)){
		e = strrchr(path,'.');
		if (!e) e = strchr(path,0);
		strcpy(e,ext);
		ext[3] = x;
		if (x++=='9') x = 'a';
		if (x=='p') x++;
		if (x>'z'){
			x = '0';
			ext[2] = y;
			if (y++=='9') y = 'a';
			if (y>'z') break;
		};
	};
	strncpy(filename,path,length);
}

MTFile *mttempfile(int access)
{
	MTFile *f;
	char *e;
	char path[512];
	
	GetTempPath(260,path);
	strcat(path,"mttmp");
	e = strchr(path,0);
	do{
		itoa(tmpid++,e,10);
		strcat(path,".tmp");
	} while (mtfileexists(path));
	f = mtfileopen(path,access|MTF_CREATE|MTF_TEMP);
	return f;
}
//---------------------------------------------------------------------------
