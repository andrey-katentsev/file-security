#pragma once

#include <string>

#include "KAA/include/filesystem/path.h"

namespace KAA
{
	namespace FileSecurity
	{
		class KeyStorage
		{
		public:
			virtual ~KeyStorage();

			void SetPath(const filesystem::path::directory&);
			filesystem::path::directory GetPath(void) const;

			std::wstring GetKeyPathForSpecifiedPath(const std::wstring&) const;

		private:
			virtual void ISetPath(const filesystem::path::directory&) = 0;
			virtual filesystem::path::directory IGetPath(void) const = 0;

			virtual std::wstring IGetKeyPathForSpecifiedPath(const std::wstring&) const = 0;
		};
	}
}
