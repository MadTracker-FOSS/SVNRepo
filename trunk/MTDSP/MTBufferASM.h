//---------------------------------------------------------------------------
#ifndef MTBUFFERASM_INCLUDED
#define MTBUFFERASM_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
void MTACT a_emptybuffer(sample *dest,int count);
void MTACT a_replacebuffer(sample *dest,sample *source,int count);
void MTACT a_replacebuffermul(sample *dest,sample *source,double a,int count);
void MTACT a_addbuffer(sample *dest,sample *source,int count);
void MTACT a_addbuffermul(sample *dest,sample *source,double a,int count);
void MTACT a_addbuffermul2(sample *dest1,sample *dest2,sample *source,double a1,double a2,int count);
void MTACT a_addbufferslide(sample *dest,sample *source,double mul,double i,int count);
void MTACT a_addbufferslide2(sample *dest1,sample *dest2,sample *source,double mul1,double mul2,double i1,double i2,int count);
void MTACT a_ampbuffer(sample *dest,double a,int count);
//---------------------------------------------------------------------------
#endif
