#include "FileCipher.h"

namespace KAA
{
	namespace FileSecurity
	{
		FileCipher::~FileCipher()
		{}

		void FileCipher::EncryptFile(const filesystem::path::file& path, const filesystem::path::file& key)
		{
			return IEncryptFile(path, key);
		}

		void FileCipher::DecryptFile(const filesystem::path::file& path, const filesystem::path::file& key)
		{
			return IDecryptFile(path, key);
		}

		FileProgressHandler* FileCipher::SetProgressCallback(FileProgressHandler* handler)
		{
			return ISetProgressCallback(handler);
		}
	}
}
