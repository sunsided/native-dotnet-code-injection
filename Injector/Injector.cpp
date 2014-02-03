// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BUFSIZE 4096 //!< maximum length of the DLL path

int _tmain(int argc, _TCHAR* argv[])
{
	// fetch the process ID
	DWORD procID = 4584;

	// get a handle to the running process
	auto hHandle = OpenProcess( PROCESS_CREATE_THREAD | 
                       PROCESS_QUERY_INFORMATION | 
                       PROCESS_VM_OPERATION | 
                       PROCESS_VM_WRITE | 
                       PROCESS_VM_READ, 
                       FALSE, 
                       procID );

	// get the full path to the bootstrap dll
	TCHAR dllPath[BUFSIZE] = TEXT("");
	GetFullPathName(TEXT("bootstrapper.dll"), 
                BUFSIZE, 
                dllPath, //Output to save the full DLL path
                NULL);

	// close the handle
	CloseHandle(hHandle);

	return 0;
}

