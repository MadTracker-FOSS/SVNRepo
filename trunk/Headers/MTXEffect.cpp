//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include "MTXObject.h"
#include "MTXEffect.h"
//---------------------------------------------------------------------------
Effect::Effect(MTModule *parent,int type,int i):
Node(parent,type,i),
ei(0)
{
	int x;

	for (x=0;x<noutputs;x++){
		outputs[x].s = x;
		outputs[x].d = x;
		outputs[x].m = 1.0;
	};
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
