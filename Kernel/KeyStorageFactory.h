#pragma once

#include <memory>
#include <string>

namespace KAA
{
	namespace filesystem
	{
		class driver;
		namespace path
		{
			class directory;
		}
	}

	namespace FileSecurity
	{
		class KeyStorage;
		enum class key_storage_t
		{
			md5_based,
			crc32_based
		};

		std::unique_ptr<KeyStorage> CreateKeyStorage(key_storage_t, std::shared_ptr<filesystem::driver>, filesystem::path::directory);
	}
}
