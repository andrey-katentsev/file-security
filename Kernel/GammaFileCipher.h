#pragma once

#include <memory>
#include <string>

// FUTURE: KAA: removed keywords.h
#include "KAA/include/keywords.h"
#include "KAA/include/progress_state.h"
#include "KAA/include/stdtypes.h"

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
			FileProgressHandler* cipher_progress;

			void IEncryptFile(const filesystem::path::file&, const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&, const filesystem::path::file&) override;

			FileProgressHandler* ISetProgressCallback(FileProgressHandler*) override;

			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
		};
	}
}
