//---------------------------------------------------------------------------
#ifndef MTFILTER_INCLUDED
#define MTFILTER_INCLUDED
//---------------------------------------------------------------------------
class MTFilter;
class MTFilterInstance;
//---------------------------------------------------------------------------
#include "MTEffect.h"
#include "../Headers/MTXDSP.h"
//---------------------------------------------------------------------------
class MTFilter : public Effect{
public:
	float frequency;
	float resonance;

	MTFilter(MTModule *parent,int i);
	EffectInstance* MTCT createinstance(int noutputs,sample **outputs,int ninputs,sample **inputs,InstrumentInstance *caller = 0);
	void MTCT deleteinstance(EffectInstance *i);
	int MTCT getnumparams(int cat);
	const char* MTCT getparamname(int cat,int id);
	double MTCT getparam(int cat,int id,char *display);
	void MTCT setparam(int cat,int id,double value,int steps = 0);
};

class MTFilterInstance : public EffectInstance{
public:
	MTFilterInstance(Effect *p,int no,sample **o,int ni,sample **i,InstrumentInstance *caller);
	int MTCT process(int ooffset,int ioffset,int count,bool &silence);
	void MTCT setparam(int cat,int id,double value,int steps = 0);
private:
	FilterStatus status[8];
};
//---------------------------------------------------------------------------
#endif
