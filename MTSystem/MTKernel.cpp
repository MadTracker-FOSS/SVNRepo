//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTKernel.h"
#include "../Headers/MTXSystem2.h"
#include "../../debug/Interface/MTSystemRES.h"
#include <mmsystem.h>
//---------------------------------------------------------------------------
MTThread *systhread;
unsigned int nthreads,nathreads;
MTThread **threads;
MTLock *threadlock;
double sysfrequency;
bool pf;
int privateid[16];
BOOL (WINAPI *tryenter)(LPCRITICAL_SECTION lpCriticalSection);
//---------------------------------------------------------------------------
MTThread* mtgetsysthread()
{
	return systhread;
}

MTThread* mtgetcurrentthread()
{
	return (MTThread*)TlsGetValue(privateid[0]);
}

bool mtissysthread()
{
	return (GetCurrentThreadId()==systhread->id);
}

bool mtsetprivatedata(int id,void *data)
{
	return TlsSetValue(privateid[id+8],data);
}

void* mtgetprivatedata(int id)
{
	return TlsGetValue(privateid[id+8]);
}

int mtlocalalloc()
{
	return TlsAlloc();
}

bool mtlocalfree(int id)
{
	return TlsFree(id);
}

bool mtlocalset(int id,void *value)
{
	return TlsSetValue(id,value);
}

void* mtlocalget(int id)
{
	return TlsGetValue(id);
}

MTThread* mtthreadcreate(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name)
{
	return new MTThread(proc,autofree,autostart,param,priority,name);
}

MTProcess* mtprocesscreate(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name)
{
	return new MTProcess(tproc,param,type,priority,data,pproc,silent,name);
}

MTLock* mtlockcreate()
{
	return new MTLock();
}

void mtlockdelete(MTLock *lock)
{
	delete lock;
}

MTEvent* mteventcreate(bool autoreset,int interval,int resolution,bool periodic,bool pulse)
{
	return new MTEvent(autoreset,interval,resolution,periodic,pulse);
}

void mteventdelete(MTEvent *event)
{
	delete event;
}

MTTimer* mttimercreate(int interval,int resolution,bool periodic,int param,TimerProc proc)
{
	return new MTTimer(interval,resolution,periodic,param,proc);
}

MTTimer* mttimerevent(int interval,int resolution,bool periodic,MTEvent *event,bool pulse)
{
	return new MTTimer(interval,resolution,periodic,event,pulse);
}

void mttimerdelete(MTTimer *timer)
{
	delete timer;
}

MTCPUMonitor* mtcpumonitorcreate(int ncounters)
{
	return new MTCPUMonitor(ncounters);
}

int mtsyscounter()
{
	return GetTickCount();
}

bool mtsyscounterex(double *count)
{
	__int64 c;
	bool ok;
	
	if (pf){
		ok = QueryPerformanceCounter((LARGE_INTEGER*)&c)!=0;
		if (ok) *count = (double)c/sysfrequency;
		return ok;
	}
	else{
		*count = (double)GetTickCount()/1000;
		return true;
	};
}

void mtsyswait(int ms)
{
	Sleep(ms);
}

int mtsyswaitmultiple(int count,MTEvent **events,bool all,int timeout)
{
	if (count<=0) return -1;
	int x;
	HANDLE *h = (HANDLE*)mtmemalloc(sizeof(HANDLE)*count);
	for (x=0;x<count;x++) h[x] = events[x]->event;
	x = WaitForMultipleObjects(count,h,all,timeout);
	if (x==WAIT_TIMEOUT) x = -1;
	else x = x-WAIT_OBJECT_0;
	mtmemfree(h);
	return x;
}

int mtgetlasterror()
{
	return (int)mtgetprivatedata(-7);
}

void mtsetlasterror(int error)
{
	mtsetprivatedata(-7,(void*)error);
}
//---------------------------------------------------------------------------
void initKernel()
{
	int x;
	__int64 intf;
	HMODULE hkernel;

	for (x=0;x<16;x++) privateid[x] = TlsAlloc();
	pf = QueryPerformanceFrequency((LARGE_INTEGER*)&intf)!=0;
	if (pf) sysfrequency = (double)intf;
	else sysfrequency = 1000;
	threadlock = new MTLock();
	mtsetprivatedata(-8,new MTThread());
#ifdef _DEBUG
	char *callstack = (char*)mtmemalloc(MAX_STACK,MTM_ZERO);
	mtsetprivatedata(-5,callstack);
	mtsetprivatedata(-4,callstack+MAX_STACK-1);
	mtsetprivatedata(-3,0);
#endif
	hkernel = GetModuleHandle("KERNEL32.DLL");
	*(int*)&tryenter = (int)GetProcAddress(hkernel,"TryEnterCriticalSection");
}

