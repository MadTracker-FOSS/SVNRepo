//---------------------------------------------------------------------------
#ifndef MTAUDIO1_INCLUDED
#define MTAUDIO1_INCLUDED

#ifdef __BORLANDC__
static const int audiotype = 'XAUD';
#else
static const int audiotype = 'DUAX';
#endif

#define MAX_AUDIODEVICES 64
#define PRIVATE_BUFFER   4096

#include "../Headers/MTXSystem.h"
#include "../Headers/MTXExtension.h"
#include "../Headers/MTXObjects.h"
#include "../Headers/MTXTrack.h"
#include "MTAudioDevice.h"
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
/*
	int nbuffers;
	float bufferms;
	int buffersamples;
*/
};

struct MTDevice{
	char *devicename;
	MTAudioDeviceManager *manager;
	int id;
	bool selected;
};

class MTAudioInterface : public MTXInterface{
public:
	MTAudioInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	virtual int MTCT getnumdevices();
	virtual const char* MTCT getdevicename(int id);
	virtual void MTCT selectdevice(int id);
	virtual void MTCT unselectdevices();
	virtual void MTCT activatedevices();
	virtual void MTCT deactivatedevices();
	virtual bool MTCT adddevicemanager(MTAudioDeviceManager *manager);
	virtual void MTCT deldevicemanager(MTAudioDeviceManager *manager);
	virtual WaveOutput* MTCT getoutput();
	bool running;
private:
	MTThread *thread;
};
//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
extern MTObjectsInterface *oi;
extern WaveOutput output;
#ifdef _DEBUG
extern MTFile *recf;
#endif
//---------------------------------------------------------------------------
#endif
