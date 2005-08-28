//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTINSTRUMENT_INCLUDED
#define MTINSTRUMENT_INCLUDED

#define MIN_RAMP  32
#define FAST_RAMP 64
#define MAX_RAMP  128

#define IF_NEEDPREPROCESS  0x01
#define IF_NEEDPOSTPROCESS 0x02
#define IF_NEEDPREBUFFER   0x04
#define IF_NEEDPOSTBUFFER  0x08
#define IF_SAMETHREAD      0x10
#define IF_SAMECOMPUTER    0x20

#define IIF_BACKGROUND 0x001
#define IIF_SLEEPING   0x002

enum{
	MTIE_NOTE = 0,
	MTIE_KILL,
	MTIE_RESET,
	MTIE_PARAM
};

#define MTIEF_ISNOTE       0x01
#define MTIEF_ISINS        0x02
#define MTIEF_VOLUME       0x01
#define MTIEF_PITCH        0x02
#define MTIEF_ADD          0x04
#define MTIEF_MUL          0x08
#define MTIEF_DONTSATURATE 0x10

enum{
	MTIP_VOLUME = 0,
	MTIP_PANNING,
	MTIP_NOTE,
	MTIP_TREMOLO,
	MTIP_VIBRATO,
	MTIP_OFFSET,
	MTIP_PRESSURE,
	MTIP_MIDI,
	MTIP_CUSTOM
};

enum{
	MTIS_BEGIN = 0,
	MTIS_CURRENT,
	MTIS_END
};

enum{
	MTIS_BEATS = 0,
	MTIS_MS,
	MTIS_SAMPLES,
	MTIS_INNER = 16
};

class Instrument;
class InstrumentInstance;
struct MTIEvent;

#include "MTObject.h"
#include "MTModule.h"
#include "MTTrack.h"
#include "MTOscillator.h"
#include "MTFilter.h"
//---------------------------------------------------------------------------
// Instrument classes
//---------------------------------------------------------------------------
class Instrument : public MTObject{
public:
	Instrument(MTModule *parent,int type,int i):MTObject(parent,type,i){	};
	virtual ~Instrument(){	};
	
	virtual InstrumentInstance* MTCT createinstance(Track *track,PatternInstance *caller,InstrumentInstance *previous) = 0;
	virtual void MTCT preprocess(int count){  };
	virtual void MTCT postprocess(int count){  };
	virtual void MTCT prebuffer(int count){  };
	virtual void MTCT postbuffer(int count){  };
	virtual bool MTCT acceptoscillator(){ return false; };
	virtual int MTCT addoscillator(Oscillator *o){ return -1; };
	virtual int MTCT deloscillator(Oscillator *o){ return -1; };
};

struct MTIEvent{
	MTObject *source;
	int type;
	double offset;
	int flags;
	int size;
	char data[8];
};

struct MTINoteEvent{
	MTObject *source;
	int type;
	double offset;
	int flags;
	int size;
	double notelength;
	double noteoffset;
	double note;
	double noteoffvelocity;
	double gvolume;
	double volume;
	float gpanx,gpany,gpanz;
	float panx,pany,panz;
};

struct MTIParamEvent{
	MTObject *source;
	int type;
	double offset;
	int flags;
	int size;
	int param;
	union{
		struct{
			double dvalue1;
			double dvalue2;
		};
		struct{
			int ivalue1;
			int ivalue2;
			int ivalue3;
			int ivalue4;
		};
		struct{
			float fvalue1;
			float fvalue2;
			float fvalue3;
			float fvalue4;
		};
	};
};

class InstrumentInstance{
public:
	int id;
	int flags;
	int cpu;
	MTModule *module;
	Instrument *parent;
	Track *track;
	PatternInstance *caller;
	int layer;
	double note;
	double cpos;
	double nextevent;
	double gvol;
	double mvol;
	float gpanx,gpany,gpanz;
	float mpanx,mpany,mpanz;
	
	InstrumentInstance(Instrument *p,Track *t,PatternInstance *c,int l,InstrumentInstance *previous){
		flags = 0; cpu = 0; parent = p; module = p->parent; track = t; caller = c; layer = l; note = cpos = nextevent = 0.0;	gvol = mvol = 1.0; mpanx = mpany = mpanz = 0.0;
	};
	virtual ~InstrumentInstance(){
	};
	
