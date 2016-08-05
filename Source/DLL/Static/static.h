// The DLL code
#pragma once
#include <windows.h>

#if CURRENT_PROJECT_ID == Static_PROJECT_ID
	#if Static_IS_STATIC
		#define Static_API
	#elif Static_IS_SHARED // || Static_IS_MODULE
		#define Static_API __declspec(dllexport)
	#elif Static_IS_MODULE // || Static_IS_MODULE
		#define Static_API __declspec(dllexport)
	#endif
#elif CURRENT_PROJECT_ID != Static_PROJECT_ID
	#if Static_IS_STATIC
		#define Static_API extern
	#elif Static_IS_SHARED // || Static_IS_MODULE
		#define Static_API __declspec(dllimport)
	#elif Static_IS_MODULE // || Static_IS_MODULE
		#error "__declspec(dllimport)" cannot be used to import symbols from modules.
	#endif
#endif

/*
#if Static_IS_STATIC
	#if CURRENT_PROJECT_ID == Static_PROJECT_ID
		#define Static_API
	#else
		#define Static_API extern
	#endif
#elif Static_IS_SHARED || Static_IS_MODULE
	#if CURRENT_PROJECT_ID == Static_PROJECT_ID
		#if COMPILING_SHARED || COMPILING_MODULE
			#define Static_API __declspec(dllexport)
		#else
			#define Static_API __declspec(dllimport)
		#endif
	
	#endif
#else
	#if COMPILING_SHARED || COMPILING_MODULE
		#define Static_API __declspec(dllimport)
	#else
		#define Static_API __declspec(dllimport)
	#endif
#endif
*/

Static_API DWORD dwTlsIndex; // address of shared memory

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

	Static_API BOOL WINAPI StoreDataStatic(DWORD dw);
	Static_API BOOL WINAPI GetDataStatic(DWORD *pdw);

#ifdef __cplusplus
}
#endif
