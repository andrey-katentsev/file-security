#include <stdexcept>
#include <vector>

#include "KAA/include/load_string.h"
#include "KAA/include/dll/get_module_handle.h"
#include "KAA/include/exception/com_failure.h"
#include "KAA/include/exception/operation_failure.h"
#include "KAA/include/exception/windows_api_failure.h"
#include "KAA/include/filesystem/path.h"
#include "KAA/include/RAII/com_initializer.h"
#include "KAA/include/RAII/com_task_memory.h"
#include "KAA/include/UI/controls.h"

#include "ClientCommunicator.h"
#include "../Common/UserReport.h"

#include <windows.h>
#include <objbase.h>
#include <shlobj.h>

#include "resource.h"

using KAA::FileSecurity::GetCommunicator;
using KAA::user_interface::get_control_text;
using KAA::user_interface::set_control_text;

namespace
{
	unsigned ToIndex(HWND combobox, const KAA::FileSecurity::wipe_method_id filter)
	{
		const auto number_of_items = KAA::user_interface::get_items_count(combobox);
		for(unsigned item_index = 0; item_index < number_of_items; ++item_index)
		{
			const auto assotiated_value = KAA::user_interface::get_item_data(combobox, item_index);
			if(filter == assotiated_value)
				return item_index;
		}

		throw KAA::operation_failure(__FUNCTIONW__, L"Unable to convert a wipe method id to a combobox item index. There is no combobox item associated with specified value.", KAA::operation_failure::R_NOT_FOUND, KAA::operation_failure::S_ERROR);
	}

	KAA::FileSecurity::wipe_method_id GetWipeMethod(const HWND combobox)
	{
		const auto item_index = KAA::user_interface::get_selected_item(combobox);
		return KAA::user_interface::get_item_data(combobox, item_index);
	}

	int CALLBACK BrowseForFolderCallback(HWND dialog, UINT message, LPARAM context, LPARAM user_data)
	{
		switch(message)
		{
		case BFFM_INITIALIZED:
			{
				const auto initial_directory = reinterpret_cast<const std::wstring*>(user_data);
				enum { use_a_string = TRUE, use_a_PIDL = FALSE };
				::SendMessageW(dialog, BFFM_SETEXPANDED, use_a_string, reinterpret_cast<LPARAM>(initial_directory->c_str()));
				::SendMessageW(dialog, BFFM_SETSELECTION, use_a_string, reinterpret_cast<LPARAM>(initial_directory->c_str()));
			} break;
		case BFFM_IUNKNOWN:
		case BFFM_SELCHANGED:
			break;
		case BFFM_VALIDATEFAILED:
			{
				enum { dismiss = 0, keep_displayed = 1 };
				return keep_displayed;
			} break;
		}
		return 0;
	}

