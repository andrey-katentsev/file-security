#pragma once

#include "KeyStorage.h"

namespace KAA
{
	namespace FileSecurity
	{
		class MD5BasedKeyStorage : public KeyStorage
		{
		public:
			explicit MD5BasedKeyStorage(const std::wstring& key_storage_path);
			MD5BasedKeyStorage(const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage(MD5BasedKeyStorage&&) = delete;
			MD5BasedKeyStorage& operator = (const MD5BasedKeyStorage&) = delete;
			MD5BasedKeyStorage& operator = (MD5BasedKeyStorage&&) = delete;
			~MD5BasedKeyStorage();

		private:
			std::wstring key_storage_path;

			void ISetPath(const std::wstring&);
			std::wstring IGetPath(void) const;

			std::wstring IGetKeyPathForSpecifiedPath(const std::wstring&) const;
		};
	}
}
