//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTXTRACK_INCLUDED
#define MTXTRACK_INCLUDED
//---------------------------------------------------------------------------
class Track;
//---------------------------------------------------------------------------
#include "MTXNode.h"
//---------------------------------------------------------------------------
// Track class
class Track : public Node{
public:
	Track(MTModule *parent,int i,int sub = 0);
	virtual ~Track();
	bool muted;
	bool solo;
	bool empty;
	double vol;
	float panx,pany,panz;
	sample *buffer[8];
	int nsamples;
	int offset;
	virtual void MTCT alloc() = 0;
	virtual void MTCT free() = 0;
};
//---------------------------------------------------------------------------
#endif
