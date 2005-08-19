//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTXEXTENSION_INCLUDED
#define MTXEXTENSION_INCLUDED

#define FOURCC(a,b,c,d) ((((long)a)<<24) | (((long)b)<<16) | (((long)c)<<8) | (((long)d)<<0))

//
//	Calling Convention
//

#if defined(WIN32) || defined(__FLAT__) || defined __BORLANDC__
	#define MTCT __cdecl
#else
	#define MTCT
#endif
#define MTACT __stdcall

#define NL "\r\n"

enum{
	MTL_FREE = 0,
	MTL_BASIC,
	MTL_PROFESSIONAL,
	MTL_COMMERCIAL
};

#if defined(MTVERSION_COMMERCIAL)
	#define MTLEVEL	MTL_COMMERCIAL
#elif defined(MTVERSION_PROFESSIONAL)
	#define MTLEVEL	MTL_PROFESSIONAL
#elif defined(MTVERSION_BASIC)
	#define MTLEVEL	MTL_BASIC
#else
	#define MTLEVEL	MTL_FREE
#endif

//
//	Uncomment the following line if you compile under a big endian processor
//	(PowerPC, etc.)
//

//#define BIG_ENDIAN

//
//	Uncomment the following line if you compile for the 64-bit version
//

//#define MT3_64BIT

//
//	Data types
//

#if defined(_WIN32)

#	if defined(__CYGWIN__)

#		include <_G_config.h>
		typedef _G_int64_t mt_int64;
		typedef _G_int32_t mt_int32;
		typedef _G_uint32_t mt_uint32;
		typedef _G_int16_t mt_int16;
		typedef _G_uint16_t mt_uint16;

#	elif defined(__MINGW32__)

		typedef short mt_int16;
		typedef unsigned short mt_uint16;
		typedef int mt_int32;
		typedef unsigned int mt_uint32;
		typedef long long mt_int64;
		typedef unsigned long long mt_uint64;

#	elif defined(__MWERKS__)

		typedef long long mt_int64;
		typedef int mt_int32;
		typedef unsigned int mt_uint32;
		typedef short mt_int16;
		typedef unsigned short mt_uint16;

#	else

		/* MSVC/Borland */
		typedef __int64 mt_int64;
		typedef __int32 mt_int32;
		typedef unsigned __int32 mt_uint32;
		typedef __int16 mt_int16;
		typedef unsigned __int16 mt_uint16;

#	endif

#elif defined(__MACOS__)

#	include <sys/types.h>
	typedef SInt16 mt_int16;
	typedef UInt16 mt_uint16;
	typedef SInt32 mt_int32;
	typedef UInt32 mt_uint32;
	typedef SInt64 mt_int64;

#elif defined(__MACOSX__)	/* MacOS X Framework build */

#	include <sys/types.h>
	typedef int16_t mt_int16;
	typedef u_int16_t mt_uint16;
	typedef int32_t mt_int32;
	typedef u_int32_t mt_uint32;
	typedef int64_t mt_int64;

#elif defined(__BEOS__)		/* Be */

#	include <inttypes.h>
	typedef int16_t mt_int16;
	typedef u_int16_t mt_uint16;
	typedef int32_t mt_int32;
	typedef u_int32_t mt_uint32;
	typedef int64_t mt_int64;

#elif defined (__EMX__)		/* OS/2 GCC */

	typedef short mt_int16;
	typedef unsigned short mt_uint16;
	typedef int mt_int32;
	typedef unsigned int mt_uint32;
	typedef long long mt_int64;

#elif defined (DJGPP)		/* DJGPP */

	typedef short mt_int16;
	typedef int mt_int32;
	typedef unsigned int mt_uint32;
	typedef long long mt_int64;

#elif defined(R5900)		/* PS2 EE */

	typedef long mt_int64;
	typedef int mt_int32;
	typedef unsigned mt_uint32;
	typedef short mt_int16;

#elif defined(__SYMBIAN32__)	/* Symbian GCC */

	typedef signed short mt_int16;
	typedef unsigned short mt_uint16;
	typedef signed int mt_int32;
	typedef unsigned int mt_uint32;
	typedef long long int mt_int64;

#else

#	include <sys/types.h>
	typedef int16_t mt_int16;
	typedef u_int16_t mt_uint16;
	typedef int32_t mt_int32;
	typedef u_int32_t mt_uint32;
	typedef int64_t mt_int64;

#endif

#ifdef MT3_64BIT
	typedef double sample;
	#define a_sample qword
	#define s_sample 8
#else
	typedef float  sample;
	#define a_sample dword
	#define s_sample 4
#endif

#define MTX_INITIALIZED	1

enum{
	SP_ROOT = 0,
	SP_CONFIG,
	SP_USERCONFIG,
	SP_EXTENSIONS,
	SP_SKINS,
	SP_INTERFACE,
	SP_HELP,
	SP_MAX
};

