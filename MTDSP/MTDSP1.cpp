//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <math.h>
#include "MTDSP1.h"
#include "MTBufferASM.h"
#include "MTFilterASM.h"
#include "MTResamplingASM.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
static const char *dspname = {"MadTracker DSP"};
static const int dspversion = 0x30000;
static const MTXKey dspkey = {0,0,0,0};
MTXInterfaces i;
MTDSPInterface *oi;
MTInterface *mtinterface;
MTSystemInterface *si;
MTResampleProc proc[255];
//---------------------------------------------------------------------------
MTDSPInterface::MTDSPInterface()
{
	type = dsptype;
	key = &dspkey;
	name = dspname;
	version = dspversion;
	status = 0;
	mtmemzero(proc,sizeof(proc));
}

bool MTDSPInterface::init()
{
	si = (MTSystemInterface*)mtinterface->getinterface(systemtype);
	if (!si) return false;
	ENTER("MTDSPInterface::init");
	LOGD("%s - [DSP] Initializing..."NL);
	emptybuffer = a_emptybuffer;
	replacebuffer = a_replacebuffer;
	replacebuffermul = a_replacebuffermul;
	addbuffer = a_addbuffer;
	addbuffermul = a_addbuffermul;
	addbuffermul2 = a_addbuffermul2;
	addbufferslide = a_addbufferslide;
	addbufferslide2 = a_addbufferslide2;
	ampbuffer = a_ampbuffer;
	filter[0x0] = a_filter_;
	filter[0x1] = a_filter_;
	filter[0x2] = a_filter_;
	filter[0x3] = a_filter_;
	filter[0x4] = a_filter_;
	filter[0x5] = a_filter_;
	filter[0x6] = a_filter_;
	filter[0x7] = a_filter_;
	filter[0x8] = a_filter_;
	filter[0x9] = a_filter_;
	filter[0xA] = a_filter_;
	filter[0xB] = a_filter_;
	filter[0xC] = a_filter_;
	filter[0xD] = a_filter_;
	filter[0xE] = a_filter_;
	filter[0xF] = a_filter_;
	resample[0x00] = a_resample_none_8;
	resample[0x01] = a_resample_none_16;
	resample[0x02] = a_resample_none_32;
	resample[0x03] = a_resample_none_32;
	resample[0x04] = a_resample_none_8;
	resample[0x05] = a_resample_none_16;
	resample[0x06] = a_resample_none_32;
	resample[0x07] = a_resample_none_32;
	resample[0x08] = a_resample_none_8;
	resample[0x09] = a_resample_none_16;
	resample[0x0A] = a_resample_none_32;
	resample[0x0B] = a_resample_none_32;
	resample[0x0C] = a_resample_none_8;
	resample[0x0D] = a_resample_none_16;
	resample[0x0E] = a_resample_none_32;
	resample[0x0F] = a_resample_none_32;
	resample[0x10] = a_resample_none_8;
	resample[0x11] = a_resample_none_16;
	resample[0x12] = a_resample_none_32;
	resample[0x13] = a_resample_none_32;
	resample[0x14] = a_resample_none_8;
	resample[0x15] = a_resample_none_16;
	resample[0x16] = a_resample_none_32;
	resample[0x17] = a_resample_none_32;
	resample[0x18] = a_resample_none_8;
	resample[0x19] = a_resample_none_16;
	resample[0x1A] = a_resample_none_32;
	resample[0x1B] = a_resample_none_32;
	resample[0x1C] = a_resample_none_8;
	resample[0x1D] = a_resample_none_16;
	resample[0x1E] = a_resample_none_32;
	resample[0x1F] = a_resample_none_32;
	status |= MTX_INITIALIZED;
	LEAVE();
	return true;
}

void MTDSPInterface::uninit()
{
	ENTER("MTDSPInterface::uninit");
	LOGD("%s - [DSP] Uninitializing..."NL);
	status &= (~MTX_INITIALIZED);
	mtmemzero(proc,sizeof(proc));
	LEAVE();
}

void MTDSPInterface::start()
{
}

void MTDSPInterface::stop()
{
}

void MTDSPInterface::processcmdline(void *params)
{

}

void MTDSPInterface::showusage(void *out)
{

}

int MTDSPInterface::config(int command,int param)
{
	return 0;
}
//---------------------------------------------------------------------------
extern "C"
{

MTXInterfaces* __stdcall MTXMain(MTInterface *mti)
{
	mtinterface = mti;
	if (!oi) oi = new MTDSPInterface();
	i.ninterfaces = 1;
	i.interfaces[0] = (MTXInterface*)oi;
	return &i;
}

}
//---------------------------------------------------------------------------
