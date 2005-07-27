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
#include <stdio.h>
#include <time.h>
#include "MTSystem1.h"
#include "../Headers/MTXSystem2.h"
#include "MTInternet.h"
#include "../Headers/MTXExtension.h"
#include "../Headers/MTXDisplay.h"
#include "../Headers/MTXGUI.h"
#include "../Headers/MTXControls.h"
#include "../../debug/Interface/MTSystemRES.h"
#include <shellapi.h>
#include <mmsystem.h>
//---------------------------------------------------------------------------
static const char *sysname = {"MadTracker System Core"};
static const int sysversion = 0x30000;
static const MTXKey systemkey = {0,0,0,0};
#ifdef MTSYSTEM_EXPORTS
MTXInterfaces i;
MTSystemInterface *si;
#endif
MTInterface *mtinterface;
MTGUIInterface *gi;
MTDisplayInterface *di;
MTResources *sysres;
OSVERSIONINFO osinfo;
SYSTEM_INFO sysinfo;
TIMECAPS timecaps;
MTWindow *auth;
bool debugged = false;
extern char rootn[64];
char dialog[256];
char *d_ok,*d_cancel,*d_yes,*d_no;
#ifdef MTSYSTEM_EXPORTS
//---------------------------------------------------------------------------
//  Logging
//---------------------------------------------------------------------------
MTFile *logfile;
char logpath[256],logbuf[1024],logdate[128],lastlog[1024];
int lastcount;
int lasttime;
bool logging = false;
bool waserror = false;
bool checklast = true;

void startlog()
{
	char buf[64];

	logfile = mtfileopen(logpath,MTF_READ|MTF_WRITE|MTF_SHAREREAD|MTF_CREATE);
	if (logfile){
		logfile->seek(0,MTF_END);
		logging = true;
		mtlog("== Logging started on %s =="NL,2);
		if (mtinterface){
			mtlog("Application:   ");
			mtlog(mtinterface->name);
			mtflog(" %d.%d.%d"NL,false,mtinterface->version>>16,(mtinterface->version>>8) & 0xFF,mtinterface->version & 0xFF);
		};
		mtlog("Platform:      ");
		mtlog(si->platform);
		mtlog(" ");
		mtlog(si->build);
		mtlog(NL"Processor:     ");
		mtlog(si->processor);
		MEMORYSTATUS mem;
		GlobalMemoryStatus(&mem);
		mtlog(NL"Memory:        Total: ");
		sprintf(buf,"%d MB (%d MB)",mem.dwTotalPhys/1048576,mem.dwTotalPageFile/1048576);
		mtlog(buf);
		mtlog(NL"               Free:  ");
		sprintf(buf,"%d MB (%d MB)"NL,mem.dwAvailPhys/1048576,mem.dwAvailPageFile/1048576);
		mtlog(buf);
		mtlog("Capabilities:  Timer resolution: ");
		sprintf(buf,"%d msec"NL""NL,timecaps.wPeriodMin);
		mtlog(buf);
	};
}

void stoplog()
{
	if (logfile){
		mtlog(NL"== Logging stopped on %s =="NL""NL""NL,2);
		logging = false;
		delete logfile;
		logfile = 0;
	};
}

