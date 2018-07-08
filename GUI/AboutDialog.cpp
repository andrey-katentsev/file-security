#include <windows.h>
#include "resource.h"

namespace
{
	BOOL ProcessControlMessage(const HWND dialog, const HWND control, const int control_identifier, const int notification_code)
	{
		switch(control_identifier)
		{
		case IDC_ABOUT_OK:
			{
				switch(notification_code)
				{
				case BN_CLICKED:
					{
						::EndDialog(dialog, IDOK);
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

	// No return value.
	INT_PTR OnHotKey(const HWND dialog, const WPARAM message_context, const LPARAM extended_message_context)
	{
		const int hot_key_identifier = message_context;
		const WORD virtual_key_code = HIWORD(extended_message_context);
		const WORD modifiers = LOWORD(extended_message_context); // any combination of MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN

		switch(hot_key_identifier)
		{
		case IDHOT_SNAPDESKTOP: // system-defined hot key
		case IDHOT_SNAPWINDOW: // system-defined hot key
		default: break;
		}

		return FALSE;
	}
}

// Typically, the dialog box procedure should return TRUE if it processed the message, and FALSE if it did not. If the dialog box procedure returns FALSE, the dialog manager performs the default dialog operation in response to the message.
// If the dialog box procedure processes a message that requires a specific return value, the dialog box procedure should set the icon_size return value by calling SetWindowLong(hwndDlg, DWL_MSGRESULT, lResult) immediately before returning TRUE. Note that you must call SetWindowLong immediately before returning TRUE; doing so earlier may result in the DWL_MSGRESULT value being overwritten by a nested dialog box message.
INT_PTR CALLBACK AboutDialog(HWND dialog, UINT message, WPARAM message_context, LPARAM extended_message_context)
{
	switch(message)
	{
	case WM_CLOSE:
		return OnClose(dialog);
	case WM_COMMAND:
		return OnCommand(dialog, message_context, extended_message_context);
	case WM_HOTKEY:
		return OnHotKey(dialog, message_context, extended_message_context);
	default:
		return FALSE;
	}
}
