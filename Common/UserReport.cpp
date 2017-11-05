// 30/01/2014

#include "UserReport.h"

#include <stdexcept>

#include "SDK/KAA/include/exception/operation_failure.h"

#include <windows.h>

namespace KAA
{
	namespace FileSecurity
	{
		UserReport::UserReport(const std::wstring& message, const severity_t importance) :
		m_message(message),
		m_importance(importance)
		{}

		std::wstring UserReport::get_title(void) const
		{
			return m_title;
		}

		std::wstring UserReport::format_message(void) const
		{
			return m_message;
		}

		UserReport::severity_t UserReport::severity(void) const
		{
			return m_importance;
		}

		unsigned ToIconID(const UserReport::severity_t importance)
		{
			switch(importance)
			{
			case UserReport::success:
				return 0; // MB_ICONQUESTION
			case UserReport::information:
				return MB_ICONASTERISK;
			case UserReport::warning:
				return MB_ICONEXCLAMATION;
			case UserReport::error:
				return MB_ICONHAND;
			default:
				{
					const std::wstring source { __FUNCTIONW__ };
					const std::wstring description { L"Unable to convert an user report severity to a message box icon index." };
					const KAA::operation_failure::status_code_t reason = KAA::operation_failure::R_INVALID_ARGUMENT;
					const KAA::operation_failure::severity_t severity = KAA::operation_failure::S_ERROR;
					throw operation_failure(source, description, reason, severity);
				}
			}
		}
	}
}
