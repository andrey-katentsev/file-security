#include "CRC32BasedKeyStorage.h"

#include "KAA/include/convert.h"
#include "KAA/include/checksum.h"

namespace KAA
{
	namespace FileSecurity
	{
		CRC32BasedKeyStorage::CRC32BasedKeyStorage(const filesystem::path::directory& path) :
		path(path)
		{}

		void CRC32BasedKeyStorage::ISetPath(const filesystem::path::directory& path)
		{
			this->path = path;
		}

		filesystem::path::directory CRC32BasedKeyStorage::IGetPath(void) const
		{
			return path;
		}

		filesystem::path::file CRC32BasedKeyStorage::IGetKeyPathForSpecifiedPath(const filesystem::path::file& path) const
		{
			const auto data = path.to_wstring();
			const auto size = sizeof(decltype(data)::value_type) * data.length();
			const auto checksum = checksum::crc32(data.c_str(), size, 0x04c11db7);
			const std::wstring extension { L".bin" };
			const auto filename = convert::to_wstring(checksum) + extension;
			return this->path + filename;
		}
	}
}
