//---------------------------------------------------------------------------
//
//  MadTracker System Core
//
//		Platforms:	All
//		Processors:	All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTXSYSTYPES_INCLUDED
#define MTXSYSTYPES_INCLUDED
//---------------------------------------------------------------------------

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
#	include <config_types.h>

#endif

//---------------------------------------------------------------------------
#endif
