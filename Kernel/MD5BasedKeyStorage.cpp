#include "MD5BasedKeyStorage.h"

#include "SDK/KAA/include/cryptography.h"
#include "SDK/KAA/include/md5.h"
#include "SDK/KAA/include/keywords.h"

namespace
{
	const std::wstring key_file_extension(L".bin");

	std::wstring AppendBackslash(std::wstring path)
	{
		const auto slash = L'\\';
		const auto backslash = L'/';
		const auto last_symbol = path.back();
		if (slash == last_symbol || backslash == last_symbol)
		{
			return path;
		}
		else
		{
			path.push_back(slash);
			return path;
		}
	}
}

namespace KAA
{
	namespace FileSecurity
	{
		MD5BasedKeyStorage::MD5BasedKeyStorage(const std::wstring& key_storage_path) :
		key_storage_path(key_storage_path)
		{}

		MD5BasedKeyStorage::~MD5BasedKeyStorage()
		{}

		void MD5BasedKeyStorage::ISetPath(const std::wstring& path)
		{
			key_storage_path = path;
		}

		std::wstring MD5BasedKeyStorage::IGetPath(void) const
		{
			return key_storage_path;
		}

		std::wstring MD5BasedKeyStorage::IGetKeyPathForSpecifiedPath(const std::wstring& path) const
		{
			const cryptography::provider csp { nullptr, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT };
			const void* data = path.c_str();
			const size_t data_size = sizeof(std::wstring::value_type) * path.length();
			const cryptography::md5 hash = cryptography::calculate_md5(csp, data, data_size);
			return AppendBackslash(key_storage_path) + cryptography::to_wstring(hash) + key_file_extension;
		}
	}
}