	virtual void MTCT processevents() = 0;
	virtual bool MTCT seek(double offset,int origin = MTIS_BEGIN,int units = MTIS_BEATS){
		return false;
	};
	virtual void MTCT process(int count) = 0;
	virtual void MTCT sendevents(int nevents,MTIEvent **events) = 0;
	virtual float MTCT getimportance() = 0;
	virtual void MTCT changecaller(PatternInstance *newcaller){
		caller = newcaller;
	};
};
//---------------------------------------------------------------------------
// MadTracker Instrument
//---------------------------------------------------------------------------
#define MAX_GRPS 64

#define MTIF_FADEOUT   0x0100
#define MTIF_FAE       0x0200 // Fade After Envelope

#define EF_ENABLED 0x1
#define EF_SUSTAIN 0x2
#define EF_LOOP    0x4

enum{
	NNA_NOTECUT = 0,
	NNA_CONTINUE,
	NNA_NOTEOFF,
	NNA_FADEOUT
};

enum{
	NDT_DISABLED = 0,
	NDT_NOTE,
	NDT_SAMPLE,
	NDT_INSTR
};

// Insrtument group
struct Group{
	Oscillator *spl;
	double vol;
	float panx,pany,panz;
	signed char pitch;
	char reserved[3];
};

// Instrument Envelope structure
struct IEnvelope{
	char flags;
	char npoints;
	char susts,suste,loops,loope;
	short reserved;
	EnvPoint points[16];
};

class InstrumentType : public ObjectType{
public:
	InstrumentType();

	MTObject* MTCT create(MTModule *parent,int id,void *param);
};

// Instrument class
class MTInstrument : public Instrument{
public:
	MTInstrument(MTModule *parent,int i);
	~MTInstrument();
	
	double gvol;
	float gpanx,gpany,gpanz;
	unsigned char vibtype,vibsweep,vibdepth,vibrate;
	double fadeout;
	unsigned short nna;
	MTFilter *filter;
	unsigned short cutoff;
	unsigned char resonance,attack,decay;
	IEnvelope env[8];
	Group grp[MAX_GRPS];
	unsigned char range[8][96];
	
	InstrumentInstance* MTCT createinstance(Track *track,PatternInstance *caller,InstrumentInstance *previous);
	void MTCT enumchildren(MTObjectEnum enumproc,void *data);
	bool MTCT acceptoscillator();
	int MTCT addoscillator(Oscillator *o);
	int MTCT deloscillator(Oscillator *o);
	virtual void MTCT setfilter(bool active);
};

#define MTIIF_NOTECUT    0x100
#define MTIIF_NOTEOFF    0x200
#define MTIIF_FADEOUT    0x300
#define MTIIF_STATUSMASK 0x700

struct EnvStatus{
	double count;
	double pos;
	unsigned short envp,lenvp;
	bool wait;
	bool process;
};

class MTInstrumentInstance : public InstrumentInstance{
public:
	OscillatorInstance *osc[1];
	unsigned char grp[1];
	double cfadeout;
	double fadeout;
	unsigned short iflags,nna;
	EnvStatus envs[8];
	MTFilterInstance *filter;
	sample *buffer[8];
	
	MTInstrumentInstance(Instrument *p,Track *t,PatternInstance *c,int l,InstrumentInstance *previous);
	~MTInstrumentInstance();
	void MTCT processevents();
	bool MTCT seek(double offset,int origin,int units);
	void MTCT process(int count);
	void MTCT sendevents(int nevents,MTIEvent **events);
	float MTCT getimportance();
private:
	double lastpos,fadepos;
	double tvol,cvol;
	float tpanx,tpany,tpanz,cpanx,cpany,cpanz;
	double ccutoff;
	double getvolume(double delay = 0.0);
	void getpanning(float *x,float *y,float *z,double delay = 0.0);
};
//---------------------------------------------------------------------------
extern InstrumentType *instrumenttype;
//---------------------------------------------------------------------------
#endif
