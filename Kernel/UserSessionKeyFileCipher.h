#pragma once

#include <memory>

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

		// FUTURE: KAA: std::shared_ptr.
		// TODO: KAA: remove commented code.
		class UserSessionKeyFileCipher final : public FileCipher
		{
		public:
			explicit UserSessionKeyFileCipher(std::shared_ptr<filesystem::driver>);

			/*UserSessionKeyFileCipher(const UserSessionKeyFileCipher&) = delete;
			UserSessionKeyFileCipher(UserSessionKeyFileCipher&&) = delete;
			UserSessionKeyFileCipher& operator = (const UserSessionKeyFileCipher&) = delete;
			UserSessionKeyFileCipher& operator = (UserSessionKeyFileCipher&&) = delete;*/

		private:
			std::shared_ptr<filesystem::driver> filesystem;
			FileProgressHandler* cipher_progress;

			void IEncryptFile(const filesystem::path::file&, const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&, const filesystem::path::file&) override;

			FileProgressHandler* ISetProgressCallback(FileProgressHandler*) override;

			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
		};
	}
}
