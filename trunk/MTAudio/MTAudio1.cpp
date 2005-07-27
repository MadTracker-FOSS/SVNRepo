//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright � 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include "MTAudio1.h"
#include "MTAudio2.h"
#include "../Headers/MTXGUI.h"
#include "../Headers/MTXControls.h"
#include "../Headers/MTXModule.h"
#include "../Headers/MTXSystem2.h"
#include "MTWaveOut.h"
#include "MTDirectSound.h"
//---------------------------------------------------------------------------
static const char *audioname = {"MadTracker Audio Core"};
static const int audioversion = 0x30000;
static const MTXKey audiokey = {0,0,0,0};
MTXInterfaces i;
MTAudioInterface *ai;
MTInterface *mtinterface;
MTSystemInterface *si;
MTObjectsInterface *oi;
WaveOutput output;
MTWaveOutDeviceManager *womanager;
MTDirectSoundDeviceManager *dsmanager;
MTDevice *devices[32];
int ndevices;
#ifdef _DEBUG
MTFile *recf;
#endif
//---------------------------------------------------------------------------
int AudioThread(MTThread*,void* pool)
{
	double oldc,curc,intc,usec;
	
	si->setprivatedata(0,si->memalloc(sizeof(sample)*PRIVATE_BUFFER,MTM_ZERO));
	oldc = 0;
	while (ai->running){
		if (output.event->wait(1000)){
			if (!output.ndevices) continue;
			si->syscounterex(&curc);
			if (oldc)
				intc = curc-oldc;
			else
				intc = -1;
			oldc = curc;
			output.lock->lock();
			MTTRY{
				generateoutput();
			}
			MTCATCH{
			};
			output.lock->unlock();
			si->syscounterex(&usec);
			usec -= curc;
		};
	};
	si->memfree(si->getprivatedata(0));
	return 0;
}
//---------------------------------------------------------------------------
MTAudioInterface::MTAudioInterface():
thread(0),
running(true)
{
	type = audiotype;
	key = &audiokey;
	name = audioname;
	version = audioversion;
	status = 0;
}

bool MTAudioInterface::init()
{
	int x;
	MTConfigFile *conf;
	char device[64];
	
	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	oi = (MTObjectsInterface*)mtinterface->getinterface(objectstype);
	if ((!si) || (!oi)) return false;
	ENTER("MTAudioInterface::init");
	LOGD("%s - [Audio] Initializing..."NL);
	mtmemzero(&output,sizeof(output));
	output.lock = si->lockcreate();
	output.event = si->eventcreate(true);
	output.frequency = 44100;
	output.latency = 50.0;
	output.interval = 50.0;
	output.mincpu = 0.0;
	output.maxcpu = 0.75;
	womanager = new MTWaveOutDeviceManager();
	dsmanager = new MTDirectSoundDeviceManager();
	adddevicemanager(womanager);
	adddevicemanager(dsmanager);
	if ((conf = (MTConfigFile*)mtinterface->getconf("Global",false))){
		if (conf->setsection("MTAudio")){
			conf->getparameter("Frequency",&output.frequency,MTCT_SINTEGER,sizeof(output.frequency));
			conf->getparameter("Latency",&output.latency,MTCT_FLOAT,sizeof(output.latency));
			conf->getparameter("Interval",&output.interval,MTCT_FLOAT,sizeof(output.interval));
			conf->getparameter("MinCPU",&output.mincpu,MTCT_FLOAT,sizeof(output.mincpu));
			conf->getparameter("MaxCPU",&output.maxcpu,MTCT_FLOAT,sizeof(output.maxcpu));
			if (output.maxcpu>0.9f) output.maxcpu = 0.9f;
			if (output.maxcpu<0.1f) output.maxcpu = 0.1f;
			if (output.mincpu>output.maxcpu) output.mincpu = output.maxcpu -0.1f;
			if (output.mincpu<0.0f) output.mincpu = 0.0f;
			if (conf->getparameter("Device",device,MTCT_STRING,sizeof(device))){
				unselectdevices();
				for (x=0;x<ndevices;x++){
					if (stricmp(devices[x]->devicename,device)==0){
						selectdevice(x);
						break;
					};
				};
			};
		};
		mtinterface->releaseconf(conf);
	};
	LOGD("%s - [Audio] Starting audio thread..."NL);
	running = true;
	thread = si->threadcreate(AudioThread,false,true,this,MTT_REALTIME,"Audio");
	status |= MTX_INITIALIZED;
	LEAVE();
	return true;
}

