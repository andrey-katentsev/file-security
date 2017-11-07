#pragma once

//#include <memory>
#include <string>

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

		// REMARKS: do not takes ownership on provided filesystem::driver*.
		class GammaFileCipher : public FileCipher
		{
		public:
			//explicit GammaFileCipher(filesystem::driver*, progress_routine_t progress_notify, void* progress_context);
			//GammaFileCipher(filesystem::driver*, const cipher_progress_callback*);
			explicit GammaFileCipher(filesystem::driver*);
			~GammaFileCipher();

		private:
			//std::auto_ptr<filesystem::driver> m_filesystem;
			filesystem::driver* m_filesystem;
			FileProgressHandler* cipher_progress;

			void IEncryptFile(const std::wstring&, const std::wstring&) override;
			void IDecryptFile(const std::wstring&, const std::wstring&) override;

			FileProgressHandler* ISetProgressCallback(FileProgressHandler*) override;

			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);

			// FUTURE: C++11: move semantics, = delete.
			GammaFileCipher(const GammaFileCipher&);
			GammaFileCipher& operator = (const GammaFileCipher&);
		};
	}
}