void mtlog(const char *log,char date)
{
#ifdef _DEBUG
	bool iserror = false;
#endif

	if ((!logging) || (!log) || (log[0]==0)) return;
	if (!logfile){
		logging = false;
		startlog();
		if (!logging) return;
	};
	if ((checklast) && (log[0]!='\r') && (log[0]!='\n') && (log[0]!=' ') && (mtsyscounter()-lasttime>3000) && (strcmp(lastlog,log)==0)) lastcount++;
	else{
		if ((checklast) && (lastcount>1)){
			sprintf(logdate,"  WARNING: ^ This message has been repeated %d times."NL""NL,lastcount);
			logfile->write(logdate,strlen(logdate));
		};
		if (strstr(log,"ERROR")){
#ifdef _DEBUG
			iserror = true;
#endif
			waserror = true;
			if (debugged) DebugBreak();
		};
		if (checklast){
			lastcount = 1;
			lasttime = mtsyscounter();
			strcpy(lastlog,log);
		};
		if (date){
			time_t lt;
			tm *lts;
			time(&lt);
			lts = localtime(&lt);
			if (date>1){
				sprintf(logdate,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",lts->tm_year+1900,lts->tm_mon+1,lts->tm_mday,lts->tm_hour,lts->tm_min,lts->tm_sec);
			}
			else{
				sprintf(logdate,"%.2d:%.2d:%.2d (%.8d)",lts->tm_hour,lts->tm_min,lts->tm_sec,mtsyscounter());
			};
			sprintf(logbuf,log,logdate);
			if (debugged) OutputDebugString(logbuf);
			logfile->write(logbuf,strlen(logbuf));
		}
		else{
			if (debugged) OutputDebugString(log);
			logfile->write(log,strlen(log));
		};
#ifdef _DEBUG
		if (iserror){
			const char *cs = CALLSTACK;
			if ((cs) && (cs[0])){
				sprintf(logbuf,"  %s"NL,CALLSTACK);
				logfile->write(logbuf,strlen(logbuf));
			};
		};
#endif
	};
}

void mtflog(const char *log,char date,...)
{
	va_list l;
	int dl;
	const char *e;
	static char logdate[32];
	
	e = log;
	if (date){
		while (true){
			e = strchr(e,'%');
			if (!e) return;
			if (*(++e)!='s') continue;
			dl = ((++e)-log);
			memcpy(logdate,log,dl);
			logdate[dl] = 0;
			checklast = false;
			mtlog(logdate,date);
			checklast = true;
			break;
		};
	};
	va_start(l,date);
	vsprintf(logbuf,e,l);
	va_end(l);
	mtlog(logbuf);
}

void mtdump(unsigned char *address,int length,int offset)
{
	unsigned int *cl = (unsigned int*)address;
	unsigned char *e = address+length;
	int x;
	char val[9];

	val[8] = 0;
	offset = 0;
	try{
		while (address<e){
			if (IsBadReadPtr(address,8)==0){
				memcpy(val,address,8);
				for (x=0;x<8;x++){
					if (val[x]<' ') val[x] = '.';
				};
				mtflog("%.8X (+%.4X): %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X - %.8X %.8X ; %s"NL,false,address,offset,address[0],address[1],address[2],address[3],address[4],address[5],address[6],address[7],cl[0],cl[1],val);
				address += 8;
				cl += 2;
			}
			else{
				mtflog("%.8X (+%.4X): ?? ?? ?? ?? ?? ?? ?? ?? - ???????? ???????? ; ????????"NL,false,address,offset);
				address += 8;
				cl += 2;
			};
			offset += 8;
		};
	}
	catch(...){
	};
}
//---------------------------------------------------------------------------
//  Call Stack
//---------------------------------------------------------------------------
void mtenter(const char *func)
{
#ifdef _DEBUG
	int l;
	bool first;
	char *callstack = (char*)mtgetprivatedata(-5);
	char *csp = (char*)mtgetprivatedata(-4);
	int lostcount = (int)mtgetprivatedata(-3);

	if (!callstack) return;
	l = strlen(func);
	first = (csp==&callstack[MAX_STACK-1]);
	if (!first){
		if (csp-3-l<callstack){
			mtsetprivatedata(-3,(void*)(lostcount+1));
			return;
		};
		csp -= 3;
		memcpy(csp," < ",3);
	}
	else if (csp-l<callstack){
		mtsetprivatedata(-3,(void*)(lostcount+1));
		return;
	};
	csp -= l;
	memcpy(csp,func,l);
	mtsetprivatedata(-4,csp);
#endif
}

void mtfenter(const char *func,...)
{
#ifdef _DEBUG
	int x;
	bool first;
	char *callstack = (char*)mtgetprivatedata(-5);
	char *csp = (char*)mtgetprivatedata(-4);
	int lostcount = (int)mtgetprivatedata(-3);
	va_list l;
	static char buf[256];

	if (!callstack) return;
	va_start(l,func);
	vsprintf(buf,func,l);
	va_end(l);
	x = strlen(buf);
	first = (csp==&callstack[MAX_STACK-1]);
	if (!first){
		if (csp-3-x<callstack){
			mtsetprivatedata(-3,(void*)(lostcount+1));
			return;
		};
		csp -= 3;
		memcpy(csp," < ",3);
	}
	else if (csp-x<callstack){
			mtsetprivatedata(-3,(void*)(lostcount+1));
		return;
	};
	csp -= x;
	memcpy(csp,buf,x);
	mtsetprivatedata(-4,csp);
#endif
}

void mtleave()
{
#ifdef _DEBUG
	char *e;
	char *callstack = (char*)mtgetprivatedata(-5);
	char *csp = (char*)mtgetprivatedata(-4);
	int lostcount = (int)mtgetprivatedata(-3);

	if (!callstack) return;
	if (lostcount){
		mtsetprivatedata(-3,(void*)(lostcount-1));
		return;
	};
	e = strchr(csp,'<');
	if (e){
		csp = e+2;
	}
	else{
		csp = &callstack[MAX_STACK-1];
	};
	mtsetprivatedata(-4,csp);
#endif
}

const char* mtgetcallstack()
{
#ifdef _DEBUG
	return (const char*)mtgetprivatedata(-4);
#else
	return "N/A (Use debug build instead.)";
#endif
}

void mtgetlibmemoryrange(void *lib,int flags,void **start,int *length)
{
	MEMORY_BASIC_INFORMATION meminfo;
	void *end;

	if ((!start) || (!length)) return;
	*start = lib;
	*length = 0;
	end = lib;
	while (VirtualQuery(end,&meminfo,sizeof(meminfo))){
		if ((meminfo.AllocationBase==0) || (meminfo.State!=MEM_COMMIT)) break;
		end = (char*)end+meminfo.RegionSize;
	};
	*length = (char*)end-(char*)lib;
}
//---------------------------------------------------------------------------
//  Exception Handling
//---------------------------------------------------------------------------
static struct{
	unsigned int ecode;
	char *emessage;
} _e[20] = {
	{EXCEPTION_ACCESS_VIOLATION,"Access violation"},
	{EXCEPTION_ARRAY_BOUNDS_EXCEEDED,"Array out of bounds"},
	{EXCEPTION_BREAKPOINT,"Breakpoint"},
	{EXCEPTION_DATATYPE_MISALIGNMENT,"Data misalignment"},
	{EXCEPTION_FLT_DENORMAL_OPERAND,"FP Denormal operand"},
	{EXCEPTION_FLT_DIVIDE_BY_ZERO,"FP Divide by zero"},
	{EXCEPTION_FLT_INEXACT_RESULT,"FP Inexact result"},
	{EXCEPTION_FLT_INVALID_OPERATION,"FP Invalid operation"},
	{EXCEPTION_FLT_OVERFLOW,"FP Overflow"},
	{EXCEPTION_FLT_STACK_CHECK,"FP Stack check"},
	{EXCEPTION_FLT_UNDERFLOW,"FP Underflow"},
	{EXCEPTION_ILLEGAL_INSTRUCTION,"Illegal instruction"},
	{EXCEPTION_IN_PAGE_ERROR,"In page error"},
	{EXCEPTION_INT_DIVIDE_BY_ZERO,"Divide by zero"},
	{EXCEPTION_INT_OVERFLOW,"Overflow"},
	{EXCEPTION_INVALID_DISPOSITION,"Invalid disposition"},
	{EXCEPTION_NONCONTINUABLE_EXCEPTION,"Noncontinuable exception"},
	{EXCEPTION_PRIV_INSTRUCTION,"Privilege instruction"},
	{EXCEPTION_STACK_OVERFLOW,"Stack overflow"},
	{0,"Unknown exception"}
};
char errorbuf[1024];

int WINAPI onexception(EXCEPTION_POINTERS *ep)
{
	static char *readwrite[2] = {"read","write"};

	if (GetAsyncKeyState(VK_SHIFT)<0) return EXCEPTION_EXECUTE_HANDLER;
	if (ep){
		int x;
		CONTEXT &c = *ep->ContextRecord;
		unsigned char *cip = (unsigned char*)c.Eip;
		unsigned char *csp = (unsigned char*)c.Esp;
		char *cecode,*cext,*e;
		bool waslogging = logging;

		if (!logging) startlog();
		cecode = _e[19].emessage;
		for (x=0;x<19;x++){
			if (ep->ExceptionRecord->ExceptionCode==_e[x].ecode){
				cecode = _e[x].emessage;
				break;
			};
		};
		strcpy(errorbuf,"%s - [EXCEPTION] %s at address %.8X");
		if ((mtinterface) && ((cext = mtinterface->getextension((char*)ep->ExceptionRecord->ExceptionAddress)))){
			e = strrchr(cext,'/');
			if (!e) e = strrchr(cext,'\\');
			if (e) cext = e+1;
			strcat(errorbuf," in %s"NL);
			mtflog(errorbuf,true,cecode,(char*)ep->ExceptionRecord->ExceptionAddress,cext);
		}
		else{
			strcat(errorbuf,NL);
			mtflog(errorbuf,true,cecode,(char*)ep->ExceptionRecord->ExceptionAddress);
		};
		strcpy(errorbuf,"Callstack:"NL"%s"NL);
		mtflog(errorbuf,false,mtgetcallstack());
		if ((x==0) && (ep->ExceptionRecord->NumberParameters>=2)){
			mtflog("Cannot %s memory at address %.8X"NL""NL,false,readwrite[ep->ExceptionRecord->ExceptionInformation[0]],ep->ExceptionRecord->ExceptionInformation[1]);
		};
#ifdef _X86_
		mtflog("EAX: %.8X  ESI: %.8X"NL"\
EBX: %.8X  EDI: %.8X"NL"\
ECX: %.8X  ESP: %.8X"NL"\
EDX: %.8X  EBP: %.8X"NL"\
EIP: %.8X"NL""NL,false,c.Eax,c.Esi,c.Ebx,c.Edi,c.Ecx,c.Esp,c.Edx,c.Ebp,c.Eip);
		if (IsBadReadPtr(cip,16)==0){
			mtlog("Bytes at EIP:"NL);
			mtflog("%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X ",false,cip[0],cip[1],cip[2],cip[3],cip[4],cip[5],cip[6],cip[7]);
			cip += 8;
			mtflog("%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X"NL""NL,false,cip[0],cip[1],cip[2],cip[3],cip[4],cip[5],cip[6],cip[7]);
		};
		mtlog("Stack dump:"NL);
		mtdump(csp,64);
		if (!waslogging) stoplog();
#endif
	};
	return (debugged)?EXCEPTION_CONTINUE_SEARCH:EXCEPTION_EXECUTE_HANDLER;
}
#endif
//---------------------------------------------------------------------------
//  Memory Debugging
//---------------------------------------------------------------------------
#ifdef _DEBUG
unsigned int nallocs,nmallocs,totalmemory,peakmemory;
MTLock memlock;
struct MTAlloc{
	void *address;
	int size;
	void *caller;
	char *callstack;
} *mtalloc;

MTAlloc *addalloc()
{
	if ((!mtalloc) || (nmallocs<=0)){
		nmallocs = 1024;
		mtalloc = (MTAlloc*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,nmallocs*sizeof(MTAlloc));
	}
	else if (nallocs>=nmallocs){
		nmallocs += 64;
		mtalloc = (MTAlloc*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,mtalloc,nmallocs*sizeof(MTAlloc));
	};
	return &mtalloc[nallocs++];
}

void delalloc(unsigned int id)
{
	if (mtalloc[id].callstack) HeapFree(GetProcessHeap(),0,mtalloc[id].callstack);
	if (id!=nallocs-1) memcpy(&mtalloc[id],&mtalloc[nallocs-1],sizeof(MTAlloc));
	mtmemzero(&mtalloc[nallocs-1],sizeof(MTAlloc));
	nallocs--;
	if ((nallocs<nmallocs-64) && (nmallocs>1024)){
		nmallocs -= 64;
		if (nmallocs>0){
			mtalloc = (MTAlloc*)HeapReAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,mtalloc,nmallocs*sizeof(MTAlloc));
		}
		else{
			HeapFree(GetProcessHeap(),0,mtalloc);
			mtalloc = 0;
		};
	};
}
#endif
//---------------------------------------------------------------------------
void* mtmemalloc(int size,int flags)
{
	void *mem;
	
#ifdef _DEBUG
	if (size>0x2000000){
		LOGD("%s - [System] ERROR: Too big memory allocation!"NL);
		return 0;
	};
#endif
	mem = HeapAlloc(GetProcessHeap(),(flags & MTM_ZERO)?HEAP_ZERO_MEMORY:0,size);
#ifdef _DEBUG
	if (mem){
		const char *cs;
		memlock.lock();
		totalmemory += size;
		if (totalmemory>peakmemory) peakmemory = totalmemory;
		MTAlloc &calloc = *addalloc();
		calloc.address = mem;
		calloc.size = size;
		calloc.caller = *((void**)(((char*)(&size))-4));
#ifdef MTSYSTEM_EXPORTS
		cs = mtgetcallstack();
		if ((cs) && (strlen(cs)>0)){
			calloc.callstack = (char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,strlen(cs)+1);
			strcpy(calloc.callstack,cs);
		};
#endif
		memlock.unlock();
	}
	else{
		LOGD("%s - [System] ERROR: Cannot allocate memory!"NL);
	};
#endif
	return mem;
}

