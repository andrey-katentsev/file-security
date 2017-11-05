#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		KeyStorage::~KeyStorage()
		{}

		void KeyStorage::SetPath(const std::wstring& path)
		{
			return ISetPath(path);
		}

		std::wstring KeyStorage::GetPath(void) const
		{
			return IGetPath();
		}

		std::wstring KeyStorage::GetKeyPathForSpecifiedPath(const std::wstring& path) const
		{
			return IGetKeyPathForSpecifiedPath(path);
		};
	}
}
