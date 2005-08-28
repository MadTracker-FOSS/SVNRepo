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
#ifndef MTTRACK_INCLUDED
#define MTTRACK_INCLUDED

class Track;

#include "MTNode.h"
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
	int MTCT process(int ooffset,int ioffset,int count,bool &silence);
	virtual void MTCT alloc();
	virtual void MTCT free();
};
//---------------------------------------------------------------------------
#endif
