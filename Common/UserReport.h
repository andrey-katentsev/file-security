// 30/01/2014

#pragma once

#include <string>

namespace KAA
{
	namespace FileSecurity
	{
		class UserReport
		{
		public:
			enum class severity_t
			{
				success,
				information,
				warning,
				error
			};

			UserReport(const std::wstring& message, severity_t); // FIX: KAA: provide with report title.

			std::wstring format_message(void) const;
			severity_t severity(void) const;

		private:
			std::wstring m_title;
			std::wstring m_message;
			severity_t m_importance;

			std::wstring get_title(void) const;
		};

		unsigned ToIconID(const UserReport::severity_t);
	}
}
