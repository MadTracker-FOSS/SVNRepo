//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <time.h>
#include "MTLocalFile.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
MTLocalHook localhook;
int allocalign = 0;
char rootn[64] = {"Desktop"};
//---------------------------------------------------------------------------
MTLocalHook::MTLocalHook()
{
}

MTFile* MTLocalHook::fileopen(char *url,int flags)
{
	return new MTLocalFile(url,flags);
}

MTFolder* MTLocalHook::folderopen(char *url)
{
	return new MTLocalFolder(url);
}

bool MTLocalHook::filecopy(char *source,char *dest)
{
	return (CopyFile(source,dest,false)==TRUE);
}

bool MTLocalHook::filerename(char *source,char *dest)
{
	return (MoveFile(source,dest)==TRUE);
}

bool MTLocalHook::filedelete(char *url)
{
	return (DeleteFile(url)==TRUE);
}

void MTLocalHook::filetype(char *url,char *type,int length)
{
	char *e;
	
	e = (char*)strrchr(url,'.');
	if ((e) && ((strchr(e,'\\')) || (strchr(e,'/')))) e = 0;
	if (e){
		strncpy(type,e,length);
		strlwr(type);
	};
}
//---------------------------------------------------------------------------
MTLocalFile::MTLocalFile(char *path,int access):
maccess(access),
maph(0),
cpos(0),
from(0),
to(0x7FFFFFFF),
mmap(0),
stdhandle(true)
{
	int waccess = 0;
	int wshare = 0;
	int wcreate = 0;
	int wattr = 0;
	char *e;

	mtsetlasterror(0);
	e = strstr(path,"://");
	if (e) path = e+3;
	e = path;
	if (strcmp(path,"stdin")==0){
		h = GetStdHandle(STD_INPUT_HANDLE);
	}
	else if (strcmp(path,"stdout")==0){
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	else if (strcmp(path,"stderr")==0){
		h = GetStdHandle(STD_ERROR_HANDLE);
	}
	else{
		stdhandle = false;
		if (access & MTF_READ) waccess |= GENERIC_READ;
		if (access & MTF_WRITE) waccess |= GENERIC_WRITE;
		if (access & MTF_SHAREREAD) wshare |= FILE_SHARE_READ;
		if (access & MTF_SHAREWRITE) wshare |= FILE_SHARE_WRITE;
		if (access & MTF_CREATE) wcreate |= OPEN_ALWAYS;
		else wcreate |= OPEN_EXISTING;
		if (access & MTF_TEMP) wattr |= FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE;
		h = CreateFile(e,waccess,wshare,0,wcreate,wattr,0);
	};
	if (h==INVALID_HANDLE_VALUE){
		switch (GetLastError()){
		case 2:
		case 3:
		case 15:	// File not found
			mtsetlasterror(1);
			break;
		case 5:
		case 19:
		case 32:	// Access denied
			mtsetlasterror(3);
			break;
		default:
			mtsetlasterror(-1);
			break;
		};
	};
}

MTLocalFile::MTLocalFile(MTFile *parent,int start,int end,int access):
maccess(access),
maph(0),
cpos(start),
from(start),
to(start+end),
mmap(0),
stdhandle(((MTLocalFile*)parent)->stdhandle)
{
	int waccess = 0;
	HANDLE p = GetCurrentProcess();

	mtsetlasterror(0);
	if (access & MTF_READ) waccess |= GENERIC_READ;
	if (access & MTF_WRITE) waccess |= GENERIC_WRITE;
	if (!DuplicateHandle(p,((MTLocalFile*)parent)->h,p,&h,waccess,true,0)){
		switch (GetLastError()){
		case 2:
		case 3:
		case 15:	// File not found
			mtsetlasterror(1);
			break;
		case 5:
		case 19:	// Access denied
			mtsetlasterror(3);
			break;
		default:
			mtsetlasterror(-1);
			break;
		};
		FLOGD1("%s - [System] WARNING: Cannot subclass file! (Error %d)"NL,mtgetlasterror());
	};
	SetFilePointer(h,start,0,FILE_BEGIN);
}

MTLocalFile::~MTLocalFile()
{
	if (maph) CloseHandle(maph);
	if (!stdhandle) CloseHandle(h);
}

int MTLocalFile::read(void *buffer,int size)
{
	int read = 0;
	
	if (cpos+size>to) size = to-cpos;
	if (size<=0) return 0;
	ReadFile(h,buffer,size,(DWORD*)&read,0);
	cpos += read;
	return read;
}

int MTLocalFile::readln(char *buffer,int maxsize)
{
	int read = 0;
	int x,y,i,r,r2;
	char *e;
	
	if (cpos+maxsize>to) maxsize = to-cpos;
	if (maxsize<=0) return 0;
	x = maxsize;
	while (x>0){
		i = x;
		if (i>32) i = 32;
		ReadFile(h,buffer,i,(DWORD*)&r,0);
		if (buffer[i-1]=='\r'){
			e = strchr(buffer,'\r');
			if ((e==&buffer[i-1]) && (i<x)){
				ReadFile(h,&buffer[i++],1,(DWORD*)&r2,0);
				r += r2;
			};
		};
		if (r){
			for (y=0;y<r;y++){
				char c = *buffer++;
				if ((c==0) || (c=='\n') || (c=='\r')){
					cpos++;
					if ((y<r-1) && (c=='\r') && (*buffer=='\n')){
						y++;
						cpos++;
					};
					mtmemzero(buffer-1,maxsize-read);
					SetFilePointer(h,y-r+1,0,FILE_CURRENT);
					goto done;
				};
				read++;
			};
			if (r<i) break;
		}
		else break;
		x -= r;
	};
done:
	cpos += read;
	return read;
}

int MTLocalFile::reads(char *buffer,int maxsize)
{
	int read = 0;
	int x,y,i,r;
	
	if (cpos+maxsize>to) maxsize = to-cpos;
	if (maxsize<=0) return 0;
	x = maxsize;
	while (x>0){
		i = x;
		if (i>32) i = 32;
		ReadFile(h,buffer,i,(DWORD*)&r,0);
		if (r){
			for (y=0;y<r;y++){
				char c = *buffer++;
				if (c==0){
					cpos++;
					mtmemzero(buffer-1,maxsize-read);
					SetFilePointer(h,y-r+1,0,FILE_CURRENT);
					goto done;
				};
				read++;
			};
			if (r<i) break;
		}
		else break;
		x -= r;
	};
done:
	cpos += read;
	return read;
}

int MTLocalFile::write(const void *buffer,int size)
{
	int written = 0;
	
	if (cpos+size>to) size = to-cpos;
	if (size<=0) return 0;
	WriteFile(h,buffer,size,(DWORD*)&written,0);
	cpos += written;
	return written;
}

int MTLocalFile::seek(int pos,int origin)
{
	if (origin==MTF_BEGIN) pos += from;
	if ((to<0x7FFFFFFF) && (origin==MTF_END)){
		origin = MTF_BEGIN;
		pos = to-pos;
	};
	cpos = SetFilePointer(h,pos,0,origin);
	return cpos-from;
}

int MTLocalFile::length()
{
	if (to<0x7FFFFFFF) return to-from;
	return GetFileSize(h,0);
}

void *MTLocalFile::getpointer(int offset,int size)
{
	int waccess = 0;
	int o;

	if ((!h) || (mmap)) return 0;
	if (offset<0) offset = cpos;
	else offset += from;
	if (size<0) size = GetFileSize(h,0)-offset;
	if (maccess & MTF_WRITE) waccess = PAGE_READWRITE;
	else if (maccess & MTF_READ) waccess = PAGE_READONLY;
	if (!maph) maph = CreateFileMapping(h,0,waccess,0,0,0);
	if (!maph) return 0;
	if (maccess & MTF_WRITE) waccess = FILE_MAP_WRITE;
	else if (maccess & MTF_READ) waccess = FILE_MAP_READ;
	if (allocalign==0) allocalign = sysinfo.dwAllocationGranularity;
	o = (offset/allocalign)*allocalign;
	o = offset-o;
	mmap = (char*)MapViewOfFile(maph,waccess,0,offset-o,size+o)+o;
	return mmap;
}

void MTLocalFile::releasepointer(void *mem)
{
	if ((maph) && (mmap) && (mmap==mem)){
		UnmapViewOfFile(mmap);
		mmap = 0;
	};
}

int MTLocalFile::pos()
{
	return cpos-from;
}

bool MTLocalFile::eof()
{
	if (cpos>=to) return true;
	return (cpos>=(int)GetFileSize(h,0));
}

bool MTLocalFile::seteof()
{
	if (mmap) return false;
	if (maph){
		CloseHandle(maph);
		maph = 0;
	};
	return (SetEndOfFile(h)!=0);
}

bool MTLocalFile::gettime(int *create,int *access,int *write)
{
	FILETIME fc,fa,fw;
	FILETIME *pfc,*pfa,*pfw;
	SYSTEMTIME stime;
	struct tm utime;
	
	pfc = pfa = pfw = 0;
	if (create) pfc = &fc;
	if (access) pfa = &fa;
	if (write) pfw = &fw;
	if (GetFileTime(h,pfc,pfa,pfw)==0) return false;
	stime.wMilliseconds = 0;
	if (create){
		FileTimeToSystemTime(pfc,&stime);
		utime.tm_year = stime.wYear-1900;
		utime.tm_mon = stime.wMonth-1;
		utime.tm_mday = stime.wDay+1;
		utime.tm_hour = stime.wHour;
		utime.tm_min = stime.wMinute;
		utime.tm_sec = stime.wSecond;
		*create = mktime(&utime);
	};
	if (access){
		FileTimeToSystemTime(pfa,&stime);
		utime.tm_year = stime.wYear-1900;
		utime.tm_mon = stime.wMonth-1;
		utime.tm_mday = stime.wDay+1;
		utime.tm_hour = stime.wHour;
		utime.tm_min = stime.wMinute;
		utime.tm_sec = stime.wSecond;
		*access = mktime(&utime);
	};
	if (write){
		FileTimeToSystemTime(pfw,&stime);
		utime.tm_year = stime.wYear-1900;
		utime.tm_mon = stime.wMonth-1;
		utime.tm_mday = stime.wDay+1;
		utime.tm_hour = stime.wHour;
		utime.tm_min = stime.wMinute;
		utime.tm_sec = stime.wSecond;
		*write = mktime(&utime);
	};
	return true;
}

bool MTLocalFile::settime(int *create,int *access,int *write)
{
	FILETIME fc,fa,fw;
	FILETIME *pfc,*pfa,*pfw;
	SYSTEMTIME stime;
	struct tm *utime;
	
	pfc = pfa = pfw = 0;
	stime.wMilliseconds = 0;
	if (create){
		pfc = &fc;
		utime = gmtime((time_t*)create);
		stime.wYear = utime->tm_year+1900;
		stime.wMonth = utime->tm_mon+1;
		stime.wDay = utime->tm_mday-1;
		stime.wHour = utime->tm_hour;
		stime.wMinute = utime->tm_min;
		stime.wSecond = utime->tm_sec;
		SystemTimeToFileTime(&stime,pfc);
	};
	if (access){
		pfa = &fa;
		utime = gmtime((time_t*)access);
		stime.wYear = utime->tm_year+1900;
		stime.wMonth = utime->tm_mon+1;
		stime.wDay = utime->tm_mday-1;
		stime.wHour = utime->tm_hour;
		stime.wMinute = utime->tm_min;
		stime.wSecond = utime->tm_sec;
		SystemTimeToFileTime(&stime,pfa);
	};
	if (write){
		pfw = &fw;
		utime = gmtime((time_t*)write);
		stime.wYear = utime->tm_year+1900;
		stime.wMonth = utime->tm_mon+1;
		stime.wDay = utime->tm_mday-1;
		stime.wHour = utime->tm_hour;
		stime.wMinute = utime->tm_min;
		stime.wSecond = utime->tm_sec;
		SystemTimeToFileTime(&stime,pfw);
	};
	return SetFileTime(h,pfc,pfa,pfw)!=0;
}

MTFile* MTLocalFile::subclass(int start,int length,int access)
{
	if (access==-1) access = maccess;
	if (start<0) start = cpos;
	return new MTLocalFile(this,start,length,access);
}
//---------------------------------------------------------------------------
MTLocalFolder::MTLocalFolder(char *path):
search(INVALID_HANDLE_VALUE),
id(0)
{
	char *me;
	int em;
	
	strcpy(mpath,path);
	me = strchr(mpath,0);
	if (me>mpath){
		if (*(me-1)=='\\') *(me-1) = 0;
	};
	if (mpath[1]==':'){
		strcat(mpath,"\\*.*");
		em = SetErrorMode(SEM_FAILCRITICALERRORS);
		search = FindFirstFile(mpath,&data);
		SetErrorMode(em);
	};
}

MTLocalFolder::~MTLocalFolder()
{
	if (search!=INVALID_HANDLE_VALUE) FindClose(search);
}

bool MTLocalFolder::getfile(const char **name,int *attrib,double *size)
{
	char tmp[512];
	
	if (search==INVALID_HANDLE_VALUE){
		if (mpath[1]==':') return false;
		if (mpath[0]==0){
			if (name) *name = rootn;
			if (attrib) *attrib = MTFA_ROOT;
			if (size) *size = 0.0;
			return true;
		};
		if (strcmp(mpath,"Root")) return false;
		char root[4] = {"C:\\"};
		SHFILEINFO info;
		mtmemzero(&info,sizeof(info));
		root[0] = id+'A';
		if (GetDriveType(root)==DRIVE_CDROM){
			strcpy(data.cFileName,"CD-ROM (");
			root[2] = ')';
			strcat(data.cFileName,root);
		}
		else{
			SHGetFileInfo(root,0,&info,sizeof(info),SHGFI_DISPLAYNAME);
			strcpy(data.cFileName,info.szDisplayName);
		};
		if (name) *name = data.cFileName;
		if (attrib) *attrib = MTFA_DISK;
		if (size) *size = 0.0;
		return true;
	};
	while ((data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) || (data.cFileName[0]=='.')){
		if (!next()) return false;
	};
	strcpy(tmp,data.cFileName);
	strupr(tmp);
	if (strcmp(data.cFileName,tmp)==0){
		strlwr(data.cFileName);
		data.cFileName[0] = tmp[0];
	};
	if (name) *name = data.cFileName;
	if (attrib){
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) *attrib = MTFA_FOLDER;
		else *attrib = MTFA_FILE;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) *attrib |= MTFA_HIDDEN;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) *attrib |= MTFA_READONLY;
	};
	if (size){
		int64todouble((int*)&data.nFileSizeHigh,size);
	};
	return true;
}

bool MTLocalFolder::next()
{
	if (search!=INVALID_HANDLE_VALUE) return FindNextFile(search,&data)!=0;
	if (mpath[0]==0) return false;
	if (strcmp(mpath,"Root")==0){
		int ld = GetLogicalDrives();
		ld >>= id;
		do{
			id++;
			ld >>= 1;
		} while ((ld) && (!(ld & 1)));
		if (ld) return true;
	};
	return false;
}
//---------------------------------------------------------------------------
