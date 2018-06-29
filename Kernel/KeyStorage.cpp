#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		KeyStorage::~KeyStorage()
		{}

		void KeyStorage::SetPath(const filesystem::path::directory& path)
		{
			return ISetPath(path);
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
