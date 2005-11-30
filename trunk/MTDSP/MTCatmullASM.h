//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTCATMULLASM_INCLUDED
#define MTCATMULLASM_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
void MTCT a_splinereplace(sample *dest,int size,double x0,sample p0,double x1,sample p1,double x2,sample p2,double x3,sample p3,double xf,double xt);
void MTCT a_splinemodulate(sample *dest,int size,double x0,sample p0,double x1,sample p1,double x2,sample p2,double x3,sample p3,double xf,double xt);
//---------------------------------------------------------------------------
#endif
