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
#include "MTObject.h"
#include "MTEffect.h"
//---------------------------------------------------------------------------
Effect::Effect(MTObject *parent,mt_uint32 type,mt_int32 i):
Node(parent,type,i),
quality(0.5),
dry(1.0),
wet(1.0),
ei(0)
{
	setoutput(A(module->master,Track)[0],1.0);
}

Effect::~Effect()
{
}

int Effect::process(int ooffset,int ioffset,int count,bool &silence)
{
	if (ei) return ei->process(ooffset,ioffset,count,silence);
	return 0;
}
//---------------------------------------------------------------------------