void MTAudioInterface::uninit()
{
	ENTER("MTAudioInterface::uninit");
#ifdef _DEBUG
	if (recf){
		si->fileclose(recf);
		recf = 0;
	};
#endif
	LOGD("%s - [Audio] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	LOGD("%s - [Audio] Stopping audio thread..."NL);
	running = false;
	if (thread){
		if (output.event) output.event->set();
		thread->terminate();
		thread = 0;
	};
	LOGD("%s - [Audio] Freeing all devices..."NL);
	deactivatedevices();
	deldevicemanager(womanager);
	deldevicemanager(dsmanager);
	delete womanager;
	delete dsmanager;
	si->lockdelete(output.lock);
	si->eventdelete(output.event);
	LEAVE();
}

#ifdef _DEBUG
void menurecord(MTShortcut *s,MTControl *c,MTUndo*)
{
	MTMenuItem *item = (MTMenuItem*)c;
	char file[512];

	if ((!c) || (c->guiid!=MTC_MENUITEM)) return;
	MTTRY{
		output.lock->lock();
		if (item->tag==0){
			item->setcaption("Stop Recording");
			item->tag = 1;
			strcpy(file,mtinterface->getprefs()->syspath[SP_ROOT]);
			strcat(file,"Record.raw");
			recf = si->fileopen(file,MTF_WRITE|MTF_SHAREREAD|MTF_CREATE);
		}
		else{
			si->fileclose(recf);
			recf = 0;
			item->setcaption("Start Recording");
			item->tag = 0;
		};
	}
	MTCATCH{
	};
	output.lock->unlock();
}
#endif

void MTAudioInterface::start()
{
#ifdef _DEBUG
	MTDesktop *dsk;
	MTMenuItem *item;
	MTGUIInterface *gi = (MTGUIInterface*)mtinterface->getinterface(guitype);
	if (gi){
		dsk = gi->getdesktop(0);
		if (dsk){
			dsk->popup->additem("|MTAudio",0,0,false,0);
			item = (MTMenuItem*)dsk->popup->additem("Start Recording",23,0,false,0);
			item->tag = 0;
			item->command = menurecord;
		};
	};
#endif
}

void MTAudioInterface::stop()
{

}

void MTAudioInterface::processcmdline(void *params)
{

}

void MTAudioInterface::showusage(void *out)
{

}

int MTAudioInterface::config(int command,int param)
{
	return 0;
}


int MTAudioInterface::getnumdevices()
{
	return ndevices;
}

const char* MTAudioInterface::getdevicename(int id)
{
	if ((id<0) || (id>=ndevices)) return 0;
	return devices[id]->devicename;
}

void MTAudioInterface::selectdevice(int id)
{
	if ((id<0) || (id>=ndevices)) return;
	devices[id]->selected = true;
}

void MTAudioInterface::unselectdevices()
{
	int x;

	for (x=0;x<ndevices;x++) devices[x]->selected = false;
}

void MTAudioInterface::activatedevices()
{
	int x,y;
	bool primary = true;
	bool ok = true;
	
	ENTER("MTAudioInterface::activatedevices");
	MTTRY{
		output.lock->lock();
		output.ndevices = 0;
		output.sync = 0;
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *module = (MTModule*)mtinterface->getmodule(--x);
			module->disabletracks();
		};
	}
	MTCATCH{
	};
	output.lock->unlock();
	mtmemzero(&output.device,sizeof(output.device));
	output.timer = 0;
	output.buffersamples = (int)(output.interval*output.frequency/1000+3) & (~3);
	y = 0;
	for (x=0;x<ndevices;x++){
		if (devices[x]->selected){
			MTAudioDevice *cdev = devices[x]->manager->newdevice(devices[x]->id);
			if (!cdev) continue;
			if (cdev->init((float)output.frequency,2,16,output.latency)){
				WaveDevice &cwdev = *mtnew(WaveDevice);
				output.device[y++] = &cwdev;
				cwdev.name = (char*)si->memalloc(strlen(devices[x]->devicename)+1);
				strcpy(cwdev.name,devices[x]->devicename);
				cwdev.device = cdev;
				cwdev.manager = devices[x]->manager;
				cwdev.bits = 16;
				cwdev.nchannels = 2;
				cwdev.datasamples = cdev->datasamples;
				cdev->getposition();
			};
		};
	};
	MTTRY{
		output.lock->lock();
		output.ndevices = y;
		if (y){
			output.timer = si->timerevent((int)output.interval,0,true,output.event,true);
			if (!output.timer){
				si->showlastoserror();
				return;
			};
			output.buffersamples = (int)(output.interval*output.frequency/1000+3) & (~3);
			if (y>1) output.sync = 1;
			for (x=0;x<y;x++) output.device[x]->master = (Track*)oi->newobject(MTO_TRACK,0,x);
			for (x=0;x<y;x++){
				WaveDevice &cdev = *output.device[x];
				cdev.device->play();
				si->syscounterex(&cdev.timeopened);
			};
		};
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *module = (MTModule*)mtinterface->getmodule(--x);
			module->enabletracks();
		};
	}
	MTCATCH{
	};
	output.lock->unlock();
	LEAVE();
}

