#pragma once

#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		class MD5BasedKeyStorage : public KeyStorage
		{
		public:
			explicit MD5BasedKeyStorage(const filesystem::path::directory& key_storage_path);
			~MD5BasedKeyStorage();

			MD5BasedKeyStorage(const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage(MD5BasedKeyStorage&&) = delete;
			MD5BasedKeyStorage& operator = (const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage& operator = (MD5BasedKeyStorage&&) = delete;

		private:
			filesystem::path::directory key_storage_path;

			void ISetPath(const filesystem::path::directory&);
			filesystem::path::directory IGetPath(void) const;

			std::wstring IGetKeyPathForSpecifiedPath(const std::wstring&) const;
		};
	}
}
