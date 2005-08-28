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
#ifndef MTPATTEDITOR_INCLUDED
#define MTPATTEDITOR_INCLUDED
//---------------------------------------------------------------------------
#include "MTPattern.h"
//---------------------------------------------------------------------------
void initPattEditor();
void uninitPattEditor();
bool MTCT PatternEdit(MTObject *object,MTWindow *window,int flags,MTUser *user);
//---------------------------------------------------------------------------
extern int peksgroup;
//---------------------------------------------------------------------------
#endif
