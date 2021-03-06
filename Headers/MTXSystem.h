//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTXSYSTEM_INCLUDED
#define MTXSYSTEM_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#ifdef _WIN32
#	include <windows.h>
#else
#	include <setjmp.h>
#endif
//---------------------------------------------------------------------------
static const int systemtype = FOURCC('X','S','Y','S');

#define MTS_WINNT    0x00001
#define MTS_MMX      0x00002
#define MTS_SSE      0x00004
#define MTS_SSE2     0x00008
#define MTS_SSE3     0x00010
#define MTS_CX8      0x00100
#define MTS_CMOV     0x00200
#define MTS_HT       0x01000
#define MTS_DEBUGGED 0x10000

#define MTR_WINDOW   FOURCC('M','T','W','N')
#define MTR_SKIN     FOURCC('M','T','S','K')
#define MTR_TEXT     FOURCC('M','T','T','X')
#define MTR_SHORTCUT FOURCC('M','T','S','C')
#define MTR_BITMAP   FOURCC('M','T','B','M')
#define MTR_HTML     FOURCC('M','T','H','T')
#define MTR_SAMPLE   FOURCC('M','T','S','P')

#define MTM_ZERO     1
#define MTM_PHYSICAL 2

#define MTT_IDLE     -15
#define MTT_LOW      -2
#define MTT_LOWER    -1
#define MTT_NORMAL   0
#define MTT_HIGHER   1
#define MTT_HIGH     2
#define MTT_REALTIME 15

enum MTProcessType{
	MTP_LOADMODULE = 0x10000,
	MTP_SAVEMODULE,
	MTP_LOADINSTRUMENT,
	MTP_SAVEINSTRUMENT,
	MTP_LOADSAMPLE,
	MTP_SAVESAMPLE,
	MTP_BUILDPEAKS = 0x20000,
	MTP_NETWORK = 0x30000,
	MTP_EXPORT = 0x40000,
	MTP_INTERNET = 0x50000
};

#define MTPF_CANCANCEL   1
#define MTPF_HASPROGRESS 2

#define MTPS_INIT     -1
#define MTPS_WAITING  0
#define MTPS_WORKING  1
#define MTPS_FINISHED 2

#define MTF_READ       0x01
#define MTF_WRITE      0x02
#define MTF_CREATE     0x04
#define MTF_SHAREREAD  0x08
#define MTF_SHAREWRITE 0x10
#define MTF_SHARE      0x18
#define MTF_TEMP       0x80

#define MTF_BEGIN      0
#define MTF_CURRENT    1
#define MTF_END        2
#define MTF_LINE       3

#define MTFA_TYPEMASK 7
#define MTFA_ROOT     1
#define MTFA_DISK     2
#define MTFA_FOLDER   3
#define MTFA_FILE     4
#define MTFA_HIDDEN   8
#define MTFA_READONLY 16

#define MTD_OK          (char*)0
#define MTD_OKCANCEL    (char*)1
#define MTD_YESNO       (char*)2
#define MTD_YESNOCANCEL (char*)3

#define MTD_INFORMATION 1
#define MTD_QUESTION    2
#define MTD_EXCLAMATION 3
#define MTD_ERROR       4
#define MTD_BUTTON1     0x00
#define MTD_BUTTON2     0x10
#define MTD_BUTTON3     0x20
#define MTD_BUTTON4     0x30
#define MTD_BUTTON5     0x40
#define MTD_MODAL       256

#define MTD_ICONMASK    15
#define MTD_BUTTONMASK  240

#define MTDR_NULL       -1
#define MTDR_CANCEL     -2
#define MTDR_TIMEOUT    -4

enum MTConfigType{
	MTCT_CONFIG = 0,
	MTCT_SINTEGER,
	MTCT_UINTEGER,
	MTCT_FLOAT,
	MTCT_BOOLEAN,
	MTCT_STRING,
	MTCT_BINARY
};

#define MTMC_STRUCTURE 1
#define MTMC_DATA      2
#define MTMC_ALL       3
#define MTMC_MD5KEYS   4
#define MTMC_HEADER    8

