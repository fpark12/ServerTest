// The DLL code
#pragma once
#include <windows.h>

constexpr int macro_strcmp(char const* lhs, char const* rhs);
// compares two strings in compile time constant fashion
constexpr int macro_strcmp(char const* lhs, char const* rhs) {
    return (('\0' == lhs[0]) && ('\0' == rhs[0])) ? 0
        : (lhs[0] != rhs[0]) ? (lhs[0] - rhs[0])
        : macro_strcmp(lhs + 1, rhs + 1);
}
// some compilers may require ((int)lhs[0] - (int)rhs[0])

#define JACK "jack"
#define QUEEN "queen"

#define USER JACK       // or QUEEN, your choice

#if 0 == macro_strcmp( USER, JACK )
#define USER_VS QUEEN
#elif 0 == macro_strcmp( USER, QUEEN )
#define USER_VS JACK
#else
#define USER_VS "unknown"
#endif

#if Static_IS_STATIC
    #if COMPILING_STATIC
        #define Static_EXPORTS
    #else
        #define Static_EXPORTS extern
    #endif
#elif Static_IS_SHARED || Static_IS_MODULE
#if 0 == macro_strcmp( PROJECT_NAME, "Static")
    #if COMPILING_SHARED || COMPILING_MODULE
    #undef Static_EXPORTS
    #define Static_EXPORTS __declspec(dllexport)
    #else
    #undef Static_EXPORTS
    #define Static_EXPORTS __declspec(dllimport)
    #endif
#else
    #if COMPILING_SHARED || COMPILING_MODULE
    #undef Static_EXPORTS
    #define Static_EXPORTS __declspec(dllexport)
    #else
    #undef Static_EXPORTS
    #define Static_EXPORTS __declspec(dllimport)
    #endif
#endif

#else
#endif

Static_EXPORTS DWORD dwTlsIndex; // address of shared memory

						 // DllMainStatic() is the entry-point function for this DLL. 

BOOL WINAPI DllMainStatic(HINSTANCE hinstDLL, // DLL module handle
	DWORD fdwReason,                    // reason called
	LPVOID lpvReserved);                 // reserved

										 // The export mechanism used here is the __declspec(export)
										 // method supported by Microsoft Visual Studio, but any
										 // other export method supported by your development
										 // environment may be substituted.

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

    Static_EXPORTS BOOL WINAPI StoreDataStatic(DWORD dw);
    Static_EXPORTS BOOL WINAPI GetDataStatic(DWORD *pdw);

#ifdef __cplusplus
}
#endif
