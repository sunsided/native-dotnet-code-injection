// Bootstrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>

static const LPCWSTR assemblyPath = L"D:\\dev\\Projekte\\various\\Code Injection\\CodeInject\\bin\\Release\\CodeInject.exe";
static const LPCWSTR classFqn = L"CodeInject.SomeClass";
static const LPCWSTR methodName = L"SomeMethod";
static const LPCWSTR parameter = L"MyParameter";

/// <summary>
/// Starts the dot net runtime.
/// </summary>
/// <remarks>http://www.codingthewheel.com/archives/how-to-inject-a-managed-assembly-dll/</remarks>
HRESULT StartTheDotNetRuntime(_In_ LPCTSTR lpCommand)
{
	FILE *file;
	fopen_s(&file, "d:\\dev\\temp2.txt", "a+");

	fprintf(file, "binding runtime.\r\n");
	fflush(file);

    fprintf(file, "Loading the .NET runtime host.\n");
	fflush(file);
      
	ICLRMetaHost *pMetaHost = NULL;
	auto result = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	if (FAILED(result))
	{
		fprintf(file, "Error: failed to create CLR instance.\n");
		fflush(file);
		
		return result;
	}
 
	fprintf(file, "Loading the .NET runtime.\n");
	fflush(file);

	ICLRRuntimeInfo *pRuntimeInfo = NULL;
	result = pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&pRuntimeInfo));
	if (FAILED(result))
	{
		fprintf(file, "Error: failed to create CLR instance.\n");
		fflush(file);

		pMetaHost->Release();
		return result;
	}
 
	fprintf(file, "Acquiring the .NET runtime.\n");
	fflush(file);

	ICLRRuntimeHost *pClrRuntimeHost = NULL;
	result = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pClrRuntimeHost));
	if (FAILED(result))
	{
		fprintf(file, "Error: failed to acquire CLR runtime.\n");
		fflush(file);

		pMetaHost->Release();
		return result;
	}

	fprintf(file, "Starting the .NET runtime.\n");
	fflush(file);

	result = pClrRuntimeHost->Start();
	if (FAILED(result))
	{
		fprintf(file, "Error: failed to start CLR runtime.\n");
		fflush(file);

		pClrRuntimeHost->Release();
		pMetaHost->Release();
		return result;
	}

	fprintf(file, "Executing payload assembly.\n");
	fflush(file);
    DWORD dwRet = 0;
    result = pClrRuntimeHost->ExecuteInDefaultAppDomain(
            assemblyPath,
            classFqn, methodName, parameter, &dwRet);
	if (FAILED(result))
	{
		fprintf(file, "Error: unable to execute example code.\n");
		fflush(file);

		pClrRuntimeHost->Stop();
		pClrRuntimeHost->Release();
		pMetaHost->Release();
		return result;
	}

	fprintf(file, "Stopping the .NET runtime.\n");
	fflush(file);

	pClrRuntimeHost->Stop();

	fprintf(file, "Releasing the .NET runtime.\n");
	fflush(file);

	pClrRuntimeHost->Release();
	pMetaHost->Release();

	fclose(file);

	return ERROR_SUCCESS;
}