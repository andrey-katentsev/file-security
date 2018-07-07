#include "GammaFileCipher.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "KAA/include/convert.h"
#include "KAA/include/cryptography.h"
#undef EncryptFile
#undef DecryptFile

#include "KAA/include/keywords.h"
#include "KAA/include/stdtypes.h"
#include "KAA/include/filesystem/driver.h"
#include "KAA/include/filesystem/path.h"
#include "KAA/include/exception/operation_failure.h"

#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		GammaFileCipher::GammaFileCipher(const std::shared_ptr<filesystem::driver> filesystem) :
		m_filesystem(filesystem),
		cipher_progress(nullptr)
		{
			if(nullptr == filesystem)
			{
				const std::wstring source { __FUNCTIONW__ };
				const std::wstring description { L"Unable to create a gamma file cipher class instance." };
				const KAA::operation_failure::status_code_t reason = KAA::operation_failure::R_INVALID_ARGUMENT;
				const KAA::operation_failure::severity_t severity = KAA::operation_failure::S_ERROR;
				throw operation_failure(source, description, reason, severity);
			}
		}

		void GammaFileCipher::IEncryptFile(const filesystem::path::file& path, const filesystem::path::file& key_path)
		{
			const filesystem::driver::mode random_read_write(true, true, true, true);
			const filesystem::driver::share exclusive_access(false, false);

			const auto master = m_filesystem->open_file(path, random_read_write, exclusive_access);

			const filesystem::driver::mode sequential_read_only(false);

			const auto key = m_filesystem->open_file(key_path, sequential_read_only, exclusive_access);

			const _fsize_t master_data_size = master->get_size();

			const size_t chunk_size = (64 * 1024) - 1;

			std::vector<uint8_t> master_buffer(chunk_size);
			std::vector<uint8_t> key_buffer(chunk_size);

			size_t bytes_read = 0;
			size_t total_bytes_written = 0;

			{
				bool stop = false;
				bool chunk_processed = false;
				progress_state progress = progress_continue;

				do 
				{
					{
						bytes_read = master->read(chunk_size, &master_buffer[0]);
						key->read(bytes_read, &key_buffer[0]);
						cryptography::gamma(&master_buffer[0], &key_buffer[0], &master_buffer[0], bytes_read);
						master->seek(-static_cast<_off_t>(bytes_read), filesystem::file::current);
						total_bytes_written += master->write(&master_buffer[0], bytes_read);
					}
					{
						chunk_processed = ( 0 != bytes_read );
						if(chunk_processed && ( progress_quiet != progress ))
							progress = ChunkProcessed(total_bytes_written, master_data_size);
						stop = ( !chunk_processed ) || ( progress_cancel == progress ) || ( progress_stop == progress );
					}
				} while(!stop);
			}
		}

		void GammaFileCipher::IDecryptFile(const filesystem::path::file& path, const filesystem::path::file& key)
		{
			return EncryptFile(path, key);
		}

		std::shared_ptr<FileProgressHandler> GammaFileCipher::ISetProgressCallback(std::shared_ptr<FileProgressHandler> handler)
		{
			const auto previous = cipher_progress;
			cipher_progress = handler;
			return previous;
		}

		progress_state GammaFileCipher::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			if(nullptr != cipher_progress)
				return cipher_progress->ChunkProcessed(total_bytes_processed, total_file_size);
			return progress_quiet;
		}
	}
}
