#pragma once

#include <cstdint>

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

		class GammaFileCipher final : public FileCipher
		{
		public:
			explicit GammaFileCipher(std::shared_ptr<filesystem::driver>);
			GammaFileCipher(const GammaFileCipher&) = delete;
			GammaFileCipher(GammaFileCipher&&) = delete;
			~GammaFileCipher() = default;

			GammaFileCipher& operator = (const GammaFileCipher&) = delete;
			GammaFileCipher& operator = (GammaFileCipher&&) = delete;

		private:
			std::shared_ptr<filesystem::driver> m_filesystem;
			std::shared_ptr<FileProgressHandler> cipher_progress;

			void IEncryptFile(const filesystem::path::file&, const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&, const filesystem::path::file&) override;

			std::shared_ptr<FileProgressHandler> ISetProgressCallback(std::shared_ptr<FileProgressHandler>) override;

			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
		};
	}
}
