//---------------------------------------------------------------------------
#ifndef MTOBJECTSASM_INCLUDED
#define MTOBJECTSASM_INCLUDED
//---------------------------------------------------------------------------
#include <math.h>
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
bool MTACT a_floattofixed(double f,int &i,unsigned int &d);
void MTACT a_changesign(char *buffer,int count);
void MTACT a_delta_add_8(char *dest,char *source,int count);
void MTACT a_delta_add_16(short *dest,short *source,int count);
void MTACT a_delta_encode_8(char *buffer,int count);
void MTACT a_delta_encode_16(short *buffer,int count);
void MTACT a_delta_decode_8(char *buffer,int count);
void MTACT a_delta_decode_16(short *buffer,int count);
void MTACT a_deinterleave_8(char **dest,char *source,int channels,int count);
void MTACT a_deinterleave_16(short **dest,short *source,int channels,int count);
void MTACT a_calcposition(int &posi,unsigned int &posd,int pitch,unsigned int pitchd,int count,bool reverse);
inline float pan_mul(float g,float p){ return g+p*(1.0f-(float)fabs(g)); };
inline float pan_div(float r,float g){ float _tmp = (float)fabs(g); return (g<1.0f)?((r-g)/(1.0f-_tmp)):0.0f; };
inline void pan_self_mul(float &g,float p){ g += p*(1.0f-(float)fabs(g)); };
//---------------------------------------------------------------------------
#endif
