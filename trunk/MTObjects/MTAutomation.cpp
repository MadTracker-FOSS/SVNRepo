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
#include "MTModule.h"
#include "MTObjects1.h"
#include "MTAutomation.h"
#include "../../debug/Interface/MTObjectsRES.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
// Automation functions
//---------------------------------------------------------------------------
Automation::Automation(MTModule *parent,int i):
MTObject(parent,MTO_AUTOMATION,i)
{
	mtmemzero(&trkauto,sizeof(trkauto));
	res->loadstringf(MTT_automation,name,255,id);
}

Automation::~Automation()
{
	int x,y;
	
	if (parent){
		parent->apatt->a[id] = 0;
		for (y=0;y<MAX_LAYERS;y++){
			for (x=0;x<MAX_SEQUENCES;x++)
				if (parent->sequ[y][x].patt==(id | 0x1000)) parent->sequ[y][x].patt = -1;
		};
	};
	for (x=0;x<MAX_TRACKS+MAX_MTRACKS;x++)
		if (trkauto[x].trkenv) si->memfree(trkauto[x].trkenv);
}
//---------------------------------------------------------------------------