bool mtmemfree(void *mem)
{
	if (mem){
#ifdef _DEBUG
		bool ok = false;
		unsigned int x;
		memlock.lock();
		for (x=0;x<nallocs;x++){
			if (mtalloc[x].address==mem){
				totalmemory -= mtalloc[x].size;
				ok = true;
				delalloc(x);
				break;
			};
		};
		memlock.unlock();
		if (!ok){
			LOGD("%s - [System] ERROR: Invalid memory pointer!"NL);
			return false;
		};
#endif
		return (HeapFree(GetProcessHeap(),0,mem)==0);
	};
	return true;
}

void* mtmemrealloc(void *mem,int size)
{
	HANDLE ph;
	void* newmem;
	int csize;
	
	if (!mem) return mtmemalloc(size,MTM_ZERO);
	ph = GetProcessHeap();
	csize = HeapSize(ph,0,mem);
	if (size<=csize) return mem;
	newmem = HeapReAlloc(ph,HEAP_ZERO_MEMORY,mem,size);
	if (!newmem){
		mtshowlastoserror();
		newmem = HeapAlloc(ph,HEAP_ZERO_MEMORY,size);
		if (!newmem) mtshowlastoserror();
		else{
			memcpy(newmem,mem,HeapSize(ph,0,mem));
			HeapFree(ph,0,mem);
		};
	};
#ifdef _DEBUG
	unsigned int x;
	memlock.lock();
	for (x=0;x<nallocs;x++){
		if (mtalloc[x].address==mem){
			totalmemory -= mtalloc[x].size;
			mtalloc[x].address = newmem;
			mtalloc[x].size = size;
			totalmemory += size;
			if (totalmemory>peakmemory) peakmemory = totalmemory;
			break;
		};
	};
	memlock.unlock();
#endif
	return (void*)newmem;
}
//---------------------------------------------------------------------------
bool MTCT DialogProc(MTWinControl *window,MTCMessage &msg)
{
	if (msg.msg==MTCM_TIMER){
		window->modalresult = MTDR_TIMEOUT;
		MTCMessage msg = {MTCM_CLOSE,0,window};
		window->message(msg);
		return true;
	}
	else if (msg.msg==MTCM_CLOSE){
		gi->deltimer(window,window->tag);
	};
	return false;
}

int mtdialog(char *message,char *caption,char *buttons,int flags,int timeout)
{
	MTDesktop *dsk;
	int x,y,l,t,h,r,size,cbutton;
	char *e;
	char buf[128];
	
	if ((sysres) && ((dsk = (MTDesktop*)di->getdefaultdesktop())) && ((dsk->flags & MTCF_HIDDEN)==0)){
		MTFile *wf = sysres->getresourcefile('NWTM',MTW_dialog,&size);
		if (wf){
			MTLabel *c;
			MTIcon *i;
			MTButton *ob;
			MTButton *b[8];
			MTWindow *alert = gi->loadwindowfromfile(wf,size,dsk);
			sysres->releaseresourcefile(wf);
			alert->messageproc = DialogProc;
			alert->flags |= MTCF_FREEONCLOSE;
			c = (MTLabel*)alert->getcontrolfromuid(MTC_ltext);
			i = (MTIcon*)alert->getcontrolfromuid(MTC_icon);
			mtmemzero(b,sizeof(b));
			ob = (MTButton*)alert->getcontrolfromuid(MTC_b1);
			if ((!c) || (!ob)){
				gi->delcontrol(alert);
				return MTDR_NULL;
			};
			alert->setcaption(caption);
			if (i) i->icon = flags & MTD_ICONMASK;
			if (((int)buttons<256) && (dialog[0]==0)){
				if (sysres->loadresource(MTR_TEXT,MTT_dialog,dialog,sizeof(dialog))){
					d_ok = dialog;
					d_cancel = strchr(d_ok,'|');
					if (d_cancel){
						*d_cancel++ = 0;
						d_yes = strchr(d_cancel,'|');
						if (d_yes){
							*d_yes++ = 0;
							d_no = strchr(d_yes,'|');
							if (d_no) *d_no++ = 0;
							else dialog[0] = 1;
						}
						else dialog[0] = 1;
					}
					else dialog[0] = 1;
				}
				else{
					dialog[0] = 1;
				};
			};
			if (dialog[0]==1){
				gi->delcontrol(alert);
				return MTDR_NULL;
			};
			cbutton = (flags & MTD_BUTTONMASK)>>4;
			l = ob->left;
			t = ob->top;
			h = ob->height;
			r = l+ob->width;
			ob->switchflags(MTCF_HIDDEN,true);
			switch ((int)buttons){
			case (int)MTD_OK:
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7]->setcaption(d_ok);
				b[7]->modalresult = 0;
				alert->focus(b[7]);
				break;
			case (int)MTD_OKCANCEL:
				b[6] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[6]->setcaption(d_ok);
				b[7]->setcaption(d_cancel);
				b[6]->modalresult = 0;
				b[7]->modalresult = 1;
				if (cbutton>1) cbutton = 1;
				alert->focus(b[6+cbutton]);
				break;
			case (int)MTD_YESNO:
				b[6] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[6]->setcaption(d_yes);
				b[7]->setcaption(d_no);
				b[6]->modalresult = 0;
				b[7]->modalresult = 1;
				if (cbutton>1) cbutton = 1;
				alert->focus(b[6+cbutton]);
				break;
			case (int)MTD_YESNOCANCEL:
				b[5] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[6] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[7] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
				b[5]->setcaption(d_yes);
				b[6]->setcaption(d_no);
				b[7]->setcaption(d_cancel);
				b[5]->modalresult = 0;
				b[6]->modalresult = 1;
				b[7]->modalresult = 2;
				if (cbutton>2) cbutton = 2;
				alert->focus(b[5+cbutton]);
				break;
			default:
				strcpy(buf,buttons);
				for (x=7;x>=0;x--){
					e = strrchr(buf,'|');
					if (!e) e = buf;
					else *e++ = 0;
					b[x] = (MTButton*)gi->newcontrol(MTC_BUTTON,0,alert,l,t,0,h,0);
					b[x]->modalresult = x;
					if (e==buf) break;
				};
				y = x;
				e = buf;
				if (cbutton>7-y) cbutton = 7-y;
				alert->focus(b[y+cbutton]);
				for (x=y;x<8;x++){
					b[x]->setcaption(e);
					b[x]->modalresult -= y;
					e = strchr(e,0)+1;
				};
				break;
			};
			r += 4;
			for (x=7;x>=0;x--){
				if (!b[x]) break;
				r -= b[x]->width+4;
				b[x]->switchflags(MTCF_DONTSAVE,true);
				b[x]->setbounds(r,t,0,0);
				b[x]->align = MTCA_BOTTOMRIGHT;
			};
			if (r<l) r = alert->width+l-r;
			else r = 0;
			c->setcaption(message);
			if (alert->width>r) r = 0;
			h = alert->height;
			if (h<96) h = 96;
			alert->setbounds((dsk->width-alert->width)/2,(dsk->height-alert->height)/2,r,h);
			alert->modalresult = MTDR_CANCEL;
			if (timeout>0) alert->tag = gi->ctrltimer(alert,0,timeout,false);
			return dsk->show(alert,((flags & MTD_MODAL)!=0)?1:0);
		};
		return MTDR_NULL;
	}
	else if ((int)buttons>=256) return MTDR_NULL;
	else{
		int wflags = 0;
		int res;
		switch ((int)buttons){
		case (int)MTD_OK:
			wflags = MB_OK;
			break;
		case (int)MTD_OKCANCEL:
			wflags = MB_OKCANCEL;
			break;
		case (int)MTD_YESNO:
			wflags = MB_YESNO;
			break;
		case (int)MTD_YESNOCANCEL:
			wflags = MB_YESNOCANCEL;
			break;
		};
		switch (flags & MTD_ICONMASK){
		case MTD_INFORMATION:
			wflags |= MB_ICONINFORMATION;
			break;
		case MTD_QUESTION:
			wflags |= MB_ICONQUESTION;
			break;
		case MTD_EXCLAMATION:
			wflags |= MB_ICONEXCLAMATION;
			break;
		case MTD_ERROR:
			wflags |= MB_ICONERROR;
			break;
		};
		if (flags & MTD_BUTTONMASK){
			wflags |= (flags & MTD_BUTTONMASK)<<4;
		};
		res = MessageBox(0,message,caption,wflags);
		if ((res==IDOK) || (res==IDYES) || (res==IDABORT)) return 0;
		if (res==IDNO) return 1;
		if (buttons==MTD_YESNOCANCEL) return 2;
		return 1;
	};
}

int mtresdialog(MTResources *res,int id,char *caption,char *buttons,int flags,int timeout,...)
{
	int ret;
	va_list l;
	char *buf = (char*)mtmemalloc(1024);
	char *buffer = (char*)mtmemalloc(512);
	
	res->loadstring(id,buffer,1024);
	va_start(l,timeout);
	vsprintf(buf,buffer,l);
	va_end(l);
	mtmemfree(buffer);
	ret = mtdialog(buf,caption,buttons,flags,timeout);
	mtmemfree(buf);
	return ret;
}

int mtauthdialog(char *message,char *login,char *password)
{
	MTDesktop *dsk;
	MTLabel *l;
	MTEdit *elogin,*epassword;
	int size;
	
	if (!auth){
		if ((sysres) && ((dsk = (MTDesktop*)di->getdefaultdesktop()))){
			MTFile *wf = sysres->getresourcefile('NWTM',MTW_auth,&size);
			if (wf){
				auth = gi->loadwindowfromfile(wf,size,dsk);
				if ((int)auth<=0) return -1;
				auth->setbounds((dsk->width-auth->width)/2,(dsk->height-auth->height)/2,0,0);
				sysres->releaseresourcefile(wf);
			};
		};
	};
	dsk = auth->dsk;
	if ((auth) && ((dsk->flags & MTCF_HIDDEN)==0)){
		l = (MTLabel*)auth->getcontrolfromuid(MTC_lcaption);
		if (l) l->setcaption(message);
		elogin = (MTEdit*)auth->getcontrolfromuid(MTC_elogin);
		epassword = (MTEdit*)auth->getcontrolfromuid(MTC_epassword);
		if (!login) elogin->switchflags(MTCF_DISABLED,true);
		if (!password) epassword->switchflags(MTCF_DISABLED,true);
		auth->modalresult = MTDR_CANCEL;
		if (dsk->show(auth,1)==0){
			if (login) strcpy(login,elogin->text);
			if (password) strcpy(password,epassword->text);
			return 0;
		};
	};
	return -1;
}

