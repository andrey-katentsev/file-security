#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		void KeyStorage::SetPath(filesystem::path::directory path)
		{
			return ISetPath(std::move(path));
		}

		filesystem::path::directory KeyStorage::GetPath(void) const
		{
			return IGetPath();
		}

		filesystem::path::file KeyStorage::GetKeyPathForSpecifiedPath(const filesystem::path::file& path) const
		{
			return IGetKeyPathForSpecifiedPath(path);
		};
	}
}
