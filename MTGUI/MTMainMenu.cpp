//---------------------------------------------------------------------------
//
//	MadTracker GUI Core
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include "MTToolBar.h"
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTToolBar
//---------------------------------------------------------------------------
MTToolBar::MTToolBar(int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(MTC_TOOLBAR,tag,p,l,t,w,h)
{
	flags |= MTCF_ACCEPTCTRLS|MTCF_TRANSPARENT;
	flags &= (~MTCF_BORDER);
}

bool MTToolBar::message(MTCMessage &msg)
{
	return MTWinControl::message(msg);
}
//---------------------------------------------------------------------------
