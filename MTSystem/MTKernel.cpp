//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32,Linux
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTKernel.h"
#include "MTXSystem2.h"
#include "../../debug/Interface/MTSystemRES.h"
#ifdef _WIN32
	#include <mmsystem.h>
#else
	#include <pthread.h>
	#include <stdlib.h>
	#include <sys/select.h>
#endif
//---------------------------------------------------------------------------
MTThread *systhread;
unsigned int nthreads,nathreads;
MTThread **threads;
MTLock *threadlock;
bool pf;
int privateid[16];
#ifdef _WIN32
	double sysfrequency;
	BOOL (WINAPI *tryenter)(LPCRITICAL_SECTION lpCriticalSection);
#endif
//---------------------------------------------------------------------------
MTThread* mtgetsysthread()
{
	return systhread;
}

MTThread* mtgetcurrentthread()
{
	return (MTThread*)mtlocalget(privateid[0]);
}

bool mtissysthread()
{
	#ifdef _WIN32
		return (GetCurrentThreadId()==systhread->id);
	#else
		return (pthread_self()==systhread->id);
	#endif
}

bool mtsetprivatedata(int id,void *data)
{
	return mtlocalset(privateid[id+8],data);
}

void* mtgetprivatedata(int id)
{
	return mtlocalget(privateid[id+8]);
}

int mtlocalalloc()
{
	#ifdef _WIN32
		return TlsAlloc();
	#else
		pthread_key_t key;
		if (pthread_key_create(&key,0)) return -1;
		return (int)key;
	#endif
}

bool mtlocalfree(int id)
{
	#ifdef _WIN32
		return TlsFree(id);
	#else
		return (pthread_key_delete((pthread_key_t)id)==0);
	#endif
}

bool mtlocalset(int id,void *value)
{
	#ifdef _WIN32
		return TlsSetValue(id,value);
	#else
		return (pthread_setspecific((pthread_key_t)id,value)==0);
	#endif
}

void* mtlocalget(int id)
{
	#ifdef _WIN32
		return TlsGetValue(id);
	#else
		return pthread_getspecific((pthread_key_t)id);
	#endif
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
	#ifdef _WIN32
		return GetTickCount();
	#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC,&ts);
		return (ts.tv_sec*1000)+(ts.tv_nsec/1000);
	#endif
}

bool mtsyscounterex(double *count)
{
	#ifdef _WIN32
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
	#else
		struct timespec ts;

		clock_gettime(CLOCK_MONOTONIC,&ts);
		return double(ts.tv_sec)+double(ts.tv_nsec)/1000000.0;
	#endif
}

void mtsyswait(int ms)
{
	#ifdef _WIN32
		Sleep(ms);
	#else
		struct timespec ts = {ms/1000,(ms*1000)%1000000};
		nanosleep(&ts,0);
	#endif
}

