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
#ifndef MTINSTREDITOR_INCLUDED
#define MTINSTREDITOR_INCLUDED
//---------------------------------------------------------------------------
#include "MTInstrument.h"
//---------------------------------------------------------------------------
void initInstrEditor();
void uninitInstrEditor();
bool MTCT InstrumentEdit(MTObject *object,MTWindow *window,int flags,MTUser *user);
//---------------------------------------------------------------------------
extern int ieksgroup;
//---------------------------------------------------------------------------
#endif
