#include <exception>
#include <typeinfo>

#include "KAA/include/convert.h"
#include "KAA/include/format_string.h"
#include "KAA/include/load_string.h"
#include "KAA/include/exception/failure.h"

#include <windows.h> // FUTURE: KAA: research EncryptFile.
#include "resource.h"

#pragma comment(linker, "/manifestdependency:\"type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

INT_PTR CALLBACK EncryptDialog(HWND, UINT, WPARAM, LPARAM);

#pragma warning(push)
#pragma warning(disable: 4702)

// If the function succeeds, terminating when it receives a WM_QUIT message,
// it should return the exit value contained in that message's wParam parameter.
// If the function terminates before entering the message loop, it should return zero.
int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	{
		const WORD winver = WINVER;
		const WORD winnt = _WIN32_WINNT;
	}

	try
	{
		::DialogBoxW(instance, MAKEINTRESOURCEW(IDD_ENCRYPT_FILE), nullptr, EncryptDialog);
	}
	catch(const KAA::failure& error)
	{
		const auto message_format = KAA::resources::load_string(IDS_UNHANDLED_KAA_EXCEPTION_REPORT_REQUEST_FORMAT);
		const auto message_box_title = KAA::resources::load_string(IDS_FILE_SECURITY);
		const auto exception_class_type_name = KAA::convert::to_wstring(typeid(error).name());
		const auto source = error.get_source();
		const auto description = error.get_description();
		const auto system_message = error.get_system_message(); // FUTURE: KAA: consider rename to get_system_message / get_reason.
		const auto message = KAA::format_string(message_format, exception_class_type_name.c_str(), source.c_str(), description.c_str(), system_message.c_str());
		::MessageBoxW(nullptr, message.c_str(), message_box_title.c_str(), MB_OK | MB_ICONHAND);
	}
	catch(const std::exception& error)
	{
		const auto message_format = KAA::resources::load_string(IDS_UNHANDLED_CPP_EXCEPTION_REPORT_REQUEST_FORMAT);
		const auto message_box_title = KAA::resources::load_string(IDS_FILE_SECURITY);
		const auto exception_class_type_name = KAA::convert::to_wstring(typeid(error).name());
		const auto error_description = KAA::convert::to_wstring(error.what());
		const auto message = KAA::format_string(message_format, exception_class_type_name.c_str(), error_description.c_str());
		::MessageBoxW(nullptr, message.c_str(), message_box_title.c_str(), MB_OK | MB_ICONHAND);
	}
	return 0;
}

#pragma warning(pop)

// NOTE: exception list might be thrown.

// exception: system_failure
// source: C run-time API

// exception: win32_failure
// source: win32 API

// exception: com_failure
// source: win32 API

// exception: exception and derived
// source: C++ standard-library

// exception: operation_failure
// source: KAA SDK

// exception: UserReport
// source: File Security
