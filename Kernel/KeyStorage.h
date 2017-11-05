#pragma once

#include <string>

namespace KAA
{
	namespace FileSecurity
	{
		class KeyStorage
		{
		public:
			virtual ~KeyStorage();

			void SetPath(const std::wstring&);
			std::wstring GetPath(void) const;

			std::wstring GetKeyPathForSpecifiedPath(const std::wstring&) const;

		private:
			virtual void ISetPath(const std::wstring&) = 0;
			virtual std::wstring IGetPath(void) const = 0;

			virtual std::wstring IGetKeyPathForSpecifiedPath(const std::wstring&) const = 0;
		};
	}
}
