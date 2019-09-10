#include "KAA/include/dll/module_context.h"

#include <windows.h>

// DEFECT: KAA: deal with global value.
KAA::dll::module_context core_dll;

// When the system calls the DllMain function with the DLL_PROCESS_ATTACH value, the function returns TRUE if it succeeds or FALSE if initialization fails.
// When the system calls the DllMain function with any value other than DLL_PROCESS_ATTACH, the return value is ignored.
BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved)
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH: // initialize once for each new process
		{
			// return TRUE to successful DLL_PROCESS_ATTACH
			// return FALSE to fail DLL load
			if(nullptr == reserved) // dynamic loads
			{
				return FALSE;
			}
			else // static loads
			{
				::DisableThreadLibraryCalls(module);
				core_dll.m_module = module;
			}
		} break;
	case DLL_THREAD_ATTACH: // do thread-specific initialization
		{ } break;
	case DLL_THREAD_DETACH: // do thread-specific cleanup
		{ } break;
	case DLL_PROCESS_DETACH: // perform any necessary cleanup
		{
			if(nullptr == reserved) // FreeLibrary has been called or the DLL load failed
			{
				return FALSE;
			}
			else // the process is terminating
			{
			}
		} break;
	}
	return TRUE;
}
