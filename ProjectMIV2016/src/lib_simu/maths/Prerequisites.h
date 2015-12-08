
#ifndef __Prerequisites_H__
#define __Prerequisites_H__

// Needed for OGRE_WCHAR_T_STRINGS below
#include <string>
#include <assert.h>

namespace Maths{

	//static link...	
	#define OGRE_STATIC_LIB

	//content of Platform.h file of Ogre source code
	/* Initial platform/compiler-related stuff to set.
	*/
	#define OGRE_PLATFORM_WIN32 1
	#define OGRE_PLATFORM_LINUX 2
	#define OGRE_PLATFORM_APPLE 3

	#define OGRE_COMPILER_MSVC 1
	#define OGRE_COMPILER_GNUC 2
	#define OGRE_COMPILER_BORL 3

	#define OGRE_ENDIAN_LITTLE 1
	#define OGRE_ENDIAN_BIG 2

	#define OGRE_ARCHITECTURE_32 1
	#define OGRE_ARCHITECTURE_64 2

	/* Finds the compiler type and version.
	*/
	#if defined( _MSC_VER )
	#   define OGRE_COMPILER OGRE_COMPILER_MSVC
	#   define OGRE_COMP_VER _MSC_VER

	#elif defined( __GNUC__ )
	#   define OGRE_COMPILER OGRE_COMPILER_GNUC
	#   define OGRE_COMP_VER (((__GNUC__)*100) + \
			(__GNUC_MINOR__*10) + \
			__GNUC_PATCHLEVEL__)

	#elif defined( __BORLANDC__ )
	#   define OGRE_COMPILER OGRE_COMPILER_BORL
	#   define OGRE_COMP_VER __BCPLUSPLUS__
	#   define __FUNCTION__ __FUNC__ 
	#else
	#   pragma error "No known compiler. Abort! Abort!"

	#endif

	/* See if we can use __forceinline or if we need to use __inline instead */
	#if OGRE_COMPILER == OGRE_COMPILER_MSVC
	#   if OGRE_COMP_VER >= 1200
	#       define FORCEINLINE __forceinline
	#   endif
	#elif defined(__MINGW32__)
	#   if !defined(FORCEINLINE)
	#       define FORCEINLINE __inline
	#   endif
	#else
	#   define FORCEINLINE __inline
	#endif

	/* Finds the current platform */

	#if defined( __WIN32__ ) || defined( _WIN32 )
	#   define OGRE_PLATFORM OGRE_PLATFORM_WIN32

	#elif defined( __APPLE_CC__)
	#   define OGRE_PLATFORM OGRE_PLATFORM_APPLE

	#else
	#   define OGRE_PLATFORM OGRE_PLATFORM_LINUX
	#endif

		/* Find the arch type */
	#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
	#   define OGRE_ARCH_TYPE OGRE_ARCHITECTURE_64
	#else
	#   define OGRE_ARCH_TYPE OGRE_ARCHITECTURE_32
	#endif

	// For generating compiler warnings - should work on any compiler
	// As a side note, if you start your message with 'Warning: ', the MSVC
	// IDE actually does catch a warning :)
	#define OGRE_QUOTE_INPLACE(x) # x
	#define OGRE_QUOTE(x) OGRE_QUOTE_INPLACE(x)
	#define OGRE_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

	//----------------------------------------------------------------------------
	// Windows Settings
	#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

	// If we're not including this from a client build, specify that the stuff
	// should get exported. Otherwise, import it.
	#	if defined( OGRE_STATIC_LIB )
			// Linux compilers don't have symbol import/export directives.
	#   	define _OgreExport
	#   	define _OgrePrivate
	#   else
	#   	if defined( OGRE_NONCLIENT_BUILD )
	#       	define _OgreExport __declspec( dllexport )
	#   	else
	#           if defined( __MINGW32__ )
	#               define _OgreExport
	#           else
	#       	    define _OgreExport __declspec( dllimport )
	#           endif
	#   	endif
	#   	define _OgrePrivate
	#	endif
	// Win32 compilers use _DEBUG for specifying debug builds.
	#   ifdef _DEBUG
	#       define OGRE_DEBUG_MODE 1
	#   else
	#       define OGRE_DEBUG_MODE 0
	#   endif

	// Disable unicode support on MingW at the moment, poorly supported in stdlibc++
	// STLPORT fixes this though so allow if found
	// MinGW C++ Toolkit supports unicode and sets the define __MINGW32_TOOLKIT_UNICODE__ in _mingw.h
	#if defined( __MINGW32__ ) && !defined(_STLPORT_VERSION)
	#   include<_mingw.h>
	#   if defined(__MINGW32_TOOLBOX_UNICODE__)
	#	    define OGRE_UNICODE_SUPPORT 1
	#   else
	#       define OGRE_UNICODE_SUPPORT 0
	#   endif
	#else
	#	define OGRE_UNICODE_SUPPORT 1
	#endif

	#endif
	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	// Linux/Apple Settings
	#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE

	// Enable GCC symbol visibility
	#   if defined( OGRE_GCC_VISIBILITY )
	#       define _OgreExport  __attribute__ ((visibility("default")))
	#       define _OgrePrivate __attribute__ ((visibility("hidden")))
	#   else
	#       define _OgreExport
	#       define _OgrePrivate
	#   endif

	// A quick define to overcome different names for the same function
	#   define stricmp strcasecmp

	// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
	// specifying a debug build.
	// (??? this is wrong, on Linux debug builds aren't marked in any way unless
	// you mark it yourself any way you like it -- zap ???)
	#   ifdef DEBUG
	#       define OGRE_DEBUG_MODE 1
	#   else
	#       define OGRE_DEBUG_MODE 0
	#   endif

	#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		#define OGRE_PLATFORM_LIB "OgrePlatform.bundle"
	#else
		//OGRE_PLATFORM_LINUX
		#define OGRE_PLATFORM_LIB "libOgrePlatform.so"
	#endif

	// Always enable unicode support for the moment
	// Perhaps disable in old versions of gcc if necessary
	#define OGRE_UNICODE_SUPPORT 1

	#endif

	//For apple, we always have a custom config.h file
	#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	#    include "config.h"
	#endif

	//----------------------------------------------------------------------------

	//----------------------------------------------------------------------------
	// Endian Settings
	// check for BIG_ENDIAN config flag, set OGRE_ENDIAN correctly
	#ifdef OGRE_CONFIG_BIG_ENDIAN
	#    define OGRE_ENDIAN OGRE_ENDIAN_BIG
	#else
	#    define OGRE_ENDIAN OGRE_ENDIAN_LITTLE
	#endif

	// Integer formats of fixed bit width
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	// define uint64 type
	#if OGRE_COMPILER == OGRE_COMPILER_MSVC
		typedef unsigned __int64 uint64;
	#else
		typedef unsigned long long uint64;
	#endif

	// define the real number values to be used
	// default to use 'float' unless precompiler option set
	#if OGRE_DOUBLE_PRECISION == 1
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
		typedef double Real;
	#else
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
		typedef float Real;
	#endif

	#if OGRE_COMPILER == OGRE_COMPILER_GNUC && OGRE_COMP_VER >= 310 && !defined(STLPORT)
	#   if OGRE_COMP_VER >= 430
	#       define HashMap ::std::tr1::unordered_map
	#		define HashSet ::std::tr1::unordered_set
	#    else
	#       define HashMap ::__gnu_cxx::hash_map
	#       define HashSet ::__gnu_cxx::hash_set
	#    endif
	#else
	#   if OGRE_COMPILER == OGRE_COMPILER_MSVC
	#       if OGRE_COMP_VER > 1300 && !defined(_STLP_MSVC)
	#           define HashMap ::stdext::hash_map
	#           define HashSet ::stdext::hash_set
	#       else
	#           define HashMap ::std::hash_map
	#           define HashMap ::std::hash_set
	#       endif
	#   else
	#       define HashMap ::std::hash_map
	#       define HashMap ::std::hash_set
	#   endif
	#endif

	/** In order to avoid finger-aches :)
	*/
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

	#if OGRE_WCHAR_T_STRINGS
		typedef std::wstring _StringBase;
	#else
		typedef std::string _StringBase;
	#endif

	typedef _StringBase String;

} // namespace Maths 

#endif // __Prerequisites_H__


