// The DLL code
#pragma once
#include <windows.h>

#ifdef _SHARED
#define MY_EXPORTS __declspec(dllexport)
#else
#define MY_EXPORTS __declspec(dllimport)
#endif

static DWORD dwTlsIndex; // address of shared memory

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

	MY_EXPORTS BOOL WINAPI StoreData(DWORD dw);
	MY_EXPORTS BOOL WINAPI GetData(DWORD *pdw);

#ifdef __cplusplus
}
#endif
