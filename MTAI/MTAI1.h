//---------------------------------------------------------------------------
//
//	MadTracker Artificial Intelligence System
//
//		Platforms:	All
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTAI1_INCLUDED
#define MTAI1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int aitype = FOURCC('X','A','I','S');
//---------------------------------------------------------------------------
#include "MTXSystem.h"
//---------------------------------------------------------------------------
class MTAIInterface : public MTXInterface{
public:
	MTAIInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
};
//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTSystemInterface *si;
//---------------------------------------------------------------------------
#endif
