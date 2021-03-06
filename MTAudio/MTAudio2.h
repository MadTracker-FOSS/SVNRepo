//---------------------------------------------------------------------------
//
//	MadTracker Audio Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTAUDIO2_INCLUDED
#define MTAUDIO2_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
typedef void (MTCT *RenderProc)(sample *source,void *dest,int count,int nchannels,int channel);
//---------------------------------------------------------------------------
void generateoutput();
//---------------------------------------------------------------------------
#endif