void uninitKernel()
{
	int x;

	stopThreads(false);
	if (threads) mtmemfree(threads);
	delete systhread;
	delete threadlock;
#ifdef _DEBUG
	mtmemfree(mtgetprivatedata(-5));
#endif
	for (x=0;x<16;x++) TlsFree(privateid[x]);
}

void stopThreads(bool processes)
{
	int x,n;

	if (nthreads>0){
#ifdef _DEBUG
		char buf[256];
		if (processes){
			n = 0;
			for (x=0;x<nthreads;x++){
				if (threads[x]->type) n++;
			};
			if (n){
				sprintf(buf,"%d process(s) still running!",n);
				mtdialog(buf,"Processes",MTD_OK,MTD_EXCLAMATION,5000);
			};
		}
		else{
			sprintf(buf,"%d thread(s) still running!",nthreads);
			mtdialog(buf,"Threads",MTD_OK,MTD_EXCLAMATION,5000);
		};
#endif
		if (processes){
			for (x=0;x<nthreads;x++){
				if (threads[x]->type){
					threads[x]->terminate();
					x--;
				};
			};
		}
		else{
			while (nthreads>0) threads[0]->terminate();
		};
	};
}
//---------------------------------------------------------------------------
MTLock::MTLock()
{
	mtmemzero(&critical,sizeof(critical));
	InitializeCriticalSection(&critical);
}

bool MTLock::lock(int timeout)
{
	if ((timeout) && (tryenter)){
		while (--timeout>=0){
			if (tryenter(&critical)) return true;
			Sleep(1);
		};
		if (tryenter(&critical)) return true;
	};
	EnterCriticalSection(&critical);
	return true;
}

void MTLock::unlock()
{
	LeaveCriticalSection(&critical);
}
//---------------------------------------------------------------------------
MTEvent::MTEvent(bool autoreset,int interval,int resolution,bool periodic,bool pulse)
{
	event = CreateEvent(0,!autoreset,false,0);
	if (interval){
		timer = timeSetEvent(interval,resolution,(LPTIMECALLBACK)event,(int)this,((periodic)?TIME_PERIODIC:TIME_ONESHOT)|((pulse)?TIME_CALLBACK_EVENT_PULSE:TIME_CALLBACK_EVENT_SET));
	}
	else timer = 0;
}

MTEvent::MTEvent():
event(0),
timer(0)
{
}

MTEvent::~MTEvent()
{
	if (timer){
		timeKillEvent(timer);
	};
	if (event){
		CloseHandle(event);
	};
}

bool MTEvent::pulse()
{
	return PulseEvent(event)!=0;
}

bool MTEvent::set()
{
	return SetEvent(event)!=0;
}

bool MTEvent::reset()
{
	return ResetEvent(event)!=0;
}

bool MTEvent::wait(int timeout)
{
	if (!event) return false;
	if ((gi) && (mtissysthread())){
		int now = mtsyscounter();
		if (WaitForSingleObject(event,10)==WAIT_OBJECT_0) return true;
		while (gi->processmessages(false)>=0){
			if (WaitForSingleObject(event,10)==WAIT_OBJECT_0) return true;
			if ((timeout>=0) && (mtsyscounter()-now>timeout)) break;
		};
		return false;
	}
	else return (WaitForSingleObject(event,timeout)==WAIT_OBJECT_0);
}

void *MTEvent::gethandle()
{
	return event;
}
//---------------------------------------------------------------------------
MTThread::MTThread(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name):
MTEvent(),
type(0),
result(0),
terminated(false),
mproc(proc),
mname(0),
mautofree(autofree),
running(false),
mparam(param),
mpriority(priority)
{
	if (name) mname = name;
	threadlock->lock();
	if (nthreads==nathreads){
		nathreads += 4;
		if (threads) threads = (MTThread**)mtmemrealloc(threads,4*nathreads);
		else threads = (MTThread**)mtmemalloc(4*nathreads);
	};
	threads[nthreads++] = this;
	if (autostart) start();
	threadlock->unlock();
}

