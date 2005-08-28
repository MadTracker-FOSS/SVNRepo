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
#ifndef MTAUTOMATION_INCLUDED
#define MTAUTOMATION_INCLUDED

class Automation;

#include "MTModule.h"
//---------------------------------------------------------------------------
// Automation
//---------------------------------------------------------------------------
struct AEnvelope{
	int npoints;
	EnvPoint points[64];
};

struct TrackEnvelopes{
	AEnvelope env[16];
};

struct TrackAuto{
	int flags;
	int fx;
	TrackEnvelopes *trkenv;
};

class Automation : public MTObject{
public:
	Automation(MTModule *parent,int i);
	virtual ~Automation();
	
	TrackAuto trkauto[MAX_TRACKS+MAX_MTRACKS];
};
//---------------------------------------------------------------------------
#endif
