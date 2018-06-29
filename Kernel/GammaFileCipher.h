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
		// TODO: KAA: remove commented code.
		class GammaFileCipher : public FileCipher
		{
		public:
			//explicit GammaFileCipher(filesystem::driver*, progress_routine_t progress_notify, void* progress_context);
			//GammaFileCipher(filesystem::driver*, const cipher_progress_callback*);
			explicit GammaFileCipher(std::shared_ptr<filesystem::driver>);
			~GammaFileCipher();

			GammaFileCipher(const GammaFileCipher&) = delete;
			GammaFileCipher(GammaFileCipher&&) = delete;
			GammaFileCipher& operator = (const GammaFileCipher&) = delete;
			GammaFileCipher& operator = (GammaFileCipher&&) = delete;

		private:
			std::shared_ptr<filesystem::driver> m_filesystem;
			FileProgressHandler* cipher_progress;

			void IEncryptFile(const std::wstring&, const std::wstring&) override;
			void IDecryptFile(const std::wstring&, const std::wstring&) override;

			FileProgressHandler* ISetProgressCallback(FileProgressHandler*) override;

			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
		};
	}
}
