//---------------------------------------------------------------------------
#ifndef MTOBJECTS2_INCLUDED
#define MTOBJECTS2_INCLUDED
//---------------------------------------------------------------------------
#if MTLEVEL >= MTL_PROFESSIONAL
//---------------------------------------------------------------------------
#include "../Headers/MTXSystem.h"
//---------------------------------------------------------------------------
int MTCT engineproc(MTThread *thread,void *param);
//---------------------------------------------------------------------------
extern int nthreads;
extern MTThread *thread[128];
//---------------------------------------------------------------------------
#endif // MTLEVEL >= MTL_PROFESSIONAL
//---------------------------------------------------------------------------
#endif
