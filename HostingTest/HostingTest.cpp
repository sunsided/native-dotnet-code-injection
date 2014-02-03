// HostingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

static const LPCWSTR assemblyPath = L"D:\\dev\\Projekte\\various\\Code Injection\\CodeInject\\bin\\Release\\CodeInject.exe";
static const LPCWSTR classFqn = L"CodeInject.SomeClass";
static const LPCWSTR methodName = L"SomeMethod";
static const LPCWSTR parameter = L"MyParameter";

int _tmain(int argc, _TCHAR* argv[])
{
	wprintf(L"Loading the .NET runtime host.\n");
      
	ICLRMetaHost *pMetaHost = NULL;
	auto result = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);
	if (FAILED(result))
	{
		wprintf(L"Error: failed to create CLR instance.\n");
	}
 
	wprintf(L"Loading the .NET runtime.\n");

	ICLRRuntimeInfo *pRuntimeInfo = NULL;
	result = pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&pRuntimeInfo);
	if (FAILED(result))
	{
		wprintf(L"Error: failed to create CLR instance.\n");
	}
 
	wprintf(L"Acquiring the .NET runtime.\n");

	ICLRRuntimeHost *pClrRuntimeHost = NULL;
	pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&pClrRuntimeHost);
 
	wprintf(L"Starting the .NET runtime.\n");
	pClrRuntimeHost->Start();

	wprintf(L"Executing payload assembly.\n");
    DWORD dwRet = 0;
    result = pClrRuntimeHost->ExecuteInDefaultAppDomain(
            assemblyPath,
            classFqn, methodName, parameter, &dwRet);
	if (FAILED(result))
	{
		wprintf(L"Error: unable to execute example code.\n");
	}

	wprintf(L"Stopping the .NET runtime.\n");
	pClrRuntimeHost->Stop();
	pClrRuntimeHost->Release();

	wprintf(L"Releasing the .NET runtime.\n");
	pMetaHost->Release();

	return 0;
}