#ifdef _DEBUG
#	define LOG(T)												si->log(T,0)
#	define LOGD(T)												si->log(T,1)
#	define FLOG1(T,P1)										si->flog(T,0,P1)
#	define FLOG2(T,P1,P2)								si->flog(T,0,P1,P2)
#	define FLOG3(T,P1,P2,P3)							si->flog(T,0,P1,P2,P3)
#	define FLOG4(T,P1,P2,P3,P4)					si->flog(T,0,P1,P2,P3,P4)
#	define FLOG5(T,P1,P2,P3,P4,P5)				si->flog(T,0,P1,P2,P3,P4,P5)
#	define FLOGD1(T,P1)									si->flog(T,1,P1)
#	define FLOGD2(T,P1,P2)								si->flog(T,1,P1,P2)
#	define FLOGD3(T,P1,P2,P3)						si->flog(T,1,P1,P2,P3)
#	define FLOGD4(T,P1,P2,P3,P4)					si->flog(T,1,P1,P2,P3,P4)
#	define FLOGD5(T,P1,P2,P3,P4,P5)			si->flog(T,1,P1,P2,P3,P4,P5)
#	define DUMP(V,L,O)										si->dump((unsigned char*)V,L,O)
#	define ENTER(F)											si->enter(F)
#	define FENTER1(F,P1)									si->fenter(F,P1)
#	define FENTER2(F,P1,P2)							si->fenter(F,P1,P2)
#	define FENTER3(F,P1,P2,P3)						si->fenter(F,P1,P2,P3)
#	define FENTER4(F,P1,P2,P3,P4)				si->fenter(F,P1,P2,P3,P4)
#	define FENTER5(F,P1,P2,P3,P4,P5)			si->fenter(F,P1,P2,P3,P4,P5)
#	define FENTER6(F,P1,P2,P3,P4,P5,P6)	si->fenter(F,P1,P2,P3,P4,P5,P6)
#	define LEAVE()												si->leave()
#	define CALLSTACK											si->getcallstack()
#else
#	define LOG(T)
#	define LOGD(T)
#	define FLOG1(T,P1)
#	define FLOG2(T,P1,P2)
#	define FLOG3(T,P1,P2,P3)
#	define FLOG4(T,P1,P2,P3,P4)
#	define FLOGD1(T,P1)
#	define FLOGD2(T,P1,P2)
#	define FLOGD3(T,P1,P2,P3)
#	define FLOGD4(T,P1,P2,P3,P4)
#	define DUMP(V,L,O)
#	define ENTER(F)
#	define FENTER1(F,P1)
#	define FENTER2(F,P1,P2)
#	define FENTER3(F,P1,P2,P3)
#	define FENTER4(F,P1,P2,P3,P4)
#	define FENTER5(F,P1,P2,P3,P4,P5)
#	define FENTER6(F,P1,P2,P3,P4,P5,P6)
#	define LEAVE()
#	define CALLSTACK
#endif
#define mtnew(T) (T*)si->memalloc(sizeof(T),MTM_ZERO)
#define A(_A,_T) ((_T**)_A->a)
#define D(_A,_T) ((_T*)_A->d)

#if defined(MTSYSTEM_EXPORTS) && defined(_WIN32)
#	define MTTRY   __try{
#	define MTCATCH }__except(LPTOP_LEVEL_EXCEPTION_FILTER(si->onerror)(GetExceptionInformation())){
#	define MTEND };
#else
	typedef	void* (*_mt_try)(bool);
#	define MTTRY   try{if (sigsetjmp((__jmp_buf_tag*)((_mt_try)si->onerror)(false),1)==0){
#	define MTCATCH }else{throw "Got signal!";}}catch(...){
//#	define MTTRY   if (sigsetjmp((__jmp_buf_tag*)((_mt_try)si->onerror)(false),1)==0){
//#	define MTCATCH }else{
#	define MTEND }((_mt_try)si->onerror)(true);
#endif
//---------------------------------------------------------------------------
class MTThread;
class MTProcess;

class MTLock{
public:
	virtual ~MTLock() = 0;
	virtual bool MTCT lock(int timeout = -1) = 0;
	virtual void MTCT unlock() = 0;
};

class MTEvent{
public:
	virtual ~MTEvent() = 0;
	virtual bool MTCT pulse() = 0;
	virtual bool MTCT set() = 0;
	virtual bool MTCT reset() = 0;
	virtual bool MTCT wait(int timeout = -1) = 0;
protected:
	int d1;
#ifdef _WIN32
	void *d2;
#else
	bool d2,d3,d4;
	void *d5,*d6,*d7;
#endif
};

