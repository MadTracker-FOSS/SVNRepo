//---------------------------------------------------------------------------
#ifndef MTXMCOMMANDS_INCLUDED
#define MTXMCOMMANDS_INCLUDED
//---------------------------------------------------------------------------
#include "MTPattern.h"
//---------------------------------------------------------------------------
void xmfirstpass(MTPatternInstance*,unsigned short command,FirstPass &pass,ColumnStatus &status,int tick,int nticks);
double xmcommand(MTPatternInstance*,unsigned short command,FirstPass &pass,ColumnStatus &status,int tick,int nticks);
//---------------------------------------------------------------------------
#endif
