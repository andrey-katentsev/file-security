#include "KAA/include/format_string.h"
#include "KAA/include/load_string.h"
#include "KAA/include/registry.h"
#include "KAA/include/registry_key.h"
#include "KAA/include/windows_registry.h"
#include "KAA/include/exception/windows_api_failure.h"
#include "KAA/include/UI/controls.h"
#include "KAA/include/filesystem/crt_directory_control.h"
#undef EncryptFile
#undef DecryptFile

#include "ClientCommunicator.h"
#include "OperationContext.h"
#include "../Common/UserReport.h"

#include <windows.h>
#undef EncryptFile
#undef DecryptFile

#include "resource.h"

using KAA::FileSecurity::GetCommunicator;
using KAA::user_interface::get_control_text;
using KAA::user_interface::set_control_text;

// FUTURE: KAA: separate code to modules.

INT_PTR CALLBACK ProgressDialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SettingsDialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDialog(HWND, UINT, WPARAM, LPARAM);

namespace
{
	const std::wstring registry_software_sub_key(L"Software\\Hyperlink Software\\File Security");
	const std::wstring registry_last_used_path_value_name(L"LastUsedPath");
	const std::wstring registry_window_horizontal_position_value_name(L"HPosition");
	const std::wstring registry_window_vertical_position_value_name(L"VPosition");