void mtshowoserror(int error)
{
	char *message;
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,error,0,(char*)&message,16,0);
	LOGD("%s - [System] ERROR: ");
	LOG(message);
	LOG(NL);
	mtdialog(message,"System Error",MTD_OK,MTD_EXCLAMATION,0);
}

void mtshowlastoserror()
{
	mtshowoserror(GetLastError());
}
//---------------------------------------------------------------------------
int mtsync_inc(int *value)
{
	return InterlockedIncrement((long*)value);
}

int mtsync_dec(int *value)
{
	return InterlockedDecrement((long*)value);
}
//---------------------------------------------------------------------------
#ifdef MTSYSTEM_EXPORTS
/*--SDK--

--SDK--*/
MTSystemInterface::MTSystemInterface():
sysflags(0),
ncpu(1),
cpufrequ(0),
memalloc(mtmemalloc),
memfree(mtmemfree),
memrealloc(mtmemrealloc),
getsysthread(mtgetsysthread),
getcurrentthread(mtgetcurrentthread),
issysthread(mtissysthread),
setprivatedata(mtsetprivatedata),
getprivatedata(mtgetprivatedata),
threadcreate(mtthreadcreate),
processcreate(mtprocesscreate),
fileopen(mtfileopen),
fileclose(mtfileclose),
fileexists(mtfileexists),
filecopy(mtfilecopy),
filedelete(mtfiledelete),
filerename(mtfilerename),
filetype(mtfiletype),
filemaketemp(mtfilemaketemp),
folderopen(mtfolderopen),
folderclose(mtfolderclose),
arraycreate(mtarraycreate),
arraydelete(mtarraydelete),
hashcreate(mthashcreate),
hashdelete(mthashdelete),
resfind(mtresfind),
resopen(mtresopen),
resclose(mtresclose),
configfind(mtconfigfind),
configopen(mtconfigopen),
configclose(mtconfigclose),
miniconfigcreate(mtminiconfigcreate),
miniconfigdelete(mtminiconfigdelete),
lockcreate(mtlockcreate),
lockdelete(mtlockdelete),
eventcreate(mteventcreate),
eventdelete(mteventdelete),
timercreate(mttimercreate),
timerevent(mttimerevent),
timerdelete(mttimerdelete),
cpumonitorcreate(mtcpumonitorcreate),
syscounter(mtsyscounter),
syscounterex(mtsyscounterex),
syswait(mtsyswait),
syswaitmultiple(mtsyswaitmultiple),
dialog(mtdialog),
resdialog(mtresdialog),
authdialog(mtauthdialog),
showoserror(mtshowoserror),
showlastoserror(mtshowlastoserror),
log(mtlog),
flog(mtflog),
dump(mtdump),
enter(mtenter),
fenter(mtfenter),
leave(mtleave),
getcallstack(mtgetcallstack),
getlibmemoryrange(mtgetlibmemoryrange),
sync_inc(mtsync_inc),
sync_dec(mtsync_dec)
{
	type = systemtype;
	key = &systemkey;
	name = sysname;
	version = sysversion;
	status = 0;
}

