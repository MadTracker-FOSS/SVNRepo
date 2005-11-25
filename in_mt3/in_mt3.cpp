// in_mt3.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "in_mt3.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
IN_MT3_API int nIn_mt3=0;

// This is an example of an exported function.
IN_MT3_API int fnIn_mt3(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see in_mt3.h for the class definition
CIn_mt3::CIn_mt3()
{ 
	return; 
}

