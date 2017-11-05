#include "FileCipher.h"

namespace KAA
{
	namespace FileSecurity
	{
		FileCipher::~FileCipher()
		{}

		void FileCipher::EncryptFile(const std::wstring& file_to_encrypt_path, const std::wstring& key_file_path)
		{
			return IEncryptFile(file_to_encrypt_path, key_file_path);
		}

		void FileCipher::DecryptFile(const std::wstring& file_to_decrypt_path, const std::wstring& key_file_path)
		{
			return IDecryptFile(file_to_decrypt_path, key_file_path);
		}

		FileProgressHandler* FileCipher::SetProgressCallback(FileProgressHandler* handler)
		{
			return ISetProgressCallback(handler);
		}
	}
}
