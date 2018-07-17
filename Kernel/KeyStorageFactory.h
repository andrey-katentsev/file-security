#pragma once

#include <memory>
#include <string>

namespace KAA
{
	namespace filesystem
	{
		namespace path
		{
			class directory;
		}
	}

	namespace FileSecurity
	{
		class KeyStorage;

		enum key_storage_t
		{
			md5_based,
			crc32_based
		};

		std::auto_ptr<KeyStorage> CreateKeyStorage(key_storage_t, const filesystem::path::directory&);
	}
}
