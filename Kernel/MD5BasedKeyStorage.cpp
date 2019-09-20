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
				constexpr auto source { __FUNCTIONW__ };
				constexpr auto description { L"unable to create MD5 based key storage class instance" };
				constexpr auto reason = operation_failure::R_INVALID_ARGUMENT;
				constexpr auto severity = operation_failure::S_ERROR;
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
			// DEFECT: KAA: read and calculate chunks instead of whole file.
			const auto chunk_size = file->get_size();
			std::vector<uint8_t> data(chunk_size);
			const auto bytes_read = file->read(chunk_size, data.data());
			const auto hash = cryptography::calculate_md5(data.data(), bytes_read);
			auto filename = convert::to_wstring(hash) + std::wstring{ L".bin" };
			return storage_path + std::move(filename);
		}
	}
}
