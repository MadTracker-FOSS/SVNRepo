//---------------------------------------------------------------------------
#ifndef MTKERNEL_INCLUDED
#define MTKERNEL_INCLUDED
//---------------------------------------------------------------------------
#define MTT_IDLE     -15
#define MTT_LOW      -2
#define MTT_LOWER    -1
#define MTT_NORMAL   0
#define MTT_HIGHER   1
#define MTT_HIGH     2
#define MTT_REALTIME 15

enum{
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

#define MAX_STACK 1024
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
class MTLock;
class MTEvent;
class MTThread;
class MTProcess;
class MTTimer;
class MTCPUMonitor;
typedef int (MTCT *ThreadProc)(MTThread *thread,void *param);
typedef void (MTCT *ProcessProc)(MTProcess *process,void *param,float p);
typedef void (MTCT *TimerProc)(MTTimer *timer,int param);
//---------------------------------------------------------------------------
#include "MTSystem1.h"
#include <windows.h>
#include "../Headers/MTXGUI.h"
//---------------------------------------------------------------------------
class MTLock{
public:
	MTLock();
	virtual bool MTCT lock(int timeout = -1);
	virtual void MTCT unlock();
private:
	CRITICAL_SECTION critical;
};

class MTEvent{
public:
	MTEvent(bool autoreset,int interval = 0,int resolution = 0,bool periodic = true,bool pulse = false);
	MTEvent();
	virtual ~MTEvent();
	virtual bool MTCT pulse();
	virtual bool MTCT set();
	virtual bool MTCT reset();
	virtual bool MTCT wait(int timeout);
	virtual void* MTCT gethandle();
protected:
	friend int MTCT mtsyswaitmultiple(int count,MTEvent **events,bool all,int timeout);
	HANDLE event;
	int timer;
};

class MTThread : public MTEvent{
public:
	unsigned int id;
	int type;
	int result;
	bool terminated;

	MTThread(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name);
	MTThread();
	~MTThread();
	bool MTCT pulse();
	bool MTCT set();
	bool MTCT reset();
	virtual void MTCT start();
	virtual bool MTCT getmessage(int &msg,int &param1,int &param2,bool wait = false);
	virtual void MTCT postmessage(int msg,int param1,int param2);
	virtual void MTCT suspend();
	virtual void MTCT resume();
	virtual void MTCT terminate();
protected:
	static DWORD WINAPI SysThread(MTThread*);
	ThreadProc mproc;
	char *mname;
	void *mparam;
	int mpriority;
	bool mautofree;
	bool running;
	bool hasmsg;
};

class MTProcess : public MTThread{
public:
	int status;
	int priority;
	void *data;
	void *guidata;
	float progress;
	ProcessProc mpproc;

	MTProcess(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name);
	virtual ~MTProcess();
	void MTCT start();
	virtual void MTCT setprogress(float p);
private:
	static int MTCT syncprocessproc(MTSync *s);
};

class MTTimer{
public:
	MTTimer(int interval,int resolution,bool periodic,int param,TimerProc proc);
	MTTimer(int interval,int resolution,bool periodic,MTEvent *event,bool pulse = false);
	virtual ~MTTimer();
private:
	static void CALLBACK WinTimerProc(UINT,UINT,DWORD,DWORD,DWORD);
	int id;
	int res;
	int mparam;
	TimerProc mproc;
};

struct MTCPUState{
	double starttime;
	double lasttime;
	double start;
	double count;
	double divider;
	double cpu;
	bool counting;
	bool used;
};

class MTCPUMonitor{
public:
	int flushid;
	MTCPUMonitor(int ncounters);
	virtual ~MTCPUMonitor();
	virtual void MTCT startslice(int id);
	virtual void MTCT endslice(int id);
	virtual void MTCT flushcpu(int id);
	virtual void MTCT startadd(int id);
	virtual void MTCT endadd(int id);
	virtual double MTCT getcpu(int id);
	virtual void* MTCT getcpustate(int id);
	virtual int MTCT getcpustateid(void *s);
	virtual int MTCT addcpustate();
	virtual void MTCT delcpustate(int id);
private:
	int n;
	MTCPUState *state;
};
//---------------------------------------------------------------------------
extern "C"
{
void initKernel();
void uninitKernel();
void stopThreads(bool processes);
//---------------------------------------------------------------------------
MTThread* MTCT mtgetsysthread();
MTThread* MTCT mtgetcurrentthread();
bool MTCT mtissysthread();
bool MTCT mtsetprivatedata(int id,void *data);
void* MTCT mtgetprivatedata(int id);
int MTCT mtlocalalloc();
bool MTCT mtlocalfree(int id);
bool MTCT mtlocalset(int id,void *value);
void* MTCT mtlocalget(int id);
MTThread* MTCT mtthreadcreate(ThreadProc proc,bool autofree,bool autostart,void *param,int priority,char *name);
MTProcess* MTCT mtprocesscreate(ThreadProc tproc,void *param,int type,int priority,void *data,ProcessProc pproc,bool silent,char *name);
MTLock* MTCT mtlockcreate();
void MTCT mtlockdelete(MTLock *lock);
MTEvent* MTCT mteventcreate(bool autoreset,int interval,int resolution,bool periodic,bool pulse);
void MTCT mteventdelete(MTEvent *event);
MTTimer* MTCT mttimercreate(int interval,int resolution,bool periodic,int param,TimerProc proc);
MTTimer* MTCT mttimerevent(int interval,int resolution,bool periodic,MTEvent *event,bool pulse);
void MTCT mttimerdelete(MTTimer *timer);
MTCPUMonitor* MTCT mtcpumonitorcreate(int ncounters);
int MTCT mtsyscounter();
bool MTCT mtsyscounterex(double *count);
void MTCT mtsyswait(int ms);
int MTCT mtsyswaitmultiple(int count,void **events,bool all,int timeout);
int MTCT mtgetlasterror();
void MTCT mtsetlasterror(int error);
}
//---------------------------------------------------------------------------
#endif
