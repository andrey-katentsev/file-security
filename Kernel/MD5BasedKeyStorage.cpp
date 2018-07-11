#include "MD5BasedKeyStorage.h"

#include "KAA/include/md5.h"

namespace
{
	const std::wstring key_file_extension(L".bin");
}

namespace KAA
{
	namespace FileSecurity
	{
		MD5BasedKeyStorage::MD5BasedKeyStorage(const filesystem::path::directory& key_storage_path) :
		key_storage_path(key_storage_path)
		{}

		void MD5BasedKeyStorage::ISetPath(const filesystem::path::directory& path)
		{
			key_storage_path = path;
		}

		filesystem::path::directory MD5BasedKeyStorage::IGetPath(void) const
		{
			return key_storage_path;
		}

		filesystem::path::file MD5BasedKeyStorage::IGetKeyPathForSpecifiedPath(const filesystem::path::file& path) const
		{
			const auto data = path.to_wstring();
			const auto data_size = sizeof ( decltype(data)::value_type ) * data.length();
			const auto hash = cryptography::calculate_md5(data.c_str(), data_size);
			const auto filename = cryptography::to_wstring(hash) + key_file_extension;
			return key_storage_path + filename;
		}
	}
}
