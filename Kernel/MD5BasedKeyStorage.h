#pragma once

#include <memory>

#include "KeyStorage.h"

namespace KAA
{
	namespace filesystem
	{
		class driver;
	}

	namespace FileSecurity
	{
		class MD5BasedKeyStorage final : public KeyStorage
		{
		public:
			MD5BasedKeyStorage(std::shared_ptr<filesystem::driver>, filesystem::path::directory storage_path);
			MD5BasedKeyStorage(const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage(MD5BasedKeyStorage&&) = delete;
			~MD5BasedKeyStorage() = default;

			MD5BasedKeyStorage& operator = (const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage& operator = (MD5BasedKeyStorage&&) = delete;

		private:
			void ISetPath(filesystem::path::directory) override;
			filesystem::path::directory IGetPath(void) const override;

			filesystem::path::file IGetKeyPathForSpecifiedPath(const filesystem::path::file&) const override;

			std::shared_ptr<filesystem::driver> filesystem;
			filesystem::path::directory storage_path;
		};
	}
}