bool MTSystemInterface::init()
{
	MTFile *f;
	MTConfigFile *conf;
	char *e;
	tm *lts;
	time_t lt;
	char lorev,hirev;
	unsigned int cpuflags,cpuver;
	bool savecpu = true;
	char buf[256],proctype[256];
	static const int cpu_wait = 3000;
	static const double cpu_div = 3000000.0;
	
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)::onexception);
	HMODULE hkernel = GetModuleHandle("KERNEL32.DLL");
	if (hkernel){
		BOOL (*isdebugger)();
		*(int*)&isdebugger = (int)GetProcAddress(hkernel,"IsDebuggerPresent");
		if (isdebugger){
			debugged = (isdebugger()!=0);
			if (GetAsyncKeyState(VK_SHIFT)>=0){
				if (debugged) sysflags |= MTS_DEBUGGED;
			};
		};
	};
	initFiles();
	if (mtinterface){
		platform = (char*)mtmemalloc(512);
		build = (char*)mtmemalloc(512);
		processor = (char*)mtmemalloc(512);
		osinfo.dwOSVersionInfoSize = sizeof(osinfo);
		GetVersionEx(&osinfo);
		GetSystemInfo(&sysinfo);
		ncpu = sysinfo.dwNumberOfProcessors;
		strcpy(platform,"Windows ");
		if (osinfo.dwPlatformId==VER_PLATFORM_WIN32_NT){
			sysflags |= MTS_WINNT;
			if (osinfo.dwMajorVersion>=5){
				switch (osinfo.dwMinorVersion){
				case 0:
					strcat(platform,"2000 ");
					break;
				case 1:
					strcat(platform,"XP ");
					break;
				default:
					strcat(platform,"NT ");
					break;
				};
			}
			else{
				strcat(platform,"NT ");
			};
		}
		else{
			osinfo.dwBuildNumber &= 0xFFFF;
			if (osinfo.dwMinorVersion>=90){
				strcat(platform,"ME ");
			}
			else if (osinfo.dwMinorVersion>=10){
				strcat(platform,"98 ");
			}
			else{
				strcat(platform,"95 ");
			};
		};
		e = strchr(platform,0);
		sprintf(e,"Version %d.%d",osinfo.dwMajorVersion,osinfo.dwMinorVersion);
		if (osinfo.szCSDVersion[0]) sprintf(build,"Build %d (%s)",osinfo.dwBuildNumber,osinfo.szCSDVersion);
		else sprintf(build,"Build %d",osinfo.dwBuildNumber);
		sprintf(processor,"%d x ",sysinfo.dwNumberOfProcessors);
		switch (sysinfo.wProcessorArchitecture){
		case PROCESSOR_ARCHITECTURE_INTEL:
			strcpy(proctype,"x86 ");
			break;
		case PROCESSOR_ARCHITECTURE_MIPS:
			strcpy(proctype,"MIPS ");
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA:
			strcpy(proctype,"Alpha ");
			break;
		case PROCESSOR_ARCHITECTURE_PPC:
			strcpy(proctype,"PPC ");
			break;
		default:
			strcpy(proctype,"Unknown ");
			break;
		};
		__asm{
			push	ebx
			mov		eax,0x80000000
			cpuid
			cmp		eax,0x80000004
			jb		_noname
			mov		eax,0x80000002
			cpuid
			mov		dword ptr buf[0],eax
			mov		dword ptr buf[4],ebx
			mov		dword ptr buf[8],ecx
			mov		dword ptr buf[12],edx
			mov		eax,0x80000003
			cpuid
			mov		dword ptr buf[16],eax
			mov		dword ptr buf[20],ebx
			mov		dword ptr buf[24],ecx
			mov		dword ptr buf[28],edx
			mov		eax,0x80000004
			cpuid
			mov		dword ptr buf[32],eax
			mov		dword ptr buf[36],ebx
			mov		dword ptr buf[40],ecx
			mov		dword ptr buf[44],edx
			mov		byte ptr buf[48],0
			xor		eax,eax
			cpuid
			jmp		_nameok
_noname:
			xor		eax,eax
			cpuid
			mov		dword ptr buf[0],ebx
			mov		dword ptr buf[4],edx
			mov		dword ptr buf[8],ecx
			mov		byte ptr buf[12],0
_nameok:
			test	eax,eax
			jz		_nommx
			mov		eax,1
			cpuid
			mov		cpuver,eax
			mov		cpuflags,edx
			pop		ebx
_nommx:
		};
		strcat(proctype,buf);
		e = strchr(proctype,0);
		if (sysflags & MTS_WINNT){
			sprintf(e," Level %d ",sysinfo.wProcessorLevel);
			e = strchr(proctype,0);
			lorev = (char)sysinfo.wProcessorRevision;
			hirev = sysinfo.wProcessorRevision>>8;
			switch (sysinfo.wProcessorArchitecture){
			case PROCESSOR_ARCHITECTURE_INTEL:
				sprintf(e,"Model %d Stepping %d",hirev,lorev);
				break;
			case PROCESSOR_ARCHITECTURE_ALPHA:
				sprintf(e,"Model %c Pass %d",hirev+'A',lorev);
				break;
			case PROCESSOR_ARCHITECTURE_PPC:
				sprintf(e,"Version %d.%d",hirev,lorev);
				break;
			default:
				sprintf(e,"Revision %d",sysinfo.wProcessorRevision);
				break;
			};
		}
		else{
			lorev = cpuver & 0xF;
			cpuver >>= 4;
			hirev = cpuver & 0xF;
			cpuver >>= 4;
			cpuver &= 0xF;
			sprintf(e," Level %d Model %d Stepping %d",cpuver,hirev,lorev);
		};
		if ((conf = (MTConfigFile*)mtinterface->getconf("Global",false))){
			if (conf->setsection("MTSystem")){
				if (conf->getparameter("CPUType",&buf,MTCT_STRING,sizeof(buf))){
					if (strcmp(buf,proctype)==0){
						if (conf->getparameter("CPUFrequency",&cpufrequ,MTCT_UINTEGER,sizeof(cpufrequ))){
							savecpu = false;
						};
					};
				};
			};
		};
		if (cpufrequ==0){
			__asm{
				rdtsc
				push	edx
				push	eax
			};
			mtsyswait(cpu_wait);
			__asm{
				rdtsc
				sub		dword ptr [esp],eax
				sbb		dword ptr [esp+4],edx
				fild	qword ptr [esp]
				fchs
				fdiv	cpu_div;
				mov		eax,this
#if __BORLANDC__
				fistp	dword ptr [eax].cpufrequ
#elif __MWERKS__
				fistp	dword ptr [eax+MTSystemInterface.cpufrequ]
#else
				fistp	dword ptr [eax]this.cpufrequ
#endif
				add		esp,8
			};
			cpufrequ = (cpufrequ/3)*3;
		};
		if (conf){
			if (savecpu){
				if (conf->createsection("MTSystem")){
					conf->setparameter("CPUType",&proctype,MTCT_STRING,sizeof(proctype));
					conf->setparameter("CPUFrequency",&cpufrequ,MTCT_UINTEGER,sizeof(cpufrequ));
				};
			};
			mtinterface->releaseconf(conf);
		};
		if (cpuflags& 0x800000) sysflags |= MTS_MMX;
		if (cpuflags & 0x1000000) sysflags |= MTS_SIMD;
		e = strchr(processor,0);
		sprintf(e,"%d MHz (%s)",cpufrequ,proctype);
		timeGetDevCaps(&timecaps,sizeof(TIMECAPS));
		di = (MTDisplayInterface*)mtinterface->getinterface(displaytype);
		gi = (MTGUIInterface*)mtinterface->getinterface(guitype);
		strcpy(buf,mtinterface->getprefs()->syspath[SP_INTERFACE]);
		strcat(buf,"MTSystem.mtr");
		if ((gi) && (di) && (mtinterface->type!='EIUG')){
			f = mtfileopen(buf,MTF_READ|MTF_SHAREREAD);
			if (f) sysres = new MTResources(f,true);
		};
		strcpy(logpath,mtinterface->getprefs()->syspath[SP_ROOT]);
		strcat(logpath,"LOG_");
		e = strrchr(logpath,0);
		memcpy(e,&mtinterface->type,4);
		e += 4;
		*e++ = '_';
		time(&lt);
		lts = localtime(&lt);
		sprintf(e,"%.4d-%.2d-%.2d",lts->tm_year+1900,lts->tm_mon+1,lts->tm_mday);
		strcat(logpath,".txt");
#ifdef _DEBUG
		startlog();
#endif
		if (sysres) sysres->loadstring(MTT_storage,rootn,sizeof(rootn));
	};
	initInternet();
	initKernel();
	status |= MTX_INITIALIZED;
	return true;
}

