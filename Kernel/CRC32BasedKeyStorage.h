#pragma once

#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CRC32BasedKeyStorage final : public KeyStorage
		{
		public:
			explicit CRC32BasedKeyStorage(const filesystem::path::directory&);
			~CRC32BasedKeyStorage() = default;

		private:
			filesystem::path::directory path;

			void ISetPath(const filesystem::path::directory&) override;
			filesystem::path::directory IGetPath(void) const override;

			filesystem::path::file IGetKeyPathForSpecifiedPath(const filesystem::path::file&) const override;
		};
	}
}
