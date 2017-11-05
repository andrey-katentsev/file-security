#pragma once

#include <memory>
#include <string>

namespace KAA
{
	namespace FileSecurity
	{
		class KeyStorage;

		enum key_storage_t
		{
			md5_based
		};

		std::auto_ptr<KeyStorage> CreateKeyStorage(key_storage_t, const std::wstring& key_storage_path);
	}
}
