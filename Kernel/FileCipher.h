#pragma once

#include <string>

namespace KAA
{
	namespace FileSecurity
	{
		class FileProgressHandler;

		// FUTURE: KAA: consider to improve progress callback design.
		class FileCipher
		{
		public:
			virtual ~FileCipher();

			void EncryptFile(const std::wstring& file_to_encrypt_path, const std::wstring& key_file_path);
			void DecryptFile(const std::wstring& file_to_decrypt_path, const std::wstring& key_file_path);

			FileProgressHandler* SetProgressCallback(FileProgressHandler*);

		private:
			virtual void IEncryptFile(const std::wstring&, const std::wstring&) = 0;
			virtual void IDecryptFile(const std::wstring&, const std::wstring&) = 0;

			virtual FileProgressHandler* ISetProgressCallback(FileProgressHandler*) = 0;
		};
	}
}
