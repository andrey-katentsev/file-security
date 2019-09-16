// 30/01/2014

#include "UserReport.h"
#include <stdexcept>
#include "KAA/include/exception/operation_failure.h"
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
					constexpr auto source { __FUNCTIONW__ };
					constexpr auto description { L"cannot convert user report severity to message box icon index: specified severity is not supported" };
					constexpr auto reason = operation_failure::R_INVALID_ARGUMENT;
					constexpr auto severity = operation_failure::S_ERROR;
					throw operation_failure(source, description, reason, severity);
				}
			}
		}
	}
}
