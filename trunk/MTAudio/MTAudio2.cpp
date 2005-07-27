//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	Win32
//		Processors:	x86
//
//	Copyright � 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include "MTAudio1.h"
#include "MTAudio2.h"
#include "../Headers/MTXObject.h"
#include "../Headers/MTXModule.h"
#include "../Headers/MTXTrack.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
void __stdcall ri8(sample *source,void *dest,int count,int nchannels,int channel)
{
	static const sample f127 = 127.0;

	__asm{
		push	esi
		push	edi
		push	ebx
		sub		esp,4
		mov		ecx,count
		mov		esi,source
		test	ecx,ecx
		mov		edi,dest
		jz		mexit
		mov		ebx,nchannels
		sub		edi,ebx
		add		edi,channel
		ALIGN	8
render:
		fld	  a_sample ptr [esi]
		fmul	f127
		fistp	dword ptr [esp]
		add		edi,ebx
		mov		eax,[esp]
		add		esi,s_sample
		cmp		eax,127
		jle		lsatok
		mov		eax,127
		jmp		satok
lsatok:
		cmp		eax,-128
		jge		satok
		mov		eax,-128
satok:
		dec		ecx
		mov		[edi],al
		jnz		render
mexit:
		add		esp,4
		pop		ebx
		pop		edi
		pop		esi
	};
}

void __stdcall ri16(sample *source,void *dest,int count,int nchannels,int channel)
{
	static const sample f32767 = 32767.0;

	__asm{
		push	esi
		push	edi
		push	ebx
		sub		esp,4
		mov		ecx,count
		mov		esi,source
		test	ecx,ecx
		mov		edi,dest
		jz		mexit
		mov		ebx,nchannels
		mov		eax,channel
		sal		ebx,1
		sal		eax,1
		sub		edi,ebx
		add		edi,eax
		ALIGN	8
render:
		fld	  a_sample ptr [esi]
		fmul	f32767
		fistp	dword ptr [esp]
		add		edi,ebx
		mov		eax,[esp]
		add		esi,s_sample
		cmp		eax,32767
		jle		lsatok
		mov		eax,32767
		jmp		satok
lsatok:
		cmp		eax,-32768
		jge		satok
		mov		eax,-32768
satok:
		dec		ecx
		mov		[edi],ax
		jnz		render
mexit:
		add		esp,4
		pop		ebx
		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void generateoutput()
{
	static RenderProc renderproc[4] = {&ri8,&ri16,0,0};
	static struct DevPos{
		bool ready;
		int start;
		char *ptr;
		int lastoffset;
		void *ptr1,*ptr2;
		unsigned long lng1,lng2;
		char *cptr;
		unsigned long clng1,clng2;
		double timeopened;
	} devpos[MAX_AUDIODEVICES];
	int x,y,pos,lngc;
	register unsigned long minlng;
	bool mix = false;

	ENTER("generateoutput");
// Retrieve playing positions
	for (x=0;x<output.ndevices;x++){
		WaveDevice &cdev = *output.device[x];
		DevPos &cdevpos = devpos[x];
		cdevpos.timeopened = cdev.timeopened;
		pos = cdev.device->getposition();
		cdevpos.ready = false;
		if (pos>=0){
			if (x) lngc = (int)((cdevpos.timeopened-devpos[0].timeopened)*output.frequency);
			else lngc = 0;
			lngc += output.buffersamples;
			cdevpos.start = (pos+lngc)%cdev.datasamples;
			cdevpos.ready = true;
		};
	};
	for (x=0;x<output.ndevices;x++){
		WaveDevice &cdev = *output.device[x];
		DevPos &cdevpos = devpos[x];
		if (cdevpos.ready){
			cdevpos.ready = false;
			lngc = cdevpos.start-cdevpos.lastoffset;
			if (lngc<=0) lngc += cdev.datasamples;
			if (!cdev.device->getdata(cdevpos.lastoffset,lngc,&cdevpos.ptr1,&cdevpos.ptr2,&cdevpos.lng1,&cdevpos.lng2)) continue;
			cdevpos.ready = true;
			cdevpos.cptr = (char*)cdevpos.ptr1;
			cdevpos.clng1 = cdevpos.lng1;
			cdevpos.clng2 = cdevpos.lng2;
/*
			double t = si->cpufrequ;
			int cdpos = cdev.device->getposition(true)%cdev.device->datasamples;
			int cddelay = cdevpos.lastoffset-cdpos;
			if (cddelay<=0) cddelay += cdev.datasamples;
			FLOG3("% 6d: % 6d (%d)"NL,cdpos,cdevpos.lastoffset,cddelay);
*/
		};
	};
	
// Player
	lngc = 0;
	do{
		minlng = 0x7FFFFFFF;
		for (x=0;x<output.ndevices;x++){
			DevPos &cdevpos = devpos[x];
			if (cdevpos.clng1){
				if (cdevpos.clng1<minlng) minlng = cdevpos.clng1;
			}
			else{
				if (cdevpos.clng2<minlng) minlng = cdevpos.clng2;
			};
		};
		if ((int)minlng<=0) break;
		if ((lngc) && (minlng<64)) break;
		output.playlng = minlng;
		lngc += minlng;
		for (x=0;x<output.ndevices;x++){
			WaveDevice &cdev = *output.device[x];
			DevPos &cdevpos = devpos[x];
			int sl = (cdev.bits*cdev.nchannels)>>3;
			cdevpos.ptr = cdevpos.cptr;
			if (cdevpos.clng1){
				if ((cdevpos.clng1 -= minlng)==0) cdevpos.cptr = (char*)cdevpos.ptr2;
				else cdevpos.cptr += minlng*sl;
			}
			else{
				cdevpos.clng2 -= minlng;
				cdevpos.cptr += minlng*sl;
			};
		};
		
		// Empty master buffer
		for (x=0;x<output.ndevices;x++){
			Track &master = *output.device[x]->master;
			for (y=0;y<master.noutputs;y++){
				mtmemzero(master.buffer[y],minlng*sizeof(sample));
			};
		};

//Player
		x = mtinterface->getnummodules();
		while (x>0){
			MTModule *cmod = (MTModule*)mtinterface->getmodule(--x);
			if (cmod->lockread) continue;
			if (cmod->process(&output)) mix = true;
		};

// Render to soundcard
		for (x=0;x<output.ndevices;x++){
			DevPos &cdevpos = devpos[x];
			WaveDevice &cdev = *output.device[x];
			Track &master = *cdev.master;
			int rid = cdev.bits/8-1;
			for (y=0;y<cdev.nchannels;y++){
				renderproc[rid](master.buffer[y],cdevpos.ptr,minlng,cdev.nchannels,y);
			};
		};
#ifdef _DEBUG
		if (recf){
			if (mix) recf->write(devpos[0].ptr,minlng*(output.device[0]->bits*output.device[0]->nchannels)>>3);
		};
#endif
	} while (minlng);
	
// Unlock devices
	for (x=0;x<output.ndevices;x++){
		WaveDevice &cdev = *output.device[x];
		DevPos &cdevpos = devpos[x];
		cdevpos.lastoffset = cdevpos.start;
		int sl = (cdev.bits*cdev.nchannels)>>3;
		if ((cdevpos.ready) && (cdevpos.ptr1)){
			cdev.device->writedata(cdevpos.ptr1,cdevpos.ptr2,cdevpos.lng1,cdevpos.lng2);
		};
	};
	LEAVE();
}
//---------------------------------------------------------------------------