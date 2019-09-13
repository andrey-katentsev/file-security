#include "CRC32BasedKeyStorage.h"

#include "KAA/include/convert.h"
#include "KAA/include/checksum.h"

namespace KAA
{
	namespace FileSecurity
	{
		CRC32BasedKeyStorage::CRC32BasedKeyStorage(filesystem::path::directory path) :
		storage_path(std::move(path))
		{}

		void CRC32BasedKeyStorage::ISetPath(filesystem::path::directory path)
		{
			storage_path = std::move(path);
		}

		filesystem::path::directory CRC32BasedKeyStorage::IGetPath(void) const
		{
			return storage_path;
		}

		filesystem::path::file CRC32BasedKeyStorage::IGetKeyPathForSpecifiedPath(const filesystem::path::file& path) const
		{
			const auto data = path.to_wstring();
			const auto size = sizeof(decltype(data)::value_type) * data.length();
			const auto checksum = checksum::crc32(data.c_str(), size, 0x04c11db7);
			const auto filename = convert::to_wstring(checksum) + std::wstring{ L".bin" };
			return storage_path + filename;
		}
	}
}