typedef int (MTCT *ThreadProc)(MTThread *thread,void *param);
typedef void (MTCT *ProcessProc)(MTProcess *process,void *param,float p);

class MTThread : public MTEvent{
public:
	mt_uint32 id;
	int type;
	int result;
	bool terminated;

	virtual ~MTThread() = 0;
	virtual void MTCT start() = 0;
	virtual bool MTCT getmessage(int &msg,int &param1,int &param2,bool wait = false) = 0;
	virtual void MTCT postmessage(int msg,int param1,int param2) = 0;
	virtual void MTCT terminate() = 0;
protected:
#ifndef _WIN32
	int d1,d2;
	void *d3;
#endif
	ThreadProc mproc;
	char *mname;
	void *mparam;
	int mpriority;
	bool mautofree;
	bool running;
};

class MTProcess : public MTThread{
public:
	int status;
	int priority;
	void *data;
	void *guidata;
	float progress;

	MTProcess(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc);
	virtual ~MTProcess() = 0;
	virtual void MTCT setprogress(float p) = 0;
};

class MTFile;
class MTFolder;

class MTFileHook{
public:
	virtual MTFile* MTCT fileopen(char *url,int flags) = 0;
	virtual MTFolder* MTCT folderopen(char *url) = 0;
	virtual bool MTCT filecopy(char *source,char *dest) = 0;
	virtual bool MTCT filerename(char *source,char *dest) = 0;
	virtual bool MTCT filedelete(char *url) = 0;
	virtual void MTCT filetype(const char *url,char *type,int length) = 0;
};

class MTFile{
public:
	char *url;

	virtual ~MTFile(){ };
	virtual int MTCT read(void *buffer,int size) = 0;
	virtual int MTCT readln(char *buffer,int maxsize) = 0;
//	virtual int MTCT reads(char *buffer,int maxsize) = 0;
	virtual int MTCT write(const void *buffer,int size) = 0;
	virtual int MTCT seek(int pos,int origin) = 0;
	virtual void* MTCT getpointer(int offset,int size) = 0;
	virtual void MTCT releasepointer(void *mem) = 0;
	virtual int MTCT length() = 0;
	virtual int MTCT pos() = 0;
	virtual bool MTCT eof() = 0;
	virtual bool MTCT seteof() = 0;
	virtual bool MTCT gettime(int *modified,int *accessed) = 0;
	virtual bool MTCT settime(int *modified,int *accessed) = 0;
	virtual MTFile* MTCT subclass(int start,int length,int access) = 0;
};

class MTFolder{
public:
	virtual ~MTFolder() = 0;
	virtual bool MTCT getfile(const char **name,int *attrib,double *size) = 0;
	virtual bool MTCT next() = 0;
};

class MTTimer;

typedef void (MTCT *TimerProc)(MTTimer *timer,int param);

class MTTimer{
public:
	virtual ~MTTimer() = 0;
};

typedef void (MTCT *ItemProc)(void *item,void *param);
typedef int (MTCT *SortProc)(void *item1,void *item2);

class MTArray{
public:
	int _is;
	int nitems;
	union{
		void **a;
		void *d;
	};

	inline void* operator[](unsigned int i){ return (a)?a[i]:((d)?(char*)d+_is*i:0); };
	virtual ~MTArray() = 0;
	virtual int MTCT additem(int at,void *item) = 0;
	virtual int MTCT additems(int at,int count) = 0;
	virtual void MTCT delitems(int from,int count) = 0;
	virtual int MTCT setitem(int at,void *item) = 0;
	virtual int MTCT push(void *item) = 0;
	virtual void* MTCT pop() = 0;
	virtual int MTCT getitemid(void *item) = 0;
	virtual void MTCT remove(void *item) = 0;
	virtual void MTCT clear(bool deldata = false,ItemProc = 0,void *param = 0) = 0;
	virtual void MTCT reset() = 0;
	virtual void* MTCT next() = 0;
	virtual void MTCT sort(SortProc proc) = 0;
};

class MTHash{
public:
	int nitems;

