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
#ifndef MTXAUDIO_INCLUDED
#define MTXAUDIO_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int audiotype = FOURCC('X','A','U','D');

#define MAX_AUDIODEVICES 256
#define PRIVATE_BUFFER   1024
//---------------------------------------------------------------------------
struct WaveDevice;
struct WaveOutput;
class MTAudioDevice;
class MTAudioDeviceManager;
class MTAudioInterface;
//---------------------------------------------------------------------------
#include "MTXSystem.h"
#include "MTXObject.h"
#include "MTXTrack.h"
//---------------------------------------------------------------------------
struct WaveDevice{
	char *name;
	MTAudioDevice *device;
	MTAudioDeviceManager *manager;
	Track *master;
	double timeopened;
	int nchannels;
	int bits;
	int datasamples;
};

struct WaveOutput{
	int ndevices;
	int sync;
	WaveDevice *device[MAX_AUDIODEVICES];
	int frequency;
	float mincpu,maxcpu;
	float latency,interval;
	int playlng;
	int buffersamples;
	MTTimer *timer;
	MTLock *lock;
	MTEvent *event;
};

class MTAudioDevice{
public:
	int datasamples;

	virtual bool MTCT init(float frequency,int nchannels,int bits,double latency) = 0;
	virtual void MTCT uninit() = 0;
	virtual bool MTCT play() = 0;
	virtual bool MTCT stop() = 0;
	virtual int MTCT getposition(bool playback = false) = 0;
	virtual bool MTCT getdata(int position,int length,void **ptr1,void **ptr2,unsigned long *lng1,unsigned long *lng2) = 0;
	virtual bool MTCT writedata(void *ptr1,void *ptr2,unsigned long lng1,unsigned long lng2) = 0;
};

class MTAudioDeviceManager{
public:
	char *devicename[16];

	virtual MTAudioDevice* MTCT newdevice(int id) = 0;
	virtual void MTCT deldevice(MTAudioDevice *device) = 0;
};

class MTAudioInterface : public MTXInterface{
public:
	bool running,recording;

	virtual int MTCT getnumdevices() = 0;
	virtual const char* MTCT getdevicename(int id) = 0;
	virtual void MTCT selectdevice(int id) = 0;
	virtual void MTCT unselectdevices() = 0;
	virtual void MTCT activatedevices() = 0;
	virtual void MTCT deactivatedevices() = 0;
	virtual bool MTCT adddevicemanager(MTAudioDeviceManager *manager) = 0;
	virtual void MTCT deldevicemanager(MTAudioDeviceManager *manager) = 0;
	virtual WaveOutput* MTCT getoutput() = 0;
	virtual void MTCT debugpoint(int offset,const char *text) = 0;
};
//---------------------------------------------------------------------------
#endif
