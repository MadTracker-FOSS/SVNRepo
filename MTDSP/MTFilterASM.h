//---------------------------------------------------------------------------
#ifndef MTFILTERASM_INCLUDED
#define MTFILTERASM_INCLUDED

#define FILTER_STEPS 100

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)
//---------------------------------------------------------------------------
#include "MTDSP1.h"
//---------------------------------------------------------------------------
void MTACT a_filter_(sample *dest,sample *source,FilterStatus &status,int count,int frequency);
//---------------------------------------------------------------------------
#endif
