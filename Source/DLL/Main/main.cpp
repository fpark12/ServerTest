#include <windows.h> 
#include <stdio.h> 

#define THREADCOUNT 1 
#ifdef _DEBUG
#define DLL_NAME(x) #x "-debug"
#else
#define DLL_NAME(x) #x
#endif



#ifndef EXAMPLEAPI
#	ifdef IS_STATIC // Compiling .lib
#		define EXAMPLEAPI
#	else
#		ifdef IS_DYNAMIC // Compiling .dll
#			if EXPORT_ID == PROJECT_ID
#				define EXAMPLEAPI __declspec(dllexport)
#			else
#				define EXAMPLEAPI __declspec(dllimport)
#			endif
#		else // Compiling .exe
#			define EXAMPLEAPI
#		endif
#	endif
#endif

VOID ErrorExit(LPSTR);

// extern "C" BOOL WINAPI StoreDataFunc(DWORD dw);
// extern "C" BOOL WINAPI GetDataFunc(DWORD *pdw);
typedef BOOL(WINAPI *StoreDataFunc)(DWORD dw);
typedef BOOL(WINAPI *GetDataFunc)(DWORD *pdw);

StoreDataFunc StoreData;
GetDataFunc GetData;

DWORD WINAPI ThreadFunc(VOID) {
	int i;

	if (!StoreData(GetCurrentThreadId()))
		ErrorExit("StoreData error");

	for (i = 0; i < THREADCOUNT; i++) {
		DWORD dwOut;
		if (!GetData(&dwOut))
			ErrorExit("GetData error");
		if (dwOut != GetCurrentThreadId())
			printf("thread %d: data is incorrect (%d)\n", GetCurrentThreadId(), dwOut);
		else printf("thread %d: data is correct\n", GetCurrentThreadId());
		Sleep(0);
	}
	return 0;
}

int main(VOID) {
	DWORD IDThread;
	HANDLE hThread[THREADCOUNT];
	int i;
	HINSTANCE hDLL;

	// Load the DLL
	const char* a = DLL_NAME(Shared);
	hDLL = LoadLibrary(a);
	if (!hDLL) {
		ErrorExit("DLL failed to load");
	}

	if (hDLL != NULL)
	{
		StoreData = (StoreDataFunc)GetProcAddress(hDLL,
			"StoreData");
		if (!StoreData)
		{
			// handle the error
			FreeLibrary(hDLL);
			return 1;
		}

		GetData = (GetDataFunc)GetProcAddress(hDLL,
			"GetData");
		if (!GetData)
		{
			// handle the error
			FreeLibrary(hDLL);
			return 1;
		}
	}

	// Create multiple threads. 

	for (i = 0; i < THREADCOUNT; i++) {
		hThread[i] = CreateThread(NULL, // default security attributes 
								  0,                           // use default stack size 
								  (LPTHREAD_START_ROUTINE)ThreadFunc, // thread function 
								  NULL,                    // no thread function argument 
								  0,                       // use default creation flags 
								  &IDThread);              // returns thread identifier 

														   // Check the return value for success. 
		if (hThread[i] == NULL)
			ErrorExit("CreateThread error\n");
	}

	WaitForMultipleObjects(THREADCOUNT, hThread, TRUE, INFINITE);

	FreeLibrary(hDLL);

	return 0;
}

VOID ErrorExit(LPSTR lpszMessage) {
	fprintf(stderr, "%s\n", lpszMessage);
	ExitProcess(0);
}