	BOOL ProcessControlMessage(const HWND dialog, const HWND control, const int control_identifier, const int notification_code)
	{
		switch(control_identifier)
		{
		case IDC_SETTINGS_BROWSE_KEY_STORAGE_PATH_BUTTON:
			{
				switch(notification_code)
				{
				case BN_CLICKED:
					{
						const auto browse_for_key_storage_path_dialog_title = KAA::resources::load_string(IDS_BROWSE_FOR_KEY_STORAGE_PATH_DIALOG_TITLE);

						BROWSEINFOW setup = { 0 };
						setup.hwndOwner = dialog;
						setup.lpszTitle = browse_for_key_storage_path_dialog_title.c_str();
						setup.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | /*BIF_EDITBOX | */BIF_RETURNONLYFSDIRS;
						setup.lpfn = BrowseForFolderCallback;

						// DEFECT: KAA: may be empty control.
						auto directory = KAA::filesystem::path::directory { get_control_text(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT) };
						const auto initial_directory = KAA::filesystem::path::make_WinAPI_directory_path(directory.to_wstring());
						setup.lParam = reinterpret_cast<LPARAM>(&initial_directory);

						std::vector<wchar_t> buffer(MAX_PATH, L'\0');

						const KAA::RAII::com_initializer com(COINIT_APARTMENTTHREADED);
						PIDLIST_ABSOLUTE selected_directory = ::SHBrowseForFolderW(&setup);
						if(nullptr != selected_directory)
						{
							const KAA::RAII::com_task_memory acquired_memory(selected_directory);
							if(TRUE != ::SHGetPathFromIDList(selected_directory, &buffer[0]))
							{
								const DWORD error = ::GetLastError();
								throw KAA::windows_api_failure(__FUNCTIONW__, L"Unable to convert an item identifier list to a file system path.", error);
							}

							directory = KAA::filesystem::path::directory { &buffer[0] };
							const auto new_key_storage_path = KAA::filesystem::path::make_CRT_directory_path(directory.to_wstring());
							set_control_text(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT, new_key_storage_path);
						}
					} break;
				default:
					return FALSE;
				}
			} break;
		case IDC_SETTINGS_OK_BUTTON:
			{
				switch(notification_code)
				{
				case BN_CLICKED:
					{
						// DEFECT: KAA: user-provided data must be validated.
						// KAA: check that path is valid.
						auto key_storage_path = get_control_text(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT);
						if(!key_storage_path.empty())
						{
							try
							{
								const HWND security_checkbox = ::GetDlgItem(dialog, IDC_SETTINGS_ABSOLUTE_STRONG_SECURITY_CHECK); // FUTURE: KAA: implement feature.
								const HWND wipe_method_combobox = ::GetDlgItem(dialog, IDC_SETTINGS_WIPE_METHOD_COMBO);
								GetCommunicator().SetWipeMethod(GetWipeMethod(wipe_method_combobox)); // DEFECT: KAA: implement transaction model.
								GetCommunicator().SetKeyStoragePath(std::move(key_storage_path));
								::EndDialog(dialog, IDOK);
							}
							catch(const KAA::FileSecurity::UserReport& report)
							{
								const auto message_box_title = KAA::resources::load_string(IDS_SETTINGS);
								::MessageBoxW(dialog, report.format_message().c_str(), message_box_title.c_str(), MB_OK | ToIconID(report.severity()));
							}
						}
						else
						{
							auto control = ::GetDlgItem(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT);
							KAA::user_interface::show_ballon_tip(control, IDS_KEY_STORAGE, IDS_EMPTY_KEY_STORAGE_PATH_PROMPT, TTI_NONE);
						}
					} break;
				default:
					return FALSE;
				}
			} break;
		default:
			return FALSE;
		}
		return TRUE;
	}

	BOOL ProcessMenuMessage(const HWND, const int)
	{ return FALSE; }

	BOOL ProcessAcceleratorMessage(const HWND, const int)
	{ return FALSE; }

	// If an application processes this message, it should return zero.
	INT_PTR OnClose(const HWND dialog)
	{
		::EndDialog(dialog, IDCLOSE);
		::SetWindowLongPtrW(dialog, DWL_MSGRESULT, 0L);
		return TRUE;
	}

	// If an application processes this message, it should return zero.
	INT_PTR OnCommand(const HWND dialog, const WPARAM message_context, const LPARAM extended_message_context)
	{
		const HWND control = reinterpret_cast<HWND>(extended_message_context);
		const int notification_code = HIWORD(message_context);
		const int id = LOWORD(message_context);

		BOOL message_processed = FALSE;

		if(nullptr != control)
		{
			message_processed = ProcessControlMessage(dialog, control, id, notification_code);
		}
		else
		{
			if(0 == notification_code)
				message_processed = ProcessMenuMessage(dialog, id);
			else
				message_processed = ProcessAcceleratorMessage(dialog, id);
		}

		if(TRUE == message_processed)
			::SetWindowLongPtrW(dialog, DWL_MSGRESULT, 0L);

		return message_processed;
	}