	virtual ~MTHash() = 0;
	virtual int MTCT additem(const char *key,void *data) = 0;
	virtual int MTCT additem(int key,void *data) = 0;
	virtual void MTCT delitem(const char *key,bool deldata = false,ItemProc proc = 0,void *param = 0) = 0;
	virtual void MTCT delitem(int key,bool deldata = false,ItemProc proc = 0,void *param = 0) = 0;
	virtual void MTCT delitemfromid(int id,bool deldata = false,ItemProc proc = 0,void *param = 0) = 0;
	virtual void* MTCT getitem(const char *key) = 0;
	virtual void* MTCT getitem(int key) = 0;
	virtual void* MTCT getitemfromid(int id) = 0;
	virtual int MTCT getitemid(void *item) = 0;
	virtual const char* MTCT getitemkey(void *item) = 0;
	virtual void MTCT clear(bool deldata = false,ItemProc proc = 0,void *param = 0) = 0;
	virtual void MTCT reset() = 0;
	virtual void* MTCT next() = 0;
};

class MTResources{
public:
	virtual ~MTResources() = 0;
	virtual int MTCT getnumresources() = 0;
	virtual bool MTCT getresourceinfo(int id,int *type,int *uid,int *size) = 0;
	virtual int MTCT loadresource(int type,int uid,void *buffer,int size) = 0;
	virtual int MTCT loadstring(int uid,char *buffer,int size) = 0;
	virtual int MTCT loadstringf(int uid,char *buffer,int size,...) = 0;
	virtual void* MTCT getresource(int type,int uid,int *size) = 0;
	virtual void MTCT releaseresource(void *res) = 0;
	virtual MTFile* MTCT getresourcefile(int type,int uid,int *size) = 0;
	virtual void MTCT releaseresourcefile(MTFile *f) = 0;
	virtual bool MTCT addresource(int type,int uid,void *res,int size) = 0;
	virtual bool MTCT addfile(int type,int uid,MTFile *f) = 0;
	virtual const char* MTCT getresourceurl() = 0;
};

class MTConfigFile{
public:
	virtual ~MTConfigFile() = 0;
	virtual void MTCT clear() = 0;
	virtual bool MTCT setsection(const char *name) = 0;
	virtual bool MTCT getparameter(const char *paramname,void *value,int desiredtype,int size) = 0;
	virtual bool MTCT createsection(const char *name) = 0;
	virtual bool MTCT setparameter(const char *paramname,void *value,int type,int size) = 0;
	virtual const char* MTCT getfilename() = 0;
};

class MTMiniConfig{
public:
	virtual ~MTMiniConfig() = 0;
	virtual bool MTCT getparameter(const char *paramname,void *value,int desiredtype,int size) = 0;
	virtual bool MTCT setparameter(const char *paramname,void *value,int type,int size) = 0;
	virtual int MTCT loadfromstream(MTFile *f,int flags = (MTMC_ALL|MTMC_HEADER)) = 0;
	virtual int MTCT savetostream(MTFile *f,int flags = (MTMC_ALL|MTMC_HEADER)) = 0;
};

class MTXML{
public:
	virtual ~MTXML() = 0;
	virtual void* MTCT readfile(const char *file,const char *encoding) = 0;
	virtual void* MTCT readmemory(const char *buf,int size,const char *encoding) = 0;
	virtual char* MTCT getname(void *node) = 0;
	virtual void* MTCT getchildren(void *node) = 0;
	virtual void* MTCT getlast(void *node) = 0;
	virtual void* MTCT getparent(void *node) = 0;
	virtual void* MTCT getnext(void *node) = 0;
	virtual void* MTCT getprev(void *node) = 0;
	virtual char* MTCT getcontent(void *node) = 0;
	virtual void* MTCT getproperties(void *node) = 0;
};

class MTCPUMonitor{
public:
	int flushid;
	virtual ~MTCPUMonitor() = 0;
	virtual void MTCT startslice(int id) = 0;
	virtual void MTCT endslice(int id) = 0;
	virtual void MTCT flushcpu(int id) = 0;
	virtual void MTCT startadd(int id) = 0;
	virtual void MTCT endadd(int id) = 0;
	virtual double MTCT getcpu(int id) = 0;
	virtual void* MTCT getcpustate(int id) = 0;
	virtual int MTCT getcpustateid(void *s) = 0;
	virtual int MTCT addcpustate() = 0;
	virtual void MTCT delcpustate(int id) = 0;
};

class MTSystemInterface : public MTXInterface{
public:
	int sysflags;
	char *platform;
	char *build;
	char *processor;
	char *hostname;
	int ncpu;
	int cpufrequ;
	void *onerror;

