//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors:	All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include "MTObjects2.h"
#include "MTModule.h"
//---------------------------------------------------------------------------
#if MTLEVEL >= MTL_PROFESSIONAL
//---------------------------------------------------------------------------
int nthreads;
MTThread *thread[128];
//---------------------------------------------------------------------------
int MTCT engineproc(MTThread *thread,void *param)
{
	int msg,lng;
	MTModule *module;

	while (!thread->terminated){
		if (thread->getmessage(msg,*(int*)&module,lng,true)){
			if (msg==4096){
				if ((!module) || (module->lockread)) continue;
				module->subprocess(0,(int)param,lng);
			};
		}
		else{
			si->syswait(1);
		};
	};
	return 0;
}
//---------------------------------------------------------------------------
#endif // MTLEVEL >= MTL_PROFESSIONAL
//---------------------------------------------------------------------------
