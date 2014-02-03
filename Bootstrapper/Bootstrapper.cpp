// Bootstrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>

#include "MSCorEE.h"

static const LPCWSTR assemblyPath = L"D:\\dev\\Projekte\\various\\Code Injection\\CodeInject\\bin\\Release\\CodeInject.exe";
static const LPCWSTR classFqn = L"CodeInject.SomeClass";
static const LPCWSTR methodName = L"SomeMethod";
static const LPCWSTR parameter = L"MyParameter";

/// <summary>
/// Starts the dot net runtime.
/// </summary>
/// <remarks>http://www.codingthewheel.com/archives/how-to-inject-a-managed-assembly-dll/</remarks>
void StartTheDotNetRuntime()
{
	FILE *file;
	fopen_s(&file, "d:\\dev\\temp2.txt", "a+");

	fprintf(file, "binding runtime.\r\n");
	fflush(file);

    // Bind to the CLR runtime..
    ICLRRuntimeHost *pClrHost = NULL;
    HRESULT hr = CorBindToRuntimeEx(
        NULL, L"wks", 0, CLSID_CLRRuntimeHost,
        IID_ICLRRuntimeHost, (PVOID*)&pClrHost);

#if 1

	fprintf(file, "starting runtime.\r\n");
	fflush(file);

    // Push the big START button shown above
    hr = pClrHost->Start();

	fprintf(file, "executing remote code.\r\n");
	fflush(file);

    // Okay, the CLR is up and running in this (previously native) process.
    // Now call a method on our managed C# class library.
    DWORD dwRet = 0;
    hr = pClrHost->ExecuteInDefaultAppDomain(
        assemblyPath,
        classFqn, methodName, parameter, &dwRet);

	fprintf(file, "stopping runtime.\r\n");
	fflush(file);

    // Optionally stop the CLR runtime (we could also leave it running)
    hr = pClrHost->Stop();

#endif

	fprintf(file, "releasing runtime.\r\n");
	fflush(file);

    // Don’t forget to clean up.
    pClrHost->Release();

	fclose(file);
}