int mtsyswaitmultiple(int count,MTEvent **events,bool all,int timeout)
{
	int x,res;

	if (count<=0) return -1;
	#ifdef _WIN32
		HANDLE *h = (HANDLE*)mtmemalloc(sizeof(HANDLE)*count);
		for (x=0;x<count;x++) h[x] = events[x]->event;
		x = WaitForMultipleObjects(count,h,all,timeout);
		if (x==WAIT_TIMEOUT) res = -1;
		else res = x-WAIT_OBJECT_0;
		mtmemfree(h);
	#else
		struct timespec to_time,delta_time;
		_mutex_cond *p,*common_mutex_cond;
		_le *q;
		_mutex_cond **mutex_cond;
		_le **le;

		mutex_cond = (_mutex_cond**)malloc(sizeof(_mutex_cond*)*count);
		le = (_le**)malloc(sizeof(_le*)*count);
		if (timeout!=-1){
			to_time.tv_sec = timeout/1000;
			to_time.tv_nsec = (timeout*1000)%1000000;
		};
		if (all){
			res = 0;
			for (x=0;x<count;x++){
				p = (_mutex_cond*)malloc(sizeof(_mutex_cond));
				pthread_mutex_init(&p->i_mutex,0);
				pthread_cond_init(&p->i_cv,0);
				q = (_le*)malloc(sizeof(_le));
				mutex_cond[x] = p;
				q->i_mutex_cond = p;
				le[x] = q;
				events[x]->_add(q);
			};
			for (x=0;x<count;x++){
				pthread_mutex_lock(&mutex_cond[x]->i_mutex);
//				while (!events[x]->signaled){
				if (!events[x]->signaled){
					if (timeout!=-1){
						if (pthread_cond_timedwait(&mutex_cond[x]->i_cv,&mutex_cond[x]->i_mutex,&to_time)==ETIMEDOUT){
							res = -1;
							break;
						};
					}
					else{
						pthread_cond_wait(&mutex_cond[x]->i_cv,&mutex_cond[x]->i_mutex);
					};
				};
				pthread_mutex_unlock(&mutex_cond[x]->i_mutex);
				if (res==-1) break;
			};
			for (x=0;x<count;x++){
				events[x]->_del(le[x]);
			};
		}
		else{
			res = -1;
			common_mutex_cond = (_mutex_cond*)malloc(sizeof(_mutex_cond));
			pthread_mutex_init(&common_mutex_cond->i_mutex,0);
			pthread_cond_init(&common_mutex_cond->i_cv,0);
			for (x=0;x<count;x++){
				q = (_le*)malloc(sizeof(_le));
				q->i_mutex_cond = common_mutex_cond;
				le[x] = q;
				events[x]->_add(q);
			};
			pthread_mutex_lock(&common_mutex_cond->i_mutex);
			for (x=0;x<count;x++){
				if (events[x]->signaled){
					res = 0;
					break;
				};
			};
			if (res==-1){
				if (timeout!=-1){
					if (pthread_cond_timedwait(&common_mutex_cond->i_cv,&common_mutex_cond->i_mutex,&to_time)!=ETIMEDOUT){
						res = 0;
					};
				}
				else{
					pthread_cond_wait(&common_mutex_cond->i_cv,&common_mutex_cond->i_mutex);
					res = 0;
				};
			};
			pthread_mutex_unlock(&mutex_cond[x]->i_mutex);
			for (x=0;x<count;x++){
				events[x]->_del(le[x]);
			};
		};
		free(mutex_cond);
		free(le);
	#endif
	return res;
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

	for (x=0;x<16;x++) privateid[x] = mtlocalalloc();
	#ifdef _WIN32
		mt_int64 intf;
		pf = QueryPerformanceFrequency((LARGE_INTEGER*)&intf)!=0;
		if (pf) sysfrequency = (double)intf;
		else sysfrequency = 1000;
	#endif
	threadlock = new MTLock();
	mtsetprivatedata(-8,new MTThread());
	#ifdef _DEBUG
		char *callstack = (char*)mtmemalloc(MAX_STACK,MTM_ZERO);
		mtsetprivatedata(-5,callstack);
		mtsetprivatedata(-4,callstack+MAX_STACK-1);
		mtsetprivatedata(-3,0);
	#endif
	#ifdef _WIN32
		HMODULE hkernel = GetModuleHandle("KERNEL32.DLL");
		*(int*)&tryenter = (int)GetProcAddress(hkernel,"TryEnterCriticalSection");
	#endif
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
	for (x=0;x<16;x++) mtlocalfree(privateid[x]);
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
	#ifdef _WIN32
		mtmemzero(&critical,sizeof(critical));
		InitializeCriticalSection(&critical);
	#else
		pthread_mutex_init(&mutex,0);
	#endif
}

MTLock::~MTLock()
{
	#ifdef _WIN32
		DeleteCriticalSection(&critical);
	#else
		pthread_mutex_destroy(&mutex);
	#endif
}

bool MTLock::lock(int timeout)
{
	#ifdef _WIN32
		if ((timeout) && (tryenter)){
			while (--timeout>=0){
				if (tryenter(&critical)) return true;
				Sleep(1);
			};
			if (tryenter(&critical)) return true;
			return false;
		};
		EnterCriticalSection(&critical);
		return true;
	#else
		if (timeout){
			struct timespec ts = {0,1000};
			while (--timeout>=0){
				if (pthread_mutex_trylock(&mutex)==0) return true;
				nanosleep(&ts,0);
			};
			if (pthread_mutex_trylock(&mutex)==0) return true;
			return false;
		}
		else{
			return (pthread_mutex_lock(&mutex)==0);
		};
	#endif
}

