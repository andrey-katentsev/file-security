#include "KeyStorageFactory.h"

#include "KAA/include/exception/operation_failure.h"

#include "CRC32BasedKeyStorage.h"
#include "MD5BasedKeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		// TODO: KAA: replace auto_ptr.
		std::unique_ptr<KeyStorage> CreateKeyStorage(const key_storage_t type, std::shared_ptr<filesystem::driver> filesystem, filesystem::path::directory path)
		{
			switch(type)
			{
			case md5_based:
				return std::make_unique<MD5BasedKeyStorage>(std::move(filesystem), std::move(path));
			case crc32_based:
				return std::make_unique<CRC32BasedKeyStorage>(std::move(path));
			default:
				{
					constexpr auto source { __FUNCTIONW__ };
					constexpr auto description { L"cannot create key storage class instance: specified type is not supported" };
					constexpr auto reason = operation_failure::R_INVALID_ARGUMENT;
					constexpr auto severity = operation_failure::S_ERROR;
					throw operation_failure(source, description, reason, severity);
				}
			}
		}
	}
}
