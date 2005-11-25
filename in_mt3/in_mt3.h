
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IN_MT3_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IN_MT3_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef IN_MT3_EXPORTS
#define IN_MT3_API __declspec(dllexport)
#else
#define IN_MT3_API __declspec(dllimport)
#endif

// This class is exported from the in_mt3.dll
class IN_MT3_API CIn_mt3 {
public:
	CIn_mt3(void);
	// TODO: add your methods here.
};

extern IN_MT3_API int nIn_mt3;

IN_MT3_API int fnIn_mt3(void);