MTThread::MTThread():
MTEvent(),
type(-1),
result(0),
terminated(false),
mproc(0),
mname("User"),
mautofree(false),
running(false),
hasmsg(false),
mparam(0),
mpriority(0)
{
	systhread = this;
	event = GetCurrentThread();
	id = GetCurrentThreadId();
}

MTThread::~MTThread()
{
	unsigned int x,y;
	bool isrunning;

	terminated = true;
	threadlock->lock();
	mautofree = false;
	isrunning = running;
	running = false;
	threadlock->unlock();
	if (isrunning){
		if (!wait(10000)){
			LOGD("%s - [System] ERROR: Thread timeout!"NL);
		};
	};
	threadlock->lock();
	if (this==systhread){
		systhread = 0;
	}
	else{
		for (x=0;x<nthreads;x++){
			if (threads[x]==this){
				for (y=x;y<nthreads-1;y++) threads[y] = threads[y+1];
				threads[--nthreads] = 0;
				break;
			};
		};
		if (nthreads<nathreads-4){
			nathreads -= 4;
			if (nathreads<=0){
				mtmemfree(threads);
				threads = 0;
				nathreads = 0;
			}
			else threads = (MTThread**)mtmemrealloc(threads,4*nathreads);
		};
	};
	threadlock->unlock();
}

bool MTThread::pulse()
{
	return false;
}

bool MTThread::set()
{
	return false;
}

bool MTThread::reset()
{
	return false;
}

void MTThread::start()
{
	event = CreateThread(0,0,(LPTHREAD_START_ROUTINE)SysThread,this,0,(LPDWORD)&id);
	running = true;
}

bool MTThread::getmessage(int &msg,int &param1,int &param2,bool wait)
{
	MSG cmsg;

	if (wait){
		hasmsg = true;
		if (GetMessage(&cmsg,0,0,0)>0){
			msg = cmsg.message;
			param1 = cmsg.wParam;
			param2 = cmsg.lParam;
			return true;
		};
		return false;
	};
	if (PeekMessage(&cmsg,0,0,0,PM_REMOVE)){
		msg = cmsg.message;
		param1 = cmsg.wParam;
		param2 = cmsg.lParam;
		return true;
	};
	return false;
}

void MTThread::postmessage(int msg,int param1,int param2)
{
	PostThreadMessage(id,msg,param1,param2);
}

void MTThread::suspend()
{
	running = false;
	SuspendThread(event);
}

void MTThread::resume()
{
	ResumeThread(event);
	running = true;
}

void MTThread::terminate()
{
	bool isrunning;

	threadlock->lock();
	terminated = true;
	mautofree = false;
	isrunning = running;
	running = false;
	threadlock->unlock();
	if (isrunning){
		if (hasmsg) PostThreadMessage(id,WM_QUIT,0,0);
		if (!wait(10000)){
			LOGD("%s - [System] ERROR: Thread timeout!"NL);
		};
	};
	delete this;
}

DWORD WINAPI MTThread::SysThread(MTThread *thread)
{
	int x;
	bool autofree;
	
	mtsetprivatedata(-8,thread);
	switch (thread->mpriority){
	case MTT_IDLE:
		x = THREAD_PRIORITY_IDLE;
		break;
	case MTT_LOW:
		x = THREAD_PRIORITY_LOWEST;
		break;
	case MTT_LOWER:
		x = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case MTT_HIGHER:
		x = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case MTT_HIGH:
		x = THREAD_PRIORITY_HIGHEST;
		break;
	case MTT_REALTIME:
		x = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	default:
		x = THREAD_PRIORITY_NORMAL;
		break;
	};
	SetThreadPriority(GetCurrentThread(),x);
	MTTRY{
#ifdef _DEBUG
		char *callstack = (char*)mtmemalloc(MAX_STACK,MTM_ZERO);
		mtsetprivatedata(-5,callstack);
		mtsetprivatedata(-4,callstack+MAX_STACK-1);
		mtsetprivatedata(-3,0);
#endif
		x = thread->mproc(thread,thread->mparam);
#ifdef _DEBUG
		mtmemfree(mtgetprivatedata(-5));
#endif
	}
	MTCATCH{
		x = -1;
	};
	threadlock->lock();
	autofree = thread->mautofree;
	if (autofree) thread->mautofree = false;
	thread->running = false;
	threadlock->unlock();
	if (thread->type){
		MTProcess *p = (MTProcess*)thread;
		p->setprogress(-1.0);
		if (autofree) delete thread;
	}
	else if (autofree) delete thread;
	return x;
}
//---------------------------------------------------------------------------
MTProcess::MTProcess(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name):
status(MTPS_INIT),
MTThread(tproc,true,false,param,priority,name),
guidata(0),
mpproc(pproc)
{
	unsigned int x;
	MTDesktop *dsk;
	int size;
	
	this->type = type;
	this->data = data;
	setprogress(0.0);
	threadlock->lock();
	status = MTPS_WAITING;
	for (x=0;x<nthreads;x++){
		if ((threads[x]!=this) && (threads[x]->type)){
			MTProcess &cprocess = *(MTProcess*)threads[x];
			if ((cprocess.status==MTPS_WORKING) && ((cprocess.type>>16)==(type>>16))){
				threadlock->unlock();
				return;
			};
		};
	};
	if ((!silent) && (sysres) && ((dsk = (MTDesktop*)di->getdefaultdesktop())) && ((dsk->flags & MTCF_HIDDEN)==0)){
		MTFile *wf = sysres->getresourcefile('NWTM',MTW_tasks,&size);
		if (wf){
			MTWindow *tasks = gi->loadwindowfromfile(wf,size,dsk);
			tasks->flags |= MTCF_FREEONCLOSE;
			guidata = tasks;
			tasks->switchflags(MTCF_HIDDEN,false);
			sysres->releaseresourcefile(wf);
		};
	};
	start();
	threadlock->unlock();
}

MTProcess::~MTProcess()
{
	unsigned int x;
	bool isrunning;

	terminated = true;
	threadlock->lock();
	mautofree = false;
	isrunning = running;
	running = false;
	threadlock->unlock();
	if (isrunning){
		if (!wait(10000)){
			LOGD("%s - [System] ERROR: Thread timeout!"NL);
		};
	};
	if (progress!=-1.0) setprogress(-1.0);
	threadlock->lock();
	status = MTPS_FINISHED;
	for (x=0;x<nthreads;x++){
		if ((threads[x]!=this) && (threads[x]->type)){
			MTProcess &cprocess = *(MTProcess*)threads[x];
			if ((cprocess.status==MTPS_WAITING) && ((cprocess.type>>16)==(type>>16))){
				cprocess.start();
				break;
			};
		};
	};
	threadlock->unlock();
}

void MTProcess::start()
{
	status = MTPS_WORKING;
	MTThread::start();
}

int MTProcess::syncprocessproc(MTSync *s)
{
	MTProcess *p = (MTProcess*)s->param[0];
	float f;
	MTCMessage msg;
	
	f = *(float*)&s->param[1];
	if (p->guidata){
		MTWindow *wnd = (MTWindow*)p->guidata;
		mtmemzero(&msg,sizeof(msg));
		if (f<0){
			msg.msg = MTCM_CLOSE;
			msg.ctrl = wnd;
			wnd->message(msg);
		}
		else{
			MTProgress *progress = (MTProgress*)wnd->getcontrolfromuid(MTC_progress);
			if (progress){
				progress->setposition((int)(f*100.0));
			};
		};
	};
	if (p->mpproc) p->mpproc(p,p->mparam,f);
	return 0;
}

void MTProcess::setprogress(float p)
{
	progress = p;
	if (gi){
		MTSync sync;
		mtmemzero(&sync,sizeof(sync));
		sync.proc = syncprocessproc;
		sync.param[0] = (int)this;
		sync.param[1] = *(int*)&p;
		gi->synchronize(&sync);
	}
	else if (mpproc) mpproc(this,mparam,p);
}
//---------------------------------------------------------------------------
MTTimer::MTTimer(int interval,int resolution,bool periodic,int param,TimerProc proc):
res(resolution),
mparam(param),
mproc(proc)
{
	timeBeginPeriod(res);
	id = timeSetEvent(interval,res,WinTimerProc,(DWORD)this,(periodic)?TIME_PERIODIC:TIME_ONESHOT);
}

MTTimer::MTTimer(int interval,int resolution,bool periodic,MTEvent *event,bool pulse):
res(resolution)
{
	timeBeginPeriod(res);
	id = timeSetEvent(interval,res,(LPTIMECALLBACK)event->gethandle(),0,((periodic)?TIME_PERIODIC:TIME_ONESHOT)|((pulse)?TIME_CALLBACK_EVENT_PULSE:TIME_CALLBACK_EVENT_SET));
}

