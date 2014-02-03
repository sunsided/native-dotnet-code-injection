// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

static const LPCWSTR bootstrapDllName = L"Bootstrapper.dll";

void SetDebugPrivilege();
DWORD_PTR Inject(const HANDLE hProcess, const LPVOID function, const wstring& argument);
DWORD_PTR GetRemoteModuleHandle(const int processId, const wchar_t* moduleName);
DWORD_PTR GetFunctionOffset(const wstring& library, const char* functionName);

// http://blog.opensecurityresearch.com/2013/01/windows-dll-injection-basics.html
// http://resources.infosecinstitute.com/using-createremotethread-for-dll-injection-on-windows/
int _tmain(int argc, _TCHAR* argv[])
{
	// acquire debug privileges
	// this should only be required in order to inject into processes of other users
	SetDebugPrivilege();

	// fetch the process ID
	const DWORD procID = 9612;

	// get a handle to the running process
	auto hProcess = OpenProcess( PROCESS_CREATE_THREAD | 
                       PROCESS_QUERY_INFORMATION | 
                       PROCESS_VM_OPERATION | 
                       PROCESS_VM_WRITE | 
                       PROCESS_VM_READ, 
                       FALSE, 
                       procID );
	if(hProcess == NULL) 
	{
		cerr << "Error: the specified process couldn't be found." << endl;
	}

	// get the full path to the bootstrap DLL
	static TCHAR dllPath[MAX_PATH] = L"";
	auto length = GetFullPathNameW(bootstrapDllName, 
									MAX_PATH, 
									dllPath, //Output to save the full DLL path
									NULL);
	if(length == 0) 
	{
		// TODO: "If the lpBuffer buffer is too small to contain the path, the return value is the size, in TCHARs, of the buffer that is required to hold the path and the terminating null character."
		cerr << "Error: could not determine DLL path." << endl;
	}
	else
	{
		cout << "Determined DLL path: ";
		wcout << dllPath << endl;
	}

	// TODO: Make sure that the DLL really exists

	// determine the starting address of LoadLibraryA (same for all processes)
	auto loadLibAddr = GetProcAddress(GetModuleHandleW(TEXT("Kernel32")), "LoadLibraryW");
	if(loadLibAddr == NULL) 
	{
		cerr << "Error: the LoadLibrary[A/W] function was not found inside kernel32.dll library." << endl;
	}
	
	// inject the bootstrap DLL
	Inject(hProcess, loadLibAddr, dllPath);

	// add the function offset to the base of the module in the remote process
    DWORD_PTR hBootstrap = GetRemoteModuleHandle(procID, bootstrapDllName);
    DWORD_PTR offset = GetFunctionOffset(dllPath, "StartTheDotNetRuntime");
    DWORD_PTR fnImplant = hBootstrap + offset;

	// build argument; use DELIM as tokenizer
    wstring argument = L"LOL";

	// wait for a keypress
	cout << "Press RETURN to continue." << endl;
	cin.ignore();
	
	// close the handle
	CloseHandle(hProcess);

	return 0;
}

void SetDebugPrivilege() 
{
	auto hProcess = GetCurrentProcess();
	if (hProcess == NULL)
	{
		cerr << "Error: unable to determine the current process." << endl;
	}

	HANDLE hToken;
	auto result = OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken);
	if(result == 0) 
	{
		cerr << "Error: unable to open the current process." << endl;
	}

	LUID luid;
	result = LookupPrivilegeValue(0, L"seDebugPrivilege", &luid);
	if(result == 0) 
	{
		cerr << "Error: unable to lookup debug privilege." << endl;
	}

	TOKEN_PRIVILEGES priv;
    priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = luid;
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    result = AdjustTokenPrivileges(hToken, false, &priv, 0, 0, 0);
	if(result != ERROR_SUCCESS) 
	{
		cerr << "Error: unable to adjust debug token privilege." << endl;
	}

	CloseHandle(hToken);
	CloseHandle(hProcess);
}

DWORD_PTR Inject(const HANDLE hProcess, const LPVOID function, const wstring& argument)
{
    // allocate some memory in remote process
	LPVOID baseAddress = VirtualAllocEx(hProcess, NULL, sizeof(TCHAR)*argument.length(), 
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    // write argument into remote process	
    BOOL isSucceeded = WriteProcessMemory(hProcess, baseAddress, argument.c_str(),  
		sizeof(TCHAR)*argument.length(), NULL);
    
    // make the remote process invoke the function
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
        (LPTHREAD_START_ROUTINE)function, baseAddress, NULL, 0);
    
    // wait for thread to exit
    WaitForSingleObject(hThread, INFINITE);
    
    // free memory in remote process
    VirtualFreeEx(hProcess, baseAddress, 0, MEM_RELEASE);
    
    // get the thread exit code
    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);
    
    // close thread handle
    CloseHandle(hThread);
    
    // return the exit code
    return exitCode;
}

DWORD_PTR GetRemoteModuleHandle(const int processId, const wchar_t* moduleName)
{
    MODULEENTRY32 me32; 
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    
    // get snapshot of all modules in the remote process 
    me32.dwSize = sizeof(MODULEENTRY32); 
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    
    // can we start looking?
    if (!Module32First(hSnapshot, &me32)) 
    {
    	CloseHandle(hSnapshot);
    	return 0;
    }
    
    // enumerate all modules till we find the one we are looking for 
    // or until every one of them is checked
    while (wcscmp(me32.szModule, moduleName) != 0 && Module32Next(hSnapshot, &me32));
    
    // close the handle
    CloseHandle(hSnapshot);
    
    // check if module handle was found and return it
    if (wcscmp(me32.szModule, moduleName) == 0)
    	return (DWORD_PTR)me32.modBaseAddr;
    
    return 0;
}

DWORD_PTR GetFunctionOffset(const wstring& library, const char* functionName)
{
    // load library into this process
    HMODULE hLoaded = LoadLibraryW(library.c_str());
    
    // get address of function to invoke
    void* lpInject = GetProcAddress(hLoaded, functionName);
    
    // compute the distance between the base address and the function to invoke
    DWORD_PTR offset = (DWORD_PTR)lpInject - (DWORD_PTR)hLoaded;
    
    // unload library from this process
    FreeLibrary(hLoaded);
    
    // return the offset to the function
    return offset;
}