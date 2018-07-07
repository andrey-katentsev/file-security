#include "FileCipher.h"

namespace KAA
{
	namespace FileSecurity
	{
		void FileCipher::EncryptFile(const filesystem::path::file& path, const filesystem::path::file& key)
		{
			return IEncryptFile(path, key);
		}

		void FileCipher::DecryptFile(const filesystem::path::file& path, const filesystem::path::file& key)
		{
			return IDecryptFile(path, key);
		}

		std::shared_ptr<FileProgressHandler> FileCipher::SetProgressCallback(std::shared_ptr<FileProgressHandler> handler)
		{
			return ISetProgressCallback(handler);
		}
	}
}