	virtual int MTCT getlasterror() = 0;
	virtual void MTCT setlasterror(int error) = 0;
	virtual void MTCT addfilehook(char *type,MTFileHook *hook) = 0;
	virtual void MTCT delfilehook(char *type,MTFileHook *hook) = 0;

	void* (MTCT *memalloc)(int size,int flags);
	bool (MTCT *memfree)(void *mem);
	void* (MTCT *memrealloc)(void *mem,int size);
	MTThread* (MTCT *getsysthread)();
	MTThread* (MTCT *getcurrentthread)();
	bool (MTCT *issysthread)();
	bool (MTCT *setprivatedata)(int id,void *data);
	void* (MTCT *getprivatedata)(int id);
	int (MTCT *localalloc)();
	bool (MTCT *localfree)(int id);
	bool (MTCT *localset)(int id,void *value);
	void* (MTCT localget)(int id);
	MTThread* (MTCT *threadcreate)(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name);
	MTProcess* (MTCT *processcreate)(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name);
	MTFile* (MTCT *fileopen)(char *url,int flags);
	void (MTCT *fileclose)(MTFile *file);
	bool (MTCT *fileexists)(char *filename);
	bool (MTCT *filecopy)(char *filename,char *destination);
	bool (MTCT *filedelete)(char *filename);
	bool (MTCT *filerename)(char *filename,char *newname);
	void (MTCT *filetype)(const char *filename,char *filetype,int length);
	void (MTCT *filemaketemp)(char *filename,int length);
	MTFolder* (MTCT *folderopen)(char *path);
	void (MTCT *folderclose)(MTFolder *folder);
	MTArray* (MTCT *arraycreate)(int nallocby,int itemsize);
	void (MTCT *arraydelete)(MTArray *array);
	MTHash* (MTCT *hashcreate)(int nallocby);
	void (MTCT *hashdelete)(MTHash *array);
	MTResources* (MTCT *resfind)(const char *filename,bool write);
	MTResources* (MTCT *resopen)(MTFile *f,bool ownfile);
	void (MTCT *resclose)(MTResources *res);
	MTConfigFile* (MTCT *configfind)(const char *filename);
	MTConfigFile* (MTCT *configopen)(const char *filename);
	void (MTCT* configclose)(MTConfigFile* file);
	MTMiniConfig* (MTCT *miniconfigcreate)();
	void (MTCT *miniconfigdelete)(MTMiniConfig *cfg);
	MTXML* (MTCT *xmlcreate)();
	void (MTCT *xmldelete)(MTXML *xml);
	MTLock* (MTCT *lockcreate)();
	void (MTCT *lockdelete)(MTLock *lock);
	MTEvent* (MTCT *eventcreate)(bool autoreset,int interval,int resolution,bool periodic,bool pulse);
	void (MTCT *eventdelete)(MTEvent *event);
	MTTimer* (MTCT *timercreate)(int interval,int resolution,bool periodic,int param,TimerProc proc);
	MTTimer* (MTCT *timerevent)(int interval,int resolution,bool periodic,MTEvent *event,bool pulse);
	void (MTCT *timerdelete)(MTTimer *timer);
	MTCPUMonitor *(MTCT *cpumonitorcreate)(int ncounters);
	int (MTCT *syscounter)();
	bool (MTCT *syscounterex)(double *count);
	void (MTCT *syswait)(int ms);
	int (MTCT *syswaitmultiple)(int count,MTEvent **events,bool all,int timeout);
	int (MTCT *dialog)(char *message,char *caption,char *buttons,int flags,int timeout);
	int (MTCT *resdialog)(MTResources *res,int id,char *caption,char *buttons,int timeout,int flags,...);
	int (MTCT *authdialog)(char *message,char *login,char *password);
	void (MTCT *showoserror)(int error);
	void (MTCT *showlastoserror)();
	void (MTCT *log)(const char *log,char date);
	void (MTCT *flog)(const char *log,char date,...);
	void (MTCT *dump)(unsigned char *address,int length,int offset);
	void (MTCT *enter)(const char *func);
	void (MTCT *fenter)(const char *func,...);
	void (MTCT *leave)();
	const char* (MTCT *getcallstack)();
	void (MTCT *getlibmemoryrange)(void *lib,int flags,void **start,int *length);
	int (MTCT *sync_inc)(int *value);
	int (MTCT *sync_dec)(int *value);
};
//---------------------------------------------------------------------------
#endif
