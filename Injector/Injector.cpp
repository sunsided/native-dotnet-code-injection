// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

static const LPCWSTR bootstrapDllName = L"Bootstrapper.dll";

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

// http://blog.opensecurityresearch.com/2013/01/windows-dll-injection-basics.html
// http://resources.infosecinstitute.com/using-createremotethread-for-dll-injection-on-windows/
int _tmain(int argc, _TCHAR* argv[])
{
	// acquire debug privileges
	// this should only be required in order to inject into processes of other users
	SetDebugPrivilege();

	// fetch the process ID
	const DWORD procID = 6536;

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
	
	// allocate memory for the DLL path in the remote process
	auto dllPathAddr = VirtualAllocEx(hProcess, 
                             0, 
                             sizeof(dllPath),
                             MEM_RESERVE|MEM_COMMIT, 
                             PAGE_EXECUTE_READWRITE);
	if(dllPathAddr == NULL) 
	{
		cerr << "Error: the memory could not be allocated inside the chosen process." << endl;
	}
 
	// write the DLL name into the remote process
	auto n = WriteProcessMemory(hProcess, 
                   dllPathAddr, 
                   dllPath, 
                   sizeof(dllPath),
                   NULL);
	if(n == 0) 
	{
		cerr << "Error: there was no bytes written to the process's address space." << endl;
	}

	// testing success
	static CHAR testBuffer[MAX_PATH] = "";
	SIZE_T bytesRead;
	n = ReadProcessMemory(hProcess, 
                   dllPathAddr, 
                   testBuffer, 
                   sizeof(testBuffer),
                   &bytesRead);
	if(n == 0) 
	{
		cerr << "Error: failed to read back process memory." << endl;
	}

	// determine the starting address of LoadLibraryA (same for all processes)
	auto loadLibAddr = GetProcAddress(GetModuleHandleW(TEXT("Kernel32")), "LoadLibraryW");
	if(loadLibAddr == NULL) 
	{
		cerr << "Error: the LoadLibrary[A/W] function was not found inside kernel32.dll library." << endl;
	}

	// execute LoadLibraryA in the remote process and pass the pointer to the DLL path
	auto rThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, dllPathAddr, 0, NULL);
	if(rThread == NULL) 
	{
		// NOTE: Error 5 (ERROR_ACCESS_DENIED) can happen if a x86 process tries to access an x64
		auto error = GetLastError();
		cerr << "Error: the remote thread could not be created. Code: " << hex << error << "." << endl;
	}
	else 
	{
		cout << "Success: the remote thread was successfully created." << endl;
	}

	// wait for the thread to end
	auto result = WaitForSingleObject(rThread, INFINITE);
	if (result == WAIT_FAILED)
	{
		auto error = GetLastError();
		cerr << "Error: waiting for the thread has failed. Code: " << hex << error << "." << endl;
	}

	// get HMODULE of the loaded dll
#if 0 // this won't work on 64bit Windows due to result code truncation
	DWORD   hLibModule;
	result = GetExitCodeThread(rThread, &hLibModule);
	if (result == STILL_ACTIVE)
	{
		cout << "Success: found injected DLL." << endl;
	}
	else if (result != 0)
	{
		auto error = GetLastError();
		cerr << "Error: unable to get HMODULE of injected DLL. Code: " << hex << error << "." << endl;
	}
#endif

	// wait for a keypress
	cout << "Press RETURN to continue." << endl;
	cin.ignore();

	// free the virtual memory
	VirtualFreeEx(hProcess, dllPathAddr, 0, MEM_RELEASE);

	// close the handle
	CloseHandle(hProcess);

	return 0;
}

