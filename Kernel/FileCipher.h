#pragma once

#include <memory>

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
			virtual ~FileCipher() = default;

			void EncryptFile(const filesystem::path::file& path, const filesystem::path::file& key);
			void DecryptFile(const filesystem::path::file& path, const filesystem::path::file& key);

			std::shared_ptr<FileProgressHandler> SetProgressCallback(std::shared_ptr<FileProgressHandler>);

		private:
			virtual void IEncryptFile(const filesystem::path::file&, const filesystem::path::file&) = 0;
			virtual void IDecryptFile(const filesystem::path::file&, const filesystem::path::file&) = 0;

			virtual std::shared_ptr<FileProgressHandler> ISetProgressCallback(std::shared_ptr<FileProgressHandler>) = 0;
		};
	}
}
