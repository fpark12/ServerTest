#pragma once

/**
* Windows specific types
**/
struct WindowsPlatformTypes : public GenericPlatformTypes
{
	#ifdef _WIN64
	typedef std::int64_t		SIZE_T;
	#else
	typedef std::int32_t		SIZE_T;
	#endif
};

typedef WindowsPlatformTypes PlatformTypes;

// Base defines, must define these for the platform, there are no defaults
#define PLATFORM_DESKTOP				1
#if defined( _WIN64 )
#define PLATFORM_64BITS					1
#else
#define PLATFORM_64BITS					0
#endif
#if defined( _MANAGED ) || defined( _M_CEE )
#define PLATFORM_COMPILER_COMMON_LANGUAGE_RUNTIME_COMPILATION	1
#endif

#define PLATFORM_LITTLE_ENDIAN						1

// Function type macros.
#define VARARGS     __cdecl					/* Functions with variable arguments */
#ifdef CDECL
#undef CDECL
#define CDECL	    __cdecl					/* Standard C function */
#endif
#define STDCALL		__stdcall				/* Standard calling convention */
#define FORCEINLINE __forceinline			/* Force code to be inline */
#define FORCENOINLINE __declspec(noinline)	/* Force code to NOT be inline */

// DLL export and import definitions
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#define OVERRIDE override

#if PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX MAX_PATH
#  define _USE_MATH_DEFINES
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#  ifndef DECLSPEC_DEPRECATED
#    define DECLSPEC_DEPRECATED __declspec(deprecated)
#  endif //DECLSPEC_DEPRECATED
#else //PLATFORM != PLATFORM_WINDOWS
#  define TRINITY_PATH_MAX PATH_MAX
#  define DECLSPEC_NORETURN
#  define DECLSPEC_DEPRECATED
#endif //PLATFORM