void MTAudioInterface::deactivatedevices()
{
	int x,y;
	bool ok = true;
	
	ENTER("MTAudioInterface::deactivatedevices");
	if (output.timer){
		si->timerdelete(output.timer);
		output.timer = 0;
	};
	MTTRY{
		output.lock->lock();
		y = output.ndevices;
		output.ndevices = 0;
		for (x=0;x<y;x++) oi->deleteobject(output.device[x]->master);
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *module = (MTModule*)mtinterface->getmodule(--x);
			module->disabletracks();
		};
		for (x=0;x<y;x++){
			WaveDevice &cdev = *output.device[x];
			cdev.device->stop();
			cdev.device->uninit();
			cdev.manager->deldevice(cdev.device);
			si->memfree(cdev.name);
			si->memfree(&cdev);
			output.device[x] = 0;
		};
	}
	MTCATCH{
	};
	output.lock->unlock();
	LEAVE();
}

bool MTAudioInterface::adddevicemanager(MTAudioDeviceManager *manager)
{
	int x,y;

	if (ndevices==MAX_AUDIODEVICES) return false;
	FENTER1("MTAudioInterface::adddevicemanager(%.8X)",manager);
	y = ndevices;
	for (x=0;x<MAX_AUDIODEVICES,y<MAX_AUDIODEVICES;x++){
		if (!manager->devicename[x]) break;
		MTDevice *cdevice = mtnew(MTDevice);
		cdevice->devicename = manager->devicename[x];
		cdevice->id = x;
		cdevice->manager = manager;
		if (y==0) cdevice->selected = true;
		devices[y++] = cdevice;
	};
	ndevices = y;
	LEAVE();
	return true;
}

void MTAudioInterface::deldevicemanager(MTAudioDeviceManager *manager)
{
	int x,y;

	FENTER1("MTAudioInterface::deldevicemanager(%.8X)",manager);
	for (x=0;x<ndevices;x++){
		if (devices[x]->manager==manager){
			si->memfree(devices[x]);
			ndevices--;
			for (y=x;y<ndevices;y++) devices[y] = devices[y+1];
			devices[ndevices] = 0;
			x--;
		};
	};
	LEAVE();
}

WaveOutput* MTAudioInterface::getoutput()
{
	return &output;
}
//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* __stdcall MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!ai) ai = new MTAudioInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)ai;
	return &i;
}

}
//---------------------------------------------------------------------------