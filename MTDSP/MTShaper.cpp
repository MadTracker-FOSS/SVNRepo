//---------------------------------------------------------------------------
//
//	MadTracker DSP Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include "MTShaper.h"
#include "MTDSP1.h"
#include "../Headers/MTXSystem2.h"
//---------------------------------------------------------------------------
MTShaper::MTShaper()
{

}

MTShaper::~MTShaper()
{
	int x;

	for (x=0;x<16;x++){
		if (l[x].type==MTSHAPE_BUFFER) si->memfree(l[x].data);
	};
}

void MTShaper::add(int layer,int x1,int x2,sample y)
{
	if ((layer<0) || (layer>=SHAPER_LAYERS)) return;
	if (l[layer].type==MTSHAPE_BUFFER) si->memfree(l[layer].data);
	l[layer].type = MTSHAPE_FLAT;
	l[layer].x1 = x1;
	l[layer].x2 = x2;
	l[layer].y1 = y;
}

void MTShaper::add(int layer,int x1,sample y1,int x2,sample y2)
{
	if ((layer<0) || (layer>=SHAPER_LAYERS)) return;
	if (l[layer].type==MTSHAPE_BUFFER) si->memfree(l[layer].data);
	l[layer].type = MTSHAPE_LINEAR;
	l[layer].x1 = x1;
	l[layer].y1 = y1;
	l[layer].x2 = x2;
	l[layer].y2 = y2;
}

void MTShaper::add(int layer,int x1,sample y1,int x2,sample y2,int x3,sample y3,int x4,sample y4)
{
	if ((layer<0) || (layer>=SHAPER_LAYERS)) return;
	if (l[layer].type==MTSHAPE_BUFFER) si->memfree(l[layer].data);
	l[layer].type = MTSHAPE_SPLINE;
	l[layer].x1 = x1;
	l[layer].y1 = y1;
	l[layer].x2 = x2;
	l[layer].y2 = y2;
	l[layer].x3 = x3;
	l[layer].y3 = y3;
	l[layer].x4 = x4;
	l[layer].y4 = y4;
}

void MTShaper::add(int layer,int x1,int x2,sample *data)
{
	if ((layer<0) || (layer>=SHAPER_LAYERS)) return;
	if (l[layer].type==MTSHAPE_BUFFER) si->memfree(l[layer].data);
	l[layer].type = MTSHAPE_BUFFER;
	l[layer].data = (sample*)si->memalloc((x2-x1)*s_sample);
	dspi->replacebuffer(l[layer].data,data,x2-x1);
	memcpy(l[layer].data,data,(x2-x1)*s_sample);
}

MTShape* MTShaper::get(int from,int to,int accept)
{
	MTShape *res;

	if (!(accept & MTSHAPE_BUFFER)) return 0;
	res = mtnew(MTShape);

	return res;
}

void MTShaper::flush(int to)
{
	int x;

	for (x=0;x<16;x++){
		MTShape &cl = l[x];
		switch (cl.type){
		case MTSHAPE_FLAT:
			if (cl.x2<=to) cl.type = MTSHAPE_EMPTY;
			break;
		case MTSHAPE_LINEAR:
			if (cl.x2<=to) cl.type = MTSHAPE_EMPTY;
			else if (cl.x1<to){
				cl.y1 = cl.y2+(cl.y1-cl.y2)*(cl.x2-to)/(cl.x2-cl.x1);
				cl.x1 = to;
			};
			break;
		case MTSHAPE_SECOND:

			break;
		case MTSHAPE_THIRD:

			break;
		case MTSHAPE_SPLINE:

			break;
		case MTSHAPE_BUFFER:
			if (cl.x2<=to){
				cl.type = MTSHAPE_EMPTY;
				si->memfree(cl.data);
			}
			else if (cl.x1<to){
				memmove(cl.data+(to-cl.x1),cl.data,cl.x2-to);
				si->memrealloc(cl.data,s_sample*(cl.x2-to));
				cl.x1 = to;
			};
			break;
		default:
			continue;
			break;
		};
	};
}
//---------------------------------------------------------------------------