	// The dialog box procedure should return TRUE to direct the system to set the keyboard focus to the control specified by wParam. Otherwise, it should return FALSE to prevent the system from setting the default keyboard focus.
	// The dialog box procedure should return the value directly. The DWL_MSGRESULT value set by the SetWindowLong function is ignored.
	INT_PTR OnInitDialog(const HWND dialog, const WPARAM focus_default, const LPARAM initialization_data)
	{
		// DEFECT: KAA: DRY violation.
		{
			const auto current_module = KAA::dll::get_calling_process_module_handle();
			const SIZE icon_size = { 16, 16 };
			// DEFECT: KAA: handle leak.
			const HANDLE browse_icon = ::LoadImageW(current_module, MAKEINTRESOURCEW(IDI_EXPLORER_XP), IMAGE_ICON, icon_size.cx, icon_size.cy, LR_DEFAULTCOLOR);
			const HWND browse_button = ::GetDlgItem(dialog, IDC_SETTINGS_BROWSE_KEY_STORAGE_PATH_BUTTON);
			::SendMessageW(browse_button, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(browse_icon));
		}
		{
			const HWND key_storage_path = ::GetDlgItem(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT);
			::SendMessageW(key_storage_path, EM_LIMITTEXT, MAX_PATH, 0);
		}
		{
			const HWND absolute_security_check = ::GetDlgItem(dialog, IDC_SETTINGS_ABSOLUTE_STRONG_SECURITY_CHECK);
			// TODO: KAA: move to the SDK.
			::SendMessageW(absolute_security_check, BM_SETCHECK, BST_CHECKED, 0);
			::EnableWindow(absolute_security_check, FALSE);
		}
		{
			const HWND wipe_method_combo = ::GetDlgItem(dialog, IDC_SETTINGS_WIPE_METHOD_COMBO);
			try
			{
				const auto available_wipe_methods = GetCommunicator().GetAvailableWipeMethods();
				for (const auto& wipe_method : available_wipe_methods)
				{
					const auto index = KAA::user_interface::add_string(wipe_method_combo, wipe_method.first);
					KAA::user_interface::set_item_data(wipe_method_combo, index, wipe_method.second);
				}
				unsigned current_method = ToIndex(wipe_method_combo, GetCommunicator().GetWipeMethod());
				KAA::user_interface::set_selected_item(wipe_method_combo, current_method);
			}
			catch(const KAA::FileSecurity::UserReport& report)
			{
				// DEFECT: KAA: DRY violation : handle user report.
				const auto message_box_title = KAA::resources::load_string(IDS_WIPE_METHODS_COMBO_BOX_INITIALIZING);
				::MessageBoxW(dialog, report.format_message().c_str(), message_box_title.c_str(), MB_OK | ToIconID(report.severity()));
			}
		}
		try
		{
			set_control_text(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT, GetCommunicator().GetKeyStoragePath().to_wstring());
			::SendMessageW(::GetDlgItem(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT), WM_KEYDOWN, VK_END, 0);
		}
		catch(const KAA::FileSecurity::UserReport& report)
		{
			const auto message_box_title = KAA::resources::load_string(IDS_KEY_STORAGE_PATH_EDIT_INITIALIZING);
			::MessageBoxW(dialog, report.format_message().c_str(), message_box_title.c_str(), MB_OK | ToIconID(report.severity()));
		}
		{
			//if(IDC_SETTINGS_ABSOLUTE_STRONG_SECURITY_CHECK != GetDlgCtrlID(reinterpret_cast<HWND>(focus_default)))
			{
				//SetFocus(GetDlgItem(dialog, IDC_SETTINGS_ABSOLUTE_STRONG_SECURITY_CHECK));
				::SetFocus(::GetDlgItem(dialog, IDC_SETTINGS_KEY_STORAGE_PATH_EDIT));
				return FALSE;
			}
		}
		//return TRUE;
	}
}

// Typically, the dialog box procedure should return TRUE if it processed the message, and FALSE if it did not. If the dialog box procedure returns FALSE, the dialog manager performs the default dialog operation in response to the message.
// If the dialog box procedure processes a message that requires a specific return value, the dialog box procedure should set the icon_size return value by calling SetWindowLong(hwndDlg, DWL_MSGRESULT, lResult) immediately before returning TRUE. Note that you must call SetWindowLong immediately before returning TRUE; doing so earlier may result in the DWL_MSGRESULT value being overwritten by a nested dialog box message.
INT_PTR CALLBACK SettingsDialog(HWND dialog, UINT message, WPARAM message_context, LPARAM extended_message_context)
{
	switch(message)
	{
	case WM_CLOSE:
		return OnClose(dialog);
	case WM_COMMAND:
		return OnCommand(dialog, message_context, extended_message_context);
	case WM_INITDIALOG:
		return OnInitDialog(dialog, message_context, extended_message_context);
	default:
		return FALSE;
	}
}
