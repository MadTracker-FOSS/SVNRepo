//---------------------------------------------------------------------------
#ifndef MTDELAY_INCLUDED
#define MTDELAY_INCLUDED
//---------------------------------------------------------------------------
class MTDelay;
class MTDelayInstance;
//---------------------------------------------------------------------------
#define MTO_MTDELAY      (MTO_EFFECT|1)
//---------------------------------------------------------------------------
#define DELAY_MAX_TAPS 16

#define DF_SAMPLES 0x01
#define DF_MS      0x02
#define DF_BEATS   0x03
#define DF_DELAYF  0x0F
//---------------------------------------------------------------------------
#include "MTEffect.h"
#include "../Headers/MTXDSP.h"
//---------------------------------------------------------------------------
class DelayType : public ObjectType{
public:
	MTArray *columns;

	DelayType();
	MTObject* MTCT create(MTModule *parent,int id,void *param);
};
//---------------------------------------------------------------------------
struct Tap{
	int flags;
	float delay;
	float volume;
	float feedback;
	Effect *effect;
	struct Pan{
		float pan;
		float amp;
		float period;
		float phase;
	} pan[3];
};
	
class MTDelay : public Effect{
public:
	MTDelay(MTModule *parent,int i);
	~MTDelay();
	void MTCT notify(MTObject *source,int message,int param1,void *param2);
	EffectInstance* MTCT createinstance(int noutputs,sample **outputs,int ninputs,sample **inputs,InstrumentInstance *caller = 0);
	void MTCT deleteinstance(EffectInstance *i);
	int MTCT getnumparams(int cat);
	const char* MTCT getparamname(int cat,int id);
	double MTCT getparam(int cat,int id,char *display);
	void MTCT setparam(int cat,int id,double value,int steps = 0);
	void MTCT settap(int tap,int flags,float delay,float volume,float feedback,Effect *effect);
private:
	friend class MTDelayInstance;
	int nsamples,cpos;
	sample *buffer[8];
	int ntaps;
	bool monomerge;
	Tap taps[DELAY_MAX_TAPS];
	int _frequency;
	double _bpm;
	void setbuffer(int nsamples);
};

class MTDelayInstance : public EffectInstance{
public:
	MTDelayInstance(Effect *p,int no,sample **o,int ni,sample **i,InstrumentInstance *caller);
	int MTCT process(int ooffset,int ioffset,int count,bool &silence);
	void MTCT setparam(int cat,int id,double value,int steps = 0);
private:
	struct TapStatus{
		float cphase[3];
	} taps[DELAY_MAX_TAPS];
};
//---------------------------------------------------------------------------
extern DelayType *delaytype;
//---------------------------------------------------------------------------
#endif
