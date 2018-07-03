#pragma once

#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		class MD5BasedKeyStorage final : public KeyStorage
		{
		public:
			explicit MD5BasedKeyStorage(const filesystem::path::directory& key_storage_path);
			MD5BasedKeyStorage(const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage(MD5BasedKeyStorage&&) = delete;
			~MD5BasedKeyStorage() = default;

			MD5BasedKeyStorage& operator = (const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage& operator = (MD5BasedKeyStorage&&) = delete;

		private:
			filesystem::path::directory key_storage_path;

			void ISetPath(const filesystem::path::directory&) override;
			filesystem::path::directory IGetPath(void) const override;

			filesystem::path::file IGetKeyPathForSpecifiedPath(const filesystem::path::file&) const override;
		};
	}
}
