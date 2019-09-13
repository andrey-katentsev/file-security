#include "MD5BasedKeyStorage.h"

#include "KAA/include/md5.h"

namespace KAA
{
	namespace FileSecurity
	{
		MD5BasedKeyStorage::MD5BasedKeyStorage(filesystem::path::directory path) :
		storage_path(std::move(path))
		{}

		void MD5BasedKeyStorage::ISetPath(filesystem::path::directory path)
		{
			storage_path = std::move(path);
		}

		filesystem::path::directory MD5BasedKeyStorage::IGetPath(void) const
		{
			return storage_path;
		}

		filesystem::path::file MD5BasedKeyStorage::IGetKeyPathForSpecifiedPath(const filesystem::path::file& path) const
		{
			const auto data = path.to_wstring();
			const auto data_size = sizeof ( decltype(data)::value_type ) * data.length();
			const auto hash = cryptography::calculate_md5(data.c_str(), data_size);
			const auto filename = convert::to_wstring(hash) + std::wstring{ L".bin" };
			return storage_path + filename;
		}
	}
}
