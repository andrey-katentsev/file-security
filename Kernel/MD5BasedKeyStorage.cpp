#include "MD5BasedKeyStorage.h"

#include <vector>

#include "KAA/include/cryptography/md5.h"
#include "KAA/include/exception/operation_failure.h"
#include "KAA/include/filesystem/driver.h"

namespace KAA
{
	namespace FileSecurity
	{
		MD5BasedKeyStorage::MD5BasedKeyStorage(std::shared_ptr<filesystem::driver> driver, filesystem::path::directory path) :
		filesystem(std::move(driver)),
		storage_path(std::move(path))
		{
			if (!filesystem)
			{
				constexpr auto source = __FUNCTION__;
				constexpr auto description = "unable to create MD5 based key storage class instance";
				constexpr auto reason = operation_failure::status_code_t::invalid_argument;
				constexpr auto severity = operation_failure::severity_t::error;
				throw operation_failure(source, description, reason, severity);
			}
		}

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
			const filesystem::driver::mode sequential_read_only { false };
			const filesystem::driver::share share_read { false };
			auto file = filesystem->open_file(path, sequential_read_only, share_read);
			cryptography::md5 hash;
			{
				auto last_chunk = false;
				constexpr auto chunk_size = 64U * 1024U; // 64 KiB
				std::vector<uint8_t> data(chunk_size);
				do
				{
					const auto bytes_read = file->read(chunk_size, data.data());
					last_chunk = bytes_read < chunk_size;
					if (last_chunk)
					{
						data.resize(bytes_read);
					}
					hash.add_data(data);
				} while (!last_chunk);
			}
			auto filename = convert::to_wstring(hash.complete()) + L".bin";
			return storage_path + std::move(filename);
		}
	}
}
