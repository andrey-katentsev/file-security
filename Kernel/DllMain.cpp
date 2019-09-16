#include "KAA/include/dll/module_context.h"

KAA::dll::module_context core_dll;

// When the system calls the DllMain function with the DLL_PROCESS_ATTACH value, the function returns TRUE if it succeeds or FALSE if initialization fails.
// When the system calls the DllMain function with any value other than DLL_PROCESS_ATTACH, the return value is ignored.
BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH: // initialize once for each new process
		// return TRUE to successful DLL_PROCESS_ATTACH
		// return FALSE to fail DLL load
		if (reserved) // static loads
		{
			core_dll.initialize(module);
			core_dll.disable_thread_notifications();
			return TRUE;
		}
		else // dynamic loads
		{
			return FALSE;
		}
	case DLL_THREAD_ATTACH: // do thread-specific initialization
		return TRUE;
	case DLL_THREAD_DETACH: // do thread-specific cleanup
		return TRUE;
	case DLL_PROCESS_DETACH: // perform any necessary cleanup
		if (reserved) // the process is terminating
		{
			return TRUE;
		}
		else // ::FreeLibrary has been called or the DLL load failed
		{
			return FALSE;
		}
	default:
		return FALSE;
	}
}
