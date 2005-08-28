//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTXAUTOMATION_INCLUDED
#define MTXAUTOMATION_INCLUDED
//---------------------------------------------------------------------------
class Automation;
//---------------------------------------------------------------------------
#include "MTXObject.h"
#include "MTXModule.h"
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