	std::auto_ptr<KAA::system::registry_key> GetRegistrySoftwareRootRead(KAA::system::registry* registry)
	{
		const KAA::system::registry::key_access query_only = { false, false, false, false, true, false };
		return registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, query_only);
	}

	std::auto_ptr<KAA::system::registry_key> GetRegistrySoftwareRootWrite(KAA::system::registry* registry)
	{
		const KAA::system::registry::key_access set_only = { false, false, false, false, false, true };
		return registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, set_only);
	}

	/*class WaitCursor
	{
	public:
		WaitCursor()
		{
			m_handle = ::LoadCursorW(::GetModuleHandleW(nullptr), IDC_WAIT);
			m_previous = ::SetCursor(m_handle);
		}

		~WaitCursor()
		{
			::SetCursor(m_previous);
		}

	private:
		HCURSOR m_handle;
		HCURSOR m_previous;
	};*/

	std::wstring QueryLastUsedPath(const KAA::system::registry_key* const key)
	{
		return key->query_string_value(registry_last_used_path_value_name);
	}

	void SaveLastUsedPath(KAA::system::registry_key* const key, const std::wstring& last_used_path)
	{
		return key->set_string_value(registry_last_used_path_value_name, last_used_path);
	}

	POINT QueryWindowPosition(const KAA::system::registry_key* const key)
	{
		const POINT coordinates =
		{
			key->query_dword_value(registry_window_horizontal_position_value_name),
			key->query_dword_value(registry_window_vertical_position_value_name)
		};
		return coordinates;
	}

	void SetRegWindowPosition(KAA::system::registry_key* const key, const POINT& coordinates)
	{
		key->set_dword_value(registry_window_horizontal_position_value_name, coordinates.x);
		key->set_dword_value(registry_window_vertical_position_value_name, coordinates.y);
	}

	// FUTURE: KAA: move to separate file.
	class CWDRestorer
	{
	public:
		CWDRestorer() : path(m_filesystem.get_current_working_directory())
		{}

		~CWDRestorer()
		{
			// FIX: KAA: exception unsafe!
			m_filesystem.set_current_working_directory(path);
		}

	private:
		KAA::filesystem::crt_directory_control m_filesystem; // FUTURE: KAA: replace with interface KAA::filesystem::directory_control.
		const std::wstring path;
	};

	void UpdateButtonsState(const HWND dialog)
	{
		const HWND encrypt_button = ::GetDlgItem(dialog, IDC_MAIN_ENCRYPT_SPECIFIED_FILE_BUTTON);
		const HWND decrypt_button = ::GetDlgItem(dialog, IDC_MAIN_DECRYPT_SPECIFIED_FILE_BUTTON);
		const bool file_encrypted = GetCommunicator().IsFileEncrypted(get_control_text(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT));
		::EnableWindow(encrypt_button, !file_encrypted);
		::EnableWindow(decrypt_button, file_encrypted);
	}

	void EncryptFileTask(void* context)
	{
		const std::wstring* path = reinterpret_cast<const std::wstring*>(context);
		GetCommunicator().EncryptFile(*path);
	}

	void DecryptFileTask(void* context)
	{
		const std::wstring* path = reinterpret_cast<const std::wstring*>(context);
		GetCommunicator().DecryptFile(*path);
	}

	BOOL ProcessControlMessage(const HWND dialog, const HWND control, const int control_identifier, const int notification_code)
	{
		switch(control_identifier)
		{
		case IDC_MAIN_BROWSE_FILE_TO_ENCRYPT_BUTTON:
			{
				switch(notification_code)
				{
				case BN_CLICKED:
					{
						std::vector<wchar_t> buffer(MAX_PATH, L'\0');
						OPENFILENAMEW setup = { 0 };
						setup.lStructSize = sizeof(OPENFILENAMEW);
						setup.hwndOwner = dialog;
						setup.lpstrFilter = L"All Files\0*.*\0\0";
						setup.lpstrFile = &buffer[0]; // may have initialization path
						setup.nMaxFile = buffer.size();
						//setup.lpstrTitle;
						setup.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
						setup.FlagsEx = OFN_EX_NOPLACESBAR;

						{
							const CWDRestorer cwd_for_this_session; // GetOpenFileNameW changes current working directory
							if(0 != ::GetOpenFileNameW(&setup))
							{
								const std::wstring selected_file_path(&buffer[0]);
								set_control_text(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT, selected_file_path);
							}
							else
							{
								const DWORD code = ::CommDlgExtendedError();
								if(0 != code) // 0 == user closed or canceled the dialog box
								{
									const std::wstring message_format(KAA::resources::load_string(IDS_GET_OPEN_FILE_NAME_DIALOG_FAILED_FORMAT));
									const std::wstring message_box_title(KAA::resources::load_string(IDS_GET_OPEN_FILE_NAME_DIALOG_FAILED_TITLE));
									const std::wstring message(KAA::format_string(message_format, code));
									::MessageBoxW(dialog, message.c_str(), message_box_title.c_str(), MB_OK | MB_ICONEXCLAMATION);
								}
							}
						}
					} break;
				default:
					return FALSE;
				}
			} break;
		case IDC_MAIN_ENCRYPT_SPECIFIED_FILE_BUTTON:
			{
				switch(notification_code)
				{
				case BN_CLICKED:
					{
						const std::wstring file_to_encrypt_path(get_control_text(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT));
						try
						{
							// FUTURE: KAA: DRY violation.
							const KAA::FileSecurity::OperationContext context = { EncryptFileTask, const_cast<std::wstring*>(&file_to_encrypt_path), IDS_FILE_ENCRYPTING, IDS_FILE_ENCRYPTED_SUCCESSFULLY, IDS_UNABLE_TO_COMPLETE_ENCRYPT_FILE_OPERATION };
							const INT_PTR code = ::DialogBoxParamW(::GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDD_PROGRESS), dialog, ProgressDialog, reinterpret_cast<LPARAM>(&context));
							if(IDOK == code)
							{
								UpdateButtonsState(dialog);
								{
									KAA::system::windows_registry registry;
									SaveLastUsedPath(GetRegistrySoftwareRootWrite(&registry).get(), file_to_encrypt_path);
								}
							}
						}
						catch(const KAA::FileSecurity::UserReport& report)
						{
							const std::wstring message_box_title(KAA::resources::load_string(IDS_FILE_ENCRYPTING));
							::MessageBoxW(dialog, report.format_message().c_str(), message_box_title.c_str(), MB_OK | ToIconID(report.severity()));
						}
						catch(const KAA::windows_api_failure& error)
						{
							const std::wstring message_box_title(KAA::resources::load_string(IDS_FILE_ENCRYPTING));
							const std::wstring message(KAA::resources::load_string(IDS_UNABLE_TO_ACCESS_SYSTEM_REGISTRY));
							// FUTURE: KAA: combine message.
							::MessageBoxW(dialog, (message + L'\n' + error.get_system_message()).c_str(), message_box_title.c_str(), MB_OK | MB_ICONEXCLAMATION);
						}
					} break;
				default:
					return FALSE;
				}
			} break;
		case IDC_MAIN_DECRYPT_SPECIFIED_FILE_BUTTON:
			{
				switch(notification_code)
				{
				case BN_CLICKED:
					{
						const std::wstring file_to_decrypt_path(get_control_text(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT));
						try
						{
							const KAA::FileSecurity::OperationContext context = { DecryptFileTask, const_cast<std::wstring*>(&file_to_decrypt_path), IDS_FILE_DECRYPTING, IDS_FILE_DECRYPTED_SUCCESSFULLY, IDS_UNABLE_TO_COMPLETE_DECRYPT_FILE_OPERATION };
							const INT_PTR code = ::DialogBoxParamW(::GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDD_PROGRESS), dialog, ProgressDialog, reinterpret_cast<LPARAM>(&context));
							if(IDOK == code)
							{
								UpdateButtonsState(dialog);
							}
						}
						catch(const KAA::FileSecurity::UserReport& report)
						{
							const std::wstring message_box_title(KAA::resources::load_string(IDS_FILE_DECRYPTING));
							::MessageBoxW(dialog, report.format_message().c_str(), message_box_title.c_str(), MB_OK | ToIconID(report.severity()));
						}
					} break;
				default:
					return FALSE;
				}
			} break;
		case IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT:
			{
				switch(notification_code)
				{
				case EN_CHANGE:
					{
						try
						{
							UpdateButtonsState(dialog);
						}
						catch(const KAA::FileSecurity::UserReport& report)
						{
							const std::wstring message_box_title(KAA::resources::load_string(IDS_FILE_STATE_REQUEST));
							::MessageBoxW(dialog, report.format_message().c_str(), message_box_title.c_str(), MB_OK | ToIconID(report.severity()));
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

	BOOL ProcessMenuMessage(const HWND dialog, const int menu_identifier)
	{
		switch(menu_identifier)
		{
		case IDC_ENCRYPT_FILE_EXIT_MENU:
			{
				::SendMessageW(dialog, WM_CLOSE, 0, 0);
			} break;
		case IDC_ENCRYPT_FILE_SETTINGS_MENU:
			{
				const INT_PTR code = ::DialogBoxW(::GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDD_SETTINGS), dialog, SettingsDialog);
				if(IDOK == code)
				{
					// FUTURE: KAA: should settings be applied here?
				}
			} break;
		case IDC_ENCRYPT_FILE_ABOUT_MENU:
			{
				::DialogBoxW(::GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDD_ABOUT), dialog, AboutDialog);
			} break;
		default:
			return FALSE;
		}
		return TRUE;
	}

	BOOL ProcessAcceleratorMessage(const HWND dialog, const int accelerator_identifier)
	{ return FALSE; }

	// If an application processes this message, it should return zero.
	INT_PTR OnClose(const HWND dialog)
	{
		{
			WINDOWINFO window_attributes = { 0 };
			window_attributes.cbSize = sizeof(WINDOWINFO);

			if(0 != ::GetWindowInfo(dialog, &window_attributes))
			{
				const POINT coordinates = { window_attributes.rcWindow.left, window_attributes.rcWindow.top };
				{
					KAA::system::windows_registry registry;
					SetRegWindowPosition(GetRegistrySoftwareRootWrite(&registry).get(), coordinates);
				}
			}
		}

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

	// The dialog box procedure should return TRUE to direct the system to set the keyboard focus to the control specified by wParam.
 // Otherwise, it should return FALSE to prevent the system from setting the default keyboard focus.
	// The dialog box procedure should return the value directly. The DWL_MSGRESULT value set by the SetWindowLong function is ignored.
	INT_PTR OnInitDialog(const HWND dialog, const WPARAM focus_default, const LPARAM initialization_data)
	{
		try
		{
			// FUTURE: KAA: relative coordinates (offset, percent) to screen resolution.
			KAA::system::windows_registry registry;
			const POINT coordinates(QueryWindowPosition(GetRegistrySoftwareRootRead(&registry).get()));
			::SetWindowPos(dialog, HWND_TOP, coordinates.x, coordinates.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		catch(const KAA::windows_api_failure& error)
		{
			if(ERROR_FILE_NOT_FOUND == error)
			{
				// FUTURE: KAA: center window.
				::SetWindowPos(dialog, HWND_TOP, 320, 240, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
		}

		{
			const HINSTANCE current_module = ::GetModuleHandleW(nullptr);
			const HANDLE lock_icon = ::LoadImageW(current_module, MAKEINTRESOURCEW(IDI_LOCK_XP), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
			::SendMessageW(dialog, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(lock_icon));
			::SendMessageW(dialog, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(lock_icon));
		}
		{
			const HINSTANCE current_module = ::GetModuleHandleW(nullptr);
			const SIZE icon_size = { 16, 16 };
			const HANDLE browse_icon = ::LoadImageW(current_module, MAKEINTRESOURCEW(IDI_EXPLORER_XP), IMAGE_ICON, icon_size.cx, icon_size.cy, LR_DEFAULTCOLOR);
			const HWND browse_button = ::GetDlgItem(dialog, IDC_MAIN_BROWSE_FILE_TO_ENCRYPT_BUTTON);
			::SendMessageW(browse_button, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(browse_icon));
		}
		{
			const HWND file_to_encrypt_edit = ::GetDlgItem(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT);
			::SendMessageW(file_to_encrypt_edit, EM_LIMITTEXT, MAX_PATH, 0);
			try
			{
				KAA::system::windows_registry registry;
				set_control_text(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT, QueryLastUsedPath(GetRegistrySoftwareRootRead(&registry).get()));
				::SendMessageW(::GetDlgItem(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT), WM_KEYDOWN, VK_END, 0);
			}
			catch(const KAA::windows_api_failure& error)
			{
				if(ERROR_FILE_NOT_FOUND != error)
					throw;
			}
		}
		{
			//if(IDC_FILE_TO_ENCRYPT_PATH_EDIT != GetDlgCtrlID(reinterpret_cast<HWND>(focus_default)))
			{
				::SetFocus(::GetDlgItem(dialog, IDC_MAIN_FILE_TO_ENCRYPT_PATH_EDIT));
				return FALSE;
			}
		}
		//return TRUE;
	}
}

// Typically, the dialog box procedure should return TRUE if it processed the message, and FALSE if it did not.
// If the dialog box procedure returns FALSE, the dialog manager performs the default dialog operation in response to the message.
// If the dialog box procedure processes a message that requires a specific return value,
// the dialog box procedure should set the icon_size return value by calling SetWindowLong(hwndDlg, DWL_MSGRESULT, lResult) immediately before returning TRUE.
// Note that you must call SetWindowLong immediately before returning TRUE; doing so earlier may result in the DWL_MSGRESULT value being overwritten by a nested dialog box message.
INT_PTR CALLBACK EncryptDialog(HWND dialog, UINT message, WPARAM message_context, LPARAM extended_message_context)
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
