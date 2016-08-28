#pragma once
#include "Private/Core/Platforms/GenericPlatform.h"

//---------------------------------------------------------
// Identify the current platform and include that header
//---------------------------------------------------------

#if PLATFORM_WINDOWS
#include "Private/Core/Platforms/WindowsPlatform.h"
#elif PLATFORM_MACOS
#include "Private/Core/Platforms/MacPlatform.h"
#endif



//------------------------------------------------------------------
// Transfer the platform types to global types
//------------------------------------------------------------------

													// Unsigned base types.
typedef PlatformTypes::uint8			uint8;		///< An 8-bit unsigned integer.
typedef PlatformTypes::uint16			uint16;		///< A 16-bit unsigned integer.
typedef PlatformTypes::uint32			uint32;		///< A 32-bit unsigned integer.
typedef PlatformTypes::uint64			uint64;		///< A 64-bit unsigned integer.
													// Signed base types.
typedef	PlatformTypes::int8				int8;		///< An 8-bit signed integer.
typedef PlatformTypes::int16			int16;		///< A 16-bit signed integer.
typedef PlatformTypes::int32			int32;		///< A 32-bit signed integer.
typedef PlatformTypes::int64			int64;		///< A 64-bit signed integer.
													// Character types.
typedef PlatformTypes::ANSICHAR			ANSICHAR;	///< An ANSI character. Normally a signed type.
typedef PlatformTypes::WIDECHAR			WIDECHAR;	///< A wide character. Normally a signed type.
typedef PlatformTypes::TCHAR			TCHAR;		///< Either ANSICHAR or WIDECHAR, depending on whether the platform supports wide characters or the requirements of the licensee.
typedef PlatformTypes::CHAR8			UTF8CHAR;	///< An 8-bit character containing a UTF8 (Unicode, 8-bit, variable-width) code unit.
typedef PlatformTypes::CHAR16			UCS2CHAR;	///< A 16-bit character containing a UCS2 (Unicode, 16-bit, fixed-width) code unit, used for compatibility with 'Windows TCHAR' across multiple platforms.
typedef PlatformTypes::CHAR16			UTF16CHAR;	///< A 16-bit character containing a UTF16 (Unicode, 16-bit, variable-width) code unit.
typedef PlatformTypes::CHAR32			UTF32CHAR;	///< A 32-bit character containing a UTF32 (Unicode, 32-bit, fixed-width) code unit.

typedef PlatformTypes::SIZE_T			size_t;		///< An unsigned integer the same size as a pointer
typedef PlatformTypes::UPTRINT			uptrint;	///< An unsigned integer the same size as a pointer
typedef PlatformTypes::PTRINT			ptrint;		///< A signed integer the same size as a pointer
typedef PlatformTypes::TYPE_OF_NULL		TYPE_OF_NULL;	///< The type of the NULL constant.
typedef PlatformTypes::TYPE_OF_NULLPTR	TYPE_OF_NULLPTR; ///< The type of the C++ nullptr keyword.


/* Miscellaneous*/
#define _TCHAR_DEFINED
#define MAX_SPRINTF 1024
#undef WINVER
#define WINVER 0x603

#if COMPILER_GNU
#  define ATTR_NORETURN __attribute__((__noreturn__))
#  define ATTR_PRINTF(F, V) __attribute__ ((__format__ (__printf__, F, V)))
#  define ATTR_DEPRECATED __attribute__((__deprecated__))
#  define TRINITY_CONSTEXPR constexpr
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F, V)
#  define ATTR_DEPRECATED
#if _MSC_VER >= 1900
#  define TRINITY_CONSTEXPR constexpr
#else
#  define TRINITY_CONSTEXPR
#endif
#endif //COMPILER == COMPILER_GNU

// Database defines
#define QueryMaxParamCount 32

// Utility Macro
#define DISABLE_COPY(T) \
  T(const T&) = delete; \
  void operator=(const T&) = delete

#define DISABLE_COPY_AND_ASSIGN(T) \
  T(const T&) = delete; \
  void operator=(const T&) = delete