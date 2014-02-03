// Bootstrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MSCorEE.h"

static const LPCWSTR assemblyPath = L"D:\dev\Projekte\various\Code Injection\CodeInject\bin\Debug\CodeInject.exe";
static const LPCWSTR classFqn = L"CodeInject.SomeClass";
static const LPCWSTR methodName = L"SomeMethod";
static const LPCWSTR parameter = L"MyParameter";

/// <summary>
/// Starts the dot net runtime.
/// </summary>
/// <remarks>http://www.codingthewheel.com/archives/how-to-inject-a-managed-assembly-dll/</remarks>
void StartTheDotNetRuntime()
{
    // Bind to the CLR runtime..
    ICLRRuntimeHost *pClrHost = NULL;
    HRESULT hr = CorBindToRuntimeEx(
        NULL, L"wks", 0, CLSID_CLRRuntimeHost,
        IID_ICLRRuntimeHost, (PVOID*)&pClrHost);

    // Push the big START button shown above
    hr = pClrHost->Start();

    // Okay, the CLR is up and running in this (previously native) process.
    // Now call a method on our managed C# class library.
    DWORD dwRet = 0;
    hr = pClrHost->ExecuteInDefaultAppDomain(
        assemblyPath,
        classFqn, methodName, parameter, &dwRet);

    // Optionally stop the CLR runtime (we could also leave it running)
    hr = pClrHost->Stop();

    // Don’t forget to clean up.
    pClrHost->Release();
}