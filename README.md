native-dotnet-code-injection
============================

Injection of managed code into non-managed Windows applications.

Injects a native bootstrap DLL into the target process by calling `LoadLibrary` as a remote thread and then calls
the initialization method of the .NET runtime (hosting API). After that, a .NET assembly is loaded into the default
application domain and executed.

## Projects ##

- Injector: A native console application that performs the actual injection task.
- Bootstrapper: The injected native DLL that provides the runtime initialization functionality.
- CodeInject: A managed assembly written in C# that provides the code to be executed in the native application.

## Test Projects ##

- HostingTest: A native console application that starts the .NET runtime and directly executes the `CodeInject` assembly code.
