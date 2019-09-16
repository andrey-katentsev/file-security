//
// File: kernel.h
// Description: processing unit API
// Created: January 27, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Hyperlink Software, 2014
//

#pragma once

#include <memory>

#include "../Common/Communicator.h"

#ifdef KERNEL_EXPORTS
#define KERNEL_API __declspec(dllexport)
#else
#define KERNEL_API __declspec(dllimport)
#endif

// FUTURE: KAA: consider use COM.
//#include <objbase.h>
//#include <olectl.h>

namespace KAA
{
	namespace FileSecurity
	{
		KERNEL_API std::unique_ptr<Communicator> GetClassObject(void);

		//STDAPI DllCanUnloadNow(void);
		//STDAPI DllGetClassObject(REFCLSID, REFIID, LPVOID*);
		//HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO*);
		//HRESULT DllInstall(BOOL, LPCWSTR);
		//STDAPI DllRegisterServer(void);
		//STDAPI DllUnregisterServer(void);
	}
}
