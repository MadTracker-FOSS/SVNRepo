//---------------------------------------------------------------------------
//	$Id$
//---------------------------------------------------------------------------
#ifndef MTLOCALFILE_INCLUDED
#define MTLOCALFILE_INCLUDED
//---------------------------------------------------------------------------
class MTLocalHook;
class MTLocalFile;
class MTLocalFolder;
//---------------------------------------------------------------------------
#include "MTFile.h"
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
//---------------------------------------------------------------------------
class MTLocalHook : public MTFileHook{
public:
	MTLocalHook();
	MTFile* MTCT fileopen(char *url,int flags);
	MTFolder* MTCT folderopen(char *url);
	bool MTCT filecopy(char *source,char *dest);
	bool MTCT filerename(char *source,char *dest);
	bool MTCT filedelete(char *url);
	void MTCT filetype(char *url,char *type,int length);
};

class MTLocalFile : public MTFile{
public:
	MTLocalFile(char *path,int access);
	MTLocalFile(MTFile *parent,int start,int end,int access);
	~MTLocalFile();
	int MTCT read(void *buffer,int size);
	int MTCT readln(char *buffer,int maxsize);
	int MTCT reads(char *buffer,int maxsize);
	int MTCT write(const void *buffer,int size);
	int MTCT seek(int pos,int origin);
	void* MTCT getpointer(int offset,int size);
	void MTCT releasepointer(void *mem);
	int MTCT length();
	int MTCT pos();
	bool MTCT eof();
	bool MTCT seteof();
	bool MTCT gettime(int *create,int *access,int *write);
	bool MTCT settime(int *create,int *access,int *write);
	MTFile* MTCT subclass(int start,int length,int access);
private:
	int maccess;
	HANDLE h,maph;
	int cpos;
	int from,to;
	void *mmap;
	bool stdhandle;
};

class MTLocalFolder : public MTFolder{
public:
	MTLocalFolder(char *url);
	~MTLocalFolder();
	bool MTCT getfile(const char **name,int *attrib,double *size);
	bool MTCT next();
private:
	char mpath[512];
	int id;
	HANDLE search;
	WIN32_FIND_DATA data;
};
//---------------------------------------------------------------------------
extern MTLocalHook localhook;
//---------------------------------------------------------------------------
#endif
