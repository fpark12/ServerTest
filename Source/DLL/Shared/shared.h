// The DLL code
#pragma once
#include <windows.h>

#if Shared_IS_STATIC
    #if COMPILING_STATIC
        #define Shared_EXPORTS
    #else
        #define Shared_EXPORTS extern
    #endif
#elif Shared_IS_SHARED || Shared_IS_MODULE
    #if COMPILING_SHARED || COMPILING_MODULE
        #undef Shared_EXPORTS
        #define Shared_EXPORTS __declspec(dllexport)
    #else
        #undef Shared_EXPORTS
        #define Shared_EXPORTS __declspec(dllimport)
    #endif
#else
#endif

						 // DllMain() is the entry-point function for this DLL. 

BOOL WINAPI DllMain(HINSTANCE hinstDLL, // DLL module handle
	DWORD fdwReason,                    // reason called
	LPVOID lpvReserved);                 // reserved

// The export mechanism used here is the __declspec(export)
// method supported by Microsoft Visual Studio, but any
// other export method supported by your development
// environment may be substituted.

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

    Shared_EXPORTS BOOL WINAPI StoreData(DWORD dw);
    Shared_EXPORTS BOOL WINAPI GetData(DWORD *pdw);

#ifdef __cplusplus
}
#endif
