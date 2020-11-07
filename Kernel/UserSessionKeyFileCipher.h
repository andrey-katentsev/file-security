#pragma once

#include "KAA/include/progress_state.h"

#include "FileCipher.h"

namespace KAA
{
	namespace filesystem
	{
		class driver;
	}

	namespace FileSecurity
	{
		class FileProgressHandler;

		class UserSessionKeyFileCipher final : public FileCipher
		{
		public:
			explicit UserSessionKeyFileCipher(std::shared_ptr<filesystem::driver>);
			UserSessionKeyFileCipher(const UserSessionKeyFileCipher&) = delete;
			UserSessionKeyFileCipher(UserSessionKeyFileCipher&&) = delete;
			~UserSessionKeyFileCipher() = default;

			UserSessionKeyFileCipher& operator = (const UserSessionKeyFileCipher&) = delete;
			UserSessionKeyFileCipher& operator = (UserSessionKeyFileCipher&&) = delete;

		private:
			std::shared_ptr<filesystem::driver> filesystem;
			std::shared_ptr<FileProgressHandler> cipher_progress;

			void IEncryptFile(const filesystem::path::file&, const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&, const filesystem::path::file&) override;

			std::shared_ptr<FileProgressHandler> ISetProgressCallback(std::shared_ptr<FileProgressHandler>) override;

			progress_state_t ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
		};
	}
}