enum{
	UP_TEMP = 0,
	UP_CACHE,
	UP_MAX
};

enum{
	P_LOADMODULE = 0x1000,
	P_SAVEMODULE,
	P_LOADINSTRUMENT,
	P_SAVEINSTRUMENT,
	P_LOADSAMPLE,
	P_SAVESAMPLE,
	P_BUILDPEAKS =  0x2000
};
//---------------------------------------------------------------------------
struct MTPreferences{
	char options[16];
	char *path[8];
	char *syspath[8];
};

struct MTXKey{
	int key1,key2,key3,key4;
};

struct MTCLParam{
	char *name;
	char *value;
};

class MTXInterface{
public:
	int type;
	const char *name;
	int version;
	const MTXKey *key;
	int status;

	virtual bool MTCT init() = 0;
	virtual void MTCT uninit() = 0;
	virtual void MTCT start() = 0;
	virtual void MTCT stop() = 0;
	virtual void MTCT processcmdline(void *params){ };
	virtual void MTCT showusage(void *out){ };
	virtual int MTCT config(int command,int param){ return 0; };
	virtual int MTCT processinput(const char *input){ return 0; };
};

struct MTXInterfaces{
	int ninterfaces;
	MTXInterface* interfaces[1];
};

typedef void (MTCT *RefreshProc)(void *param);

class ObjectWrapper{
public:
	virtual void MTCT create(void *object,void *parent,int type,int i) = 0;
	virtual void MTCT destroy(void *object) = 0;
	virtual bool MTCT lock(void *object,int flags,bool lock,int timeout = -1) = 0;
	virtual void MTCT setname(void *object,char *newname) = 0;
	virtual void MTCT setmodified(void *object,int value,int flags) = 0;
};

class MTInterface{
public:
	int type;
	const char *name;
	int version;
	const MTXKey *key;

	ObjectWrapper *_ow;

	virtual MTXInterface* MTCT getinterface(int id) = 0;
	virtual MTPreferences* MTCT getprefs() = 0;
	virtual void* MTCT getcurrentuser() = 0;
	virtual int MTCT getnummodules() = 0;
	virtual void* MTCT getmodule(int id) = 0;
	virtual void MTCT addmodule(void *module) = 0;
	virtual void MTCT delmodule(void *module) = 0;
	virtual void MTCT setmodule(void *module) = 0;
	virtual bool MTCT addchannel() = 0;
	virtual void MTCT notify(void *object,int notify,int param) = 0;
	virtual bool MTCT editobject(void *object,bool newwindow) = 0;
	virtual void* MTCT getconf(char *name,bool user) = 0;
	virtual void MTCT releaseconf(void *conf) = 0;
	virtual int MTCT addrefreshproc(RefreshProc proc,void *param) = 0;
	virtual void MTCT delrefreshproc(int id) = 0;
	virtual char* MTCT getextension(void *ptr) = 0;
	virtual bool MTCT canclose() = 0;
	virtual void* MTCT getconsole(){ return 0; };
	virtual int MTCT processinput(const char *input){ return 0; };
};

typedef MTXInterfaces* __stdcall MTXMainCall(MTInterface *mti);
//---------------------------------------------------------------------------
//	Swapping functions to convert little<->big endian
//---------------------------------------------------------------------------
#ifndef __BORLANDC__
#ifdef WIN32
#pragma warning(disable: 4035)
#endif
inline short swap_word(short a)
{
#ifdef WIN32
	__asm{
		mov		ax,a
		xchg	al,ah
	};
#else
	asm ("xchg %al,%ah"::"a"(a):"%eax");
#endif
}

inline long swap_dword(long a)
{
#ifdef WIN32
	__asm{
		mov		eax,a
		bswap	eax
	};
#else
	asm ("bswap %eax"::"a"(a):"%eax");
#endif
}

inline void int64todouble(void *int64,double *d)
{
#ifdef WIN32
	__asm{
		fild	qword ptr int64;
		fst		qword ptr d
	};
#else
	asm ("fild qword ptr %1; fst qword ptr %0":"=m"(d):"m"(int64));
#endif
}
#ifdef WIN32
#pragma warning(default: 4035)
#endif
#else
inline short __fastcall swap_word(short a)
{
	return (a>>8)|((a & 0xFF)<<8);
}

inline long __fastcall swap_dword(long a)
{
	return (a>>24)|((a & 0xFF0000)>>8)|((a & 0xFF00)<<8)|((a & 0xFF)<<24);
}

inline void __fastcall int64todouble(void *int64,double *d)
{
	*d = ((double)(*(int*)int64))*4294967296.0+(double)(*(((int*)int64)+1));
}
#endif
//---------------------------------------------------------------------------
#endif
