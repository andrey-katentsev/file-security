#include "MD5BasedKeyStorage.h"

#include "KAA/include/cryptography.h"
#include "KAA/include/md5.h"
#include "KAA/include/keywords.h"

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

		MD5BasedKeyStorage::~MD5BasedKeyStorage()
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
			const cryptography::provider csp { nullptr, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT };
			const auto data = path.to_wstring();
			const auto data_size = sizeof ( decltype(data)::value_type ) * data.length();
			const auto hash = cryptography::calculate_md5(csp, data.c_str(), data_size);
			return key_storage_path + (cryptography::to_wstring(hash) + key_file_extension);
		}
	}
}
