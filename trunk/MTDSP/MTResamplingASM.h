//---------------------------------------------------------------------------
#ifndef MTRESAMPLINGASM_INCLUDED
#define MTRESAMPLINGASM_INCLUDED

#include "MTDSP1.h"
//---------------------------------------------------------------------------
void MTACT a_resample_none_8(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd);
void MTACT a_resample_none_16(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd);
void MTACT a_resample_none_32(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd);
//---------------------------------------------------------------------------
#endif