MTTimer::~MTTimer()
{
	timeKillEvent(id);
	timeEndPeriod(res);
}

void CALLBACK MTTimer::WinTimerProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	MTTimer &ctimer = *(MTTimer*)dwUser;
	ctimer.mproc(&ctimer,ctimer.mparam);
}
//---------------------------------------------------------------------------
MTCPUMonitor::MTCPUMonitor(int ncounters):
flushid(0)
{
	int x;

	n = ((ncounters+3)>>2)<<2;
	state = (MTCPUState*)mtmemalloc(n*sizeof(MTCPUState),MTM_ZERO);
	for (x=0;x<ncounters;x++) state[x].used = true;
}

MTCPUMonitor::~MTCPUMonitor()
{
	mtmemfree(state);
}

void MTCPUMonitor::startslice(int id)
{
	register int x;
	double start;

	mtsyscounterex(&start);
	if (id>=0){
		register MTCPUState &cstate = state[id];
		cstate.lasttime = cstate.starttime;
		cstate.starttime = start;
		if (cstate.divider==0.0) cstate.count = 0.0;
	}
	else{
		for (x=0;x<n;x++){
			register MTCPUState &cstate = state[x];
			if (!cstate.used) continue;
			cstate.lasttime = cstate.starttime;
			cstate.starttime = start;
			if (cstate.divider==0.0) cstate.count = 0.0;
		};
	};
}

void MTCPUMonitor::endslice(int id)
{
	register int x,s,e;
	double end;

	mtsyscounterex(&end);
	if (id<0){
		s = 0;
		e = n;
	}
	else{
		s = id;
		e = id+1;
	};
	for (x=s;x<e;x++){
		register MTCPUState &cstate = state[x];
		if (!cstate.used) continue;
		if (cstate.counting){
			cstate.counting = false;
			cstate.count += end-cstate.start;
		};
		if (cstate.lasttime>0.0){
			cstate.divider += (cstate.starttime-cstate.lasttime);
		};
	};
}

void MTCPUMonitor::flushcpu(int id)
{
	register int x,s,e;
	double end;

	mtsyscounterex(&end);
	if (id<0){
		s = 0;
		e = n;
	}
	else{
		s = id;
		e = id+1;
	};
	for (x=s;x<e;x++){
		register MTCPUState &cstate = state[x];
		if (!cstate.used) continue;
		if (cstate.counting){
			cstate.counting = false;
			cstate.count += end-cstate.start;
		};
		if (cstate.lasttime>0.0){
			cstate.divider += (cstate.starttime-cstate.lasttime);
			cstate.cpu = cstate.count/cstate.divider;
			cstate.divider = 0.0;
			cstate.lasttime = 0.0;
		};
	};
	flushid++;
}

void MTCPUMonitor::startadd(int id)
{
	register MTCPUState &cstate = state[id];

	if (cstate.counting) return;
	cstate.counting = true;
	mtsyscounterex(&cstate.start);
}

void MTCPUMonitor::endadd(int id)
{
	double end;
	register MTCPUState &cstate = state[id];

	if (!cstate.counting) return;
	cstate.counting = false;
	mtsyscounterex(&end);
	cstate.count += end-cstate.start;
}

double MTCPUMonitor::getcpu(int id)
{
	return state[id].cpu;
}

void* MTCPUMonitor::getcpustate(int id)
{
	if (id>=n) return 0;
	return &state[id];
}

int MTCPUMonitor::getcpustateid(void *s)
{
	int x;

	for (x=0;x<n;x++){
		if (&state[x]==state) return x;
	};
	return -1;
}

int MTCPUMonitor::addcpustate()
{
	int x;

	for (x=0;x<n;x++){
		if (!state[x].used){
			mtmemzero(&state[x],sizeof(MTCPUState));
			state[x].used = true;
			return x;
		};
	};
	x = n;
	n += 4;
	state = (MTCPUState*)mtmemrealloc(state,n*sizeof(MTCPUState));
	return x;
}

void MTCPUMonitor::delcpustate(int id)
{
	int x,cn;

	state[id].used = false;
	for (x=n-1;x>=0;x--){
		if (state[x].used){
			cn = ((x+4)>>2)<<2;
			if (cn<n){
				n = cn;
				state = (MTCPUState*)mtmemrealloc(state,n*sizeof(MTCPUState));
				return;
			};
		};
	};
}
//---------------------------------------------------------------------------
