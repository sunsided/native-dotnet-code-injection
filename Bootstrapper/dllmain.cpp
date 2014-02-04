// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdio.h>

#include "Bootstrapper.h"

#define LOG_ENABLED 0

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
#if LOG_ENABLED
	FILE *file;
	fopen_s(&file, "d:\\dev\\temp.txt", "a+");
#endif

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if LOG_ENABLED
		fprintf(file, "DLL attach function called.\r\n");
#endif
		 // NOTE: StartTheDotNetRuntime() cannot be called here as this will result in a loader lock.
		break;
	case DLL_THREAD_ATTACH:
#if LOG_ENABLED
		fprintf(file, "DLL thread attach function called.\r\n");
#endif
		break;
	case DLL_THREAD_DETACH:
#if LOG_ENABLED
		fprintf(file, "DLL thread detach function called.\r\n");
#endif
		break;
	case DLL_PROCESS_DETACH:
#if LOG_ENABLED
		fprintf(file, "DLL detach function called.\r\n");
#endfi
		break;
	}

	/* close file */
#if LOG_ENABLED
	fclose(file);
#endif
 
	return TRUE;
}