void MTSystemInterface::uninit()
{
#ifdef _DEBUG
	unsigned int na;
	char buf[256];
#endif
	
	status &= (~MTX_INITIALIZED);
	mtmemfree(platform);
	mtmemfree(build);
	mtmemfree(processor);
	uninitKernel();
	uninitInternet();
	uninitSocket();
	if (sysres){
		delete sysres;
		sysres = 0;
	};
	uninitFiles();
	if (logfile){
		mtmemfree(logfile->url);
		logfile->url = 0;
	};
#ifdef _DEBUG
	if (nallocs!=0){
		int x = 0;

		na = nallocs;
		LOG("ERROR: Remaining memory allocations:"NL);
		while (nallocs>0){
			FLOG4(NL"%.8x: %d bytes allocated by %.8X"NL"Callstack:"NL"%s"NL,mtalloc[0].address,mtalloc[0].size,mtalloc[0].caller,mtalloc[0].callstack);
			if (x<16){
				mtdump((unsigned char*)mtalloc[0].address,64);
				x++;
			};
			mtmemfree(mtalloc[0].address);
		};
		sprintf(buf,"%d remaining memory allocation(s)!",na);
		mtdialog(buf,"Memory",MTD_OK,MTD_EXCLAMATION,5000);
	};
	FLOG1("Memory usage peak: %d bytes"NL,peakmemory);
#endif
	stoplog();
	if (waserror){
		if (mtdialog("One or more errors occured!"NL"Do you want to see the log file?","Errors",MTD_YESNO,MTD_QUESTION,5000)==0){
			ShellExecute(0,"open",logpath,"","",SW_SHOWNORMAL);
		};
	};
}

void MTSystemInterface::start()
{
#ifdef _DEBUG
	MTMiniConfig *mc = new MTMiniConfig();
	MTFile *mf = mtfileopen("mem://",MTF_CREATE|MTF_READ|MTF_WRITE);
	int test = 43234;
	char buf[256];
	mc->setparameter("Test",&test,MTCT_UINTEGER,4);
	mc->setparameter("Bla.Test","Hello there",MTCT_STRING,-1);
	mc->getparameter("Bla",buf,MTCT_STRING,sizeof(buf));
	mc->savetostream(mf);
	mtfileclose(mf);
	delete mc;
#endif
}

void MTSystemInterface::stop()
{
	stopThreads(true);
	if (auth){
		gi->delcontrol(auth);
		auth = 0;
	};
}

void MTSystemInterface::processcmdline(void *params)
{

}

void MTSystemInterface::showusage(void *out)
{

}

int MTSystemInterface::config(int command,int param)
{
	return 0;
}

int MTSystemInterface::getlasterror()
{
	return (int)mtgetprivatedata(-7);
}

void MTSystemInterface::setlasterror(int error)
{
	mtsetprivatedata(-7,(void*)error);
}

int MTSystemInterface::onexception(void *data)
{
	return ::onexception((EXCEPTION_POINTERS*)data);
}

void MTSystemInterface::addfilehook(char *type,MTFileHook *hook)
{
	hooks->additem(type,hook);
}

void MTSystemInterface::delfilehook(char *type,MTFileHook *hook)
{
	hooks->delitem(type);
}
//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* __stdcall MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!si) si = new MTSystemInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)si;
	return &i;
}

}
#endif
//---------------------------------------------------------------------------
