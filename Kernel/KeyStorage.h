#pragma once

#include "KAA/include/filesystem/path.h"

namespace KAA
{
	namespace FileSecurity
	{
		class KeyStorage
		{
		public:
			virtual ~KeyStorage() = default;

			void SetPath(const filesystem::path::directory&);
			filesystem::path::directory GetPath(void) const;

			filesystem::path::file GetKeyPathForSpecifiedPath(const filesystem::path::file&) const;

		private:
			virtual void ISetPath(const filesystem::path::directory&) = 0;
			virtual filesystem::path::directory IGetPath(void) const = 0;

			virtual filesystem::path::file IGetKeyPathForSpecifiedPath(const filesystem::path::file&) const = 0;
		};
	}
}
