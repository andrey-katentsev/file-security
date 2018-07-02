#pragma once

#include <string>

namespace KAA
{
	namespace filesystem
	{
		namespace path
		{
			class file;
		}
	}

	namespace FileSecurity
	{
		class FileProgressHandler;

		// FUTURE: KAA: consider to improve progress callback design.
		class FileCipher
		{
		public:
			virtual ~FileCipher();

			void EncryptFile(const filesystem::path::file& path, const filesystem::path::file& key);
			void DecryptFile(const filesystem::path::file& path, const filesystem::path::file& key);

			FileProgressHandler* SetProgressCallback(FileProgressHandler*);

		private:
			virtual void IEncryptFile(const filesystem::path::file&, const filesystem::path::file&) = 0;
			virtual void IDecryptFile(const filesystem::path::file&, const filesystem::path::file&) = 0;

			virtual FileProgressHandler* ISetProgressCallback(FileProgressHandler*) = 0;
		};
	}
}
