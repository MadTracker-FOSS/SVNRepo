//---------------------------------------------------------------------------
//
//	MadTracker 3
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright � 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <stdio.h>
	#include <unistd.h>
#endif
#include "MTData.h"
//---------------------------------------------------------------------------
void *instance;
//---------------------------------------------------------------------------
#ifdef _WIN32
#include "MTInterface.h"
//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
//
//	Main call
//
{
	instance = (void*)hInstance;
	if (init()){
retry:
		MTTRY{
			while (gi->processmessages(true)>0){
				if (wantreset){
					wantreset = false;
					LOGD("%s - Reseting extensions..."NL);
					stopExtensions();
					uninitExtensions();
					if (initExtensions()) startExtensions();
				};
			};
		}
		MTCATCH{
			SetWindowPos((HWND)wnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
			if (MessageBox(0,"An unhandled exception occured!"NL"\
A log file has been generated with details about the error."NL"\
You should send this file to Yannick so that he can fix this problem."NL"\
Click on \"Retry\" to try to continue to work with MadTracker."NL"\
Otherwise, click on \"Cancel\" to exit gracefully.","MadTracker - Fatal Error",MB_RETRYCANCEL|MB_ICONERROR|MB_TASKMODAL)==IDRETRY) goto retry;
		};
	};
	uninit();
	return 0;
}
#else
const char **cargv;
int main(int argc,const char* argv[])
{
	instance = (void*)getpid();
	cargv = argv;
	if (init()){

	};
	uninit();
	return 0;
}
#endif
//---------------------------------------------------------------------------
