//---------------------------------------------------------------------------
#ifndef MTCATMULLASM_INCLUDED
#define MTCATMULLASM_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
void MTACT a_splinereplace(sample *dest,int size,double x0,sample p0,double x1,sample p1,double x2,sample p2,double x3,sample p3,double xf,double xt);
void MTACT a_splinemodulate(sample *dest,int size,double x0,sample p0,double x1,sample p1,double x2,sample p2,double x3,sample p3,double xf,double xt);
//---------------------------------------------------------------------------
#endif
