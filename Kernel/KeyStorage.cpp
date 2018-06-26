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

		std::wstring KeyStorage::GetKeyPathForSpecifiedPath(const std::wstring& path) const
		{
			return IGetKeyPathForSpecifiedPath(path);
		};
	}
}