void MTLock::unlock()
{
	#ifdef _WIN32
		LeaveCriticalSection(&critical);
	#else
		pthread_mutex_unlock(&mutex);
	#endif
}
//---------------------------------------------------------------------------
MTEvent::MTEvent(bool autoreset,int interval,int resolution,bool periodic,bool pulse)
{
	#ifdef _WIN32
		event = CreateEvent(0,!autoreset,false,0);
		if (interval){
			timer = timeSetEvent(interval,resolution,(LPTIMECALLBACK)event,(int)this,((periodic)?TIME_PERIODIC:TIME_ONESHOT)|((pulse)?TIME_CALLBACK_EVENT_PULSE:TIME_CALLBACK_EVENT_SET));
		}
		else timer = 0;
	#else
		signaled = false;
		needreset = autoreset;
		pthread_mutex_init(&i_mutex,0);
		start = end = 0;
	#endif
}

MTEvent::MTEvent():
#ifdef _WIN32
event(0),
timer(0)
#else
signaled(false),
needreset(false)
#endif
{
	#ifndef _WIN32
		pthread_mutex_init(&i_mutex,0);
		start = end = 0;
	#endif
}

MTEvent::~MTEvent()
{
	#ifdef _WIN32
		if (timer){
			timeKillEvent(timer);
		};
		if (event){
			CloseHandle(event);
		};
	#else
		pthread_mutex_destroy(&i_mutex);
	#endif
}

bool MTEvent::pulse()
{
	#ifdef _WIN32
		return PulseEvent(event)!=0;
	#else
		_le *p;
		signaled = true;
		if (start==0) return true;
		p = start;
		do{
			pthread_mutex_lock(&p->i_mutex_cond->i_mutex);
			p = p->next;
		} while (p!=0);
		p = start;
		do{
			pthread_cond_signal(&p->i_mutex_cond->i_cv);
			p = p->next;
		} while (p!=0);
		p = start;
		do{
			pthread_mutex_unlock(&p->i_mutex_cond->i_mutex);
			p = p->next;
		} while (p!=0);
		signaled = false;
	#endif
}

bool MTEvent::set()
{
	#ifdef _WIN32
		return SetEvent(event)!=0;
	#else
		_le *p;
		signaled = true;
		if (start==0) return true;
		p = start;
		do{
			pthread_mutex_lock(&p->i_mutex_cond->i_mutex);
			p = p->next;
		} while (p!=0);
		p = start;
		do{
			pthread_cond_signal(&p->i_mutex_cond->i_cv);
			p = p->next;
		} while (p!=0);
		p = start;
		do{
			pthread_mutex_unlock(&p->i_mutex_cond->i_mutex);
			p = p->next;
		} while (p!=0);
	#endif
}

bool MTEvent::reset()
{
	#ifdef _WIN32
		return ResetEvent(event)!=0;
	#else
		signaled = false;
	#endif
}

bool MTEvent::wait(int timeout)
{
	#ifdef _WIN32
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
	#else
		// TODO UI synchronization support
		MTEvent *self = this;
		return (mtsyswaitmultiple(1,&self,true,timeout)>=0);
	#endif
}

#ifndef _WIN32
void MTEvent::_add(_le *list)
{
	pthread_mutex_lock(&i_mutex);
	if (start==0){
		start = list;
		list->next = 0;
		list->prev = 0;
		end = list;
	}
	else{
		end->next = list;
		list->prev = end;
		list->next = 0;
		end = list;
	};
	pthread_mutex_unlock(&i_mutex);
}

void MTEvent::_del(_le *list)
{
	pthread_mutex_lock(&i_mutex);
	if ((start==list) && (end==list)){
		start = end = 0;
	}
	else if (start==list){
		start = list->next;
		start->prev = 0;
	}
	else if (end==list){
		end = list->prev;
		end->next = 0;
	}
	else{
		list->prev->next = list->next;
		list->next->prev = list->prev;
	};
	free(list);
	pthread_mutex_unlock(&i_mutex);
}
#endif
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
	#ifndef _WIN32
		bool fifo = false;
		struct sched_param rt_param;
		pthread_attr_init(&attr);
		memset(&rt_param,0,sizeof(rt_param));
		switch (priority){
		case MTT_HIGHER:
			rt_param.sched_priority = 1;
			fifo = true;
			break;
		case MTT_HIGH:
			rt_param.sched_priority = 10;
			fifo = true;
			break;
		case MTT_REALTIME:
			rt_param.sched_priority = 99;
			fifo = true;
			break;
		default:
			rt_param.sched_priority = 0;
			break;
		};
		if (fifo) pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
		pthread_attr_setschedparam(&attr,&rt_param);
		pipe(_p);
	#endif
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
	#ifdef _WIN32
		event = GetCurrentThread();
		id = GetCurrentThreadId();
	#else
		id = (mt_uint32)pthread_self();
		pthread_attr_init(&attr);
		pipe(_p);
	#endif
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
	#ifndef _WIN32
		close(_p[0]);
		close(_p[1]);
		pthread_attr_destroy(&attr);
	#endif
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
	#ifdef _WIN32
		event = CreateThread(0,0,(LPTHREAD_START_ROUTINE)SysThread,this,0,(LPDWORD)&id);
	#else
		pthread_create((pthread_t*)&id,0,SysThread,this);
	#endif
	running = true;
}

