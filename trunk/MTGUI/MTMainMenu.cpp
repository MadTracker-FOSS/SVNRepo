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
#include "MTMainMenu.h"
#include "../Headers/MTXSkin.h"
//---------------------------------------------------------------------------
extern MTMenu *newctrlmenu;
//---------------------------------------------------------------------------
// MTControl
//   MTWinControl
//     MTPanel
//---------------------------------------------------------------------------
MTMainMenu::MTMainMenu(int tag,MTWinControl *p,int l,int t,int w,int h):
MTWinControl(MTC_MAINMENU,tag,p,l,t,w,h)
{
	flags |= MTCF_ACCEPTCTRLS|MTCF_TRANSPARENT;
	flags &= (~MTCF_BORDER);
}

bool MTMainMenu::message(MTCMessage &msg)
{
	return MTWinControl::message(msg);
}
//---------------------------------------------------------------------------
