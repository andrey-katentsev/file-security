#pragma once

#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CRC32BasedKeyStorage final : public KeyStorage
		{
		public:
			explicit CRC32BasedKeyStorage(filesystem::path::directory storage_path);
			~CRC32BasedKeyStorage() = default;

		private:
			void ISetPath(filesystem::path::directory) override;
			filesystem::path::directory IGetPath(void) const override;

			filesystem::path::file IGetKeyPathForSpecifiedPath(const filesystem::path::file&) const override;

			filesystem::path::directory storage_path;
		};
	}
}