bool MTThread::getmessage(int &msg,int &param1,int &param2,bool wait)
{
	#ifdef _WIN32
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
	#else
		int cmsg[3];
		fd_set rs;

		FD_ZERO(&rs);
		FD_SET(_p[0],&rs);
		if (wait) select(1,&rs,0,0,0);
		else{
			struct timeval tv = {0,0};
			select(1,&rs,0,0,&tv);
			if (!FD_ISSET(_p[0],&rs)) return false;
		};
		if (read(_p[0],&cmsg,sizeof(cmsg))!=sizeof(cmsg)) return false;
		msg = cmsg[0];
		param1 = cmsg[1];
		param2 = cmsg[2];
		return true;
	#endif
}

void MTThread::postmessage(int msg,int param1,int param2)
{
	#ifdef _WIN32
		PostThreadMessage(id,msg,param1,param2);
	#else
		int cmsg[3];

		cmsg[0] = msg;
		cmsg[1] = param1;
		cmsg[2] = param2;
		write(_p[1],&cmsg,sizeof(cmsg));
	#endif
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
		if (hasmsg) postmessage(-1,0,0);
		if (!wait(10000)){
			LOGD("%s - [System] ERROR: Thread timeout!"NL);
		};
	};
	delete this;
}

#ifdef _WIN32
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
#else
void* MTThread::SysThread(void* param)
{
	int x;
	bool autofree;
	MTThread *thread = (MTThread*)param;

	mtsetprivatedata(-8,thread);
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
	((MTEvent*)thread)->set();
	threadlock->unlock();
	if (thread->type){
		MTProcess *p = (MTProcess*)thread;
		p->setprogress(-1.0);
		if (autofree) delete thread;
	}
	else if (autofree) delete thread;
	return (void*)x;
}
#endif
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
		MTFile *wf = sysres->getresourcefile(MTR_WINDOW,MTW_tasks,&size);
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
	#ifdef _WIN32
		timeBeginPeriod(res);
		id = timeSetEvent(interval,res,WinTimerProc,(DWORD)this,(periodic)?TIME_PERIODIC:TIME_ONESHOT);
	#else
		struct sigevent se;
		se.sigev_notify = SIGEV_SIGNAL;
		se.sigev_signo = SIGALRM;
		se.sigev_value.sival_ptr = this;
		se.sigev_notify_function = LinuxTimerProc;
		se.sigev_notify_attributes = 0;
		timer_create(CLOCK_REALTIME,&se,(timer_t*)&id);
	#endif
}

MTTimer::MTTimer(int interval,int resolution,bool periodic,MTEvent *event,bool pulse):
res(resolution)
{
	#ifdef _WIN32
		timeBeginPeriod(res);
		id = timeSetEvent(interval,res,(LPTIMECALLBACK)event->event,0,((periodic)?TIME_PERIODIC:TIME_ONESHOT)|((pulse)?TIME_CALLBACK_EVENT_PULSE:TIME_CALLBACK_EVENT_SET));
	#else
		// TODO
	#endif
}

MTTimer::~MTTimer()
{
	#ifdef _WIN32
		timeKillEvent(id);
		timeEndPeriod(res);
	#else
		timer_delete((timer_t)id);
	#endif
}

#ifdef _WIN32
void CALLBACK MTTimer::WinTimerProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	MTTimer &ctimer = *(MTTimer*)dwUser;
	ctimer.mproc(&ctimer,ctimer.mparam);
}
#else
void MTTimer::LinuxTimerProc(sigval timer)
{
	MTTimer &ctimer = *(MTTimer*)timer.sival_ptr;
	ctimer.mproc(&ctimer,ctimer.mparam);
}
#endif
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
