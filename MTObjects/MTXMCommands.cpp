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
#include "MTXMCommands.h"
#include "MTColumns.h"
//---------------------------------------------------------------------------
static short retrigadd[16] = {0,-1,-2,-4,-8,-16,0,0,0,1,2,4,8,16,0,0};
static char retrigmul[16] = {0,0,0,0,0,0,11,8,0,0,0,0,0,0,24,32};
//---------------------------------------------------------------------------
void xmfirstpass(MTPatternInstance *pi,unsigned short command,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	unsigned char fx = command>>8;
	register unsigned char param = command & 0xFF;

	if (tick) return;
	if ((fx==0xE) && ((param>>4)==0xD)){
		param &= 0xF;
		pass.delay = (double)param/nticks-status.cpos;
	}
	else if ((fx==0x3) && (pass.flags & MTFP_ISNOTE)){
		pass.delay = -1.0;
		pass.flags &= (~MTFP_ISNOTE);
	};
}

double xmcommand(MTPatternInstance *pi,unsigned short command,FirstPass &pass,ColumnStatus &status,int tick,int nticks)
{
	register EffectData &ed = *(EffectData*)&status.data;
	NoteData *nd = (NoteData*)&pi->getnotestatus()->data;
	unsigned char fx = command>>8;
	register unsigned char param = command & 0xFF;
	int x;
	static MTIParamEvent pe[2] = {{0,MTIE_PARAM,0.0,0,sizeof(MTIParamEvent)},{0,MTIE_PARAM,0.0,0,sizeof(MTIParamEvent)}};
	static MTIParamEvent *cpe = pe;

	switch (fx){
// Arpeggio
	case 0x0:
		if (!param){
			if (ed.flags & EDF_TONEMODIFIED){
				ed.flags &= (~EDF_TONEMODIFIED);
				pe[0].param = MTIP_NOTE;
				pe[0].flags = 0;
				pe[0].dvalue1 = nd->onote;
				pi->sendevents(1,(MTIEvent**)&cpe);
			};
		}
		else{
			pe[0].param = MTIP_NOTE;
			pe[0].flags = 0;
			pe[0].dvalue1 = nd->onote;
			switch (tick%3){
			case 1:
				pe[0].dvalue1 += param>>4;
				break;
			case 2:
				pe[0].dvalue1 += param & 0xF;
				break;
			};
			pi->sendevents(1,(MTIEvent**)&cpe);
			ed.flags |= EDF_TONEMODIFIED;
		};
		return 1.0/nticks;
// Portamento up
	case 0x1:
		if (tick){
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			pe[0].dvalue1 = (double)param/16;
			pi->sendevents(1,(MTIEvent**)&cpe);
		};
		return 1.0/nticks;
// Portamento down
	case 0x2:
		if (tick){
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			pe[0].dvalue1 = -(double)param/16;
			pi->sendevents(1,(MTIEvent**)&cpe);
		};
		return 1.0/nticks;
// Tone portamento
	case 0x3:
		if (tick){
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			if (nd->ninstances){
				unsigned char note = *pi->getnotedata();
				double ref = (double)param/16;
				if ((note>0) && (note<=96)) nd->dnote = note+11;
				pe[0].dvalue1 = nd->dnote-nd->cnote;
				if (pe[0].dvalue1<-ref) pe[0].dvalue1 = -ref;
				else if (pe[0].dvalue1>ref) pe[0].dvalue1 = ref;
				nd->cnote += pe[0].dvalue1;
				pi->sendevents(1,(MTIEvent**)&cpe);
			};
		};
		return 1.0/nticks;
// Vibrato
	case 0x4:
		break;
// Tone portamento + Volume slide
	case 0x5:
		if (tick){
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			if (nd->ninstances){
				unsigned char note = *pi->getnotedata();
				double ref = (double)ed.old[0]/16;
				if (note) nd->dnote = note+11;
				pe[0].dvalue1 = nd->dnote-nd->cnote;
				if (pe[0].dvalue1<-ref) pe[0].dvalue1 = -ref;
				else if (pe[0].dvalue1>ref) pe[0].dvalue1 = ref;
				nd->cnote += pe[0].dvalue1;
				pi->sendevents(1,(MTIEvent**)&cpe);
			};
			if (!param) param = ed.old[1];
			else ed.old[1] = param;
			pe[1].param = MTIP_VOLUME;
			pe[1].flags = MTIEF_ADD;
			if (param>>4){
				pe[1].dvalue1 = /*pass.volume**/((double)(param>>4)/64.0);
			}
			else{
				pe[1].dvalue1 = -/*pass.volume**/((double)(param & 0xF)/64.0);
			};
			pi->sendevents(2,(MTIEvent**)&cpe);
		};
		break;
// Vibrato + Volume slide
	case 0x6:
		break;
// Tremolo
	case 0x7:
		break;
// Set panning position
	case 0x8:
		pe[0].param = MTIP_PANNING;
		pe[0].fvalue1 = (float)(param-128)/128;
		pe[0].fvalue2 = pe[0].fvalue3 = 0.0;
		pi->sendevents(1,(MTIEvent**)&cpe);
		break;
// Sample offset
	case 0x9:
		if (!param) param = ed.old[2];
		else ed.old[2] = param;
		for (x=0;x<nd->ninstances;x++){
			InstrumentInstance &ci = *nd->lastinstance[x];
			if (ci.flags & IIF_BACKGROUND) continue;
			ci.seek(param*256.0,MTIS_BEGIN,MTIS_SAMPLES|MTIS_INNER);
		};
		break;
// Volume slide
	case 0xA:
		if (tick){
			if (!param) param = ed.old[1];
			else ed.old[1] = param;
			pe[0].param = MTIP_VOLUME;
			pe[0].flags = MTIEF_ADD;
			if (param>>4){
				pe[0].dvalue1 = /*pass.volume**/((double)(param>>4)/64.0);
			}
			else{
				pe[0].dvalue1 = -/*pass.volume**/((double)(param & 0xF)/64.0);
			};
			pi->sendevents(1,(MTIEvent**)&cpe);
		};
		return 1.0/nticks;
// Set volume
	case 0xC:
		pe[0].param = MTIP_VOLUME;
		pe[0].flags = MTIEF_DONTSATURATE;
		pe[0].dvalue1 = /*pass.volume**/((double)param/64.0);
		pi->sendevents(1,(MTIEvent**)&cpe);
		break;
// Extended commands
	case 0xE:
		fx = param>>4;
		param &= 0xF;
		switch (fx){
// Fine portamento up
		case 0x1:
			if (tick) break;
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			pe[0].dvalue1 = (double)param/16;
			pi->sendevents(1,(MTIEvent**)&cpe);
			break;
// Fine portamento down
		case 0x2:
			if (tick) break;
			if (!param) param = ed.old[0];
			else ed.old[0] = param;
			pe[0].param = MTIP_NOTE;
			pe[0].flags = MTIEF_ADD;
			pe[0].dvalue1 = -(double)param/16;
			pi->sendevents(1,(MTIEvent**)&cpe);
			break;
// Set glissando control
		case 0x3:
			break;
// Set vibrato waveform
		case 0x4:
			break;
// Set finetune
		case 0x5:
			break;
// Set loop begin/end
		case 0x6:
			break;
// Set tremolo waveform
		case 0x7:
			break;
// Not implemented
		case 0x8:
			break;
// Retrig note
		case 0x9:
			break;
// Fine volume slide up
		case 0xA:
			break;
// Fine volume slide down
		case 0xB:
			break;
// Note cut
		case 0xC:
			break;
		};
		break;
// Global volume
	case 0x10:
		break;
// Global volume slide
	case 0x11:
		break;
// Key off
	case 0x14:
		break;
// Envelope position
	case 0x15:
		break;
// Panning slide
	case 0x19:
		break;
// Multi retrig
	case 0x1B:
		break;
// Tremor
	case 0x1D:
		break;
// Extra fine portamento
	case 0x21:
		break;
	};
	return 1.0;
}
//---------------------------------------------------------------------------
