#include "UserSessionKeyFileCipher.h"

#include <vector>

#include "KAA/include/cryptography.h"
#undef EncryptFile
#undef DecryptFile

#include "KAA/include/exception/operation_failure.h"
#include "KAA/include/filesystem/driver.h"

#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		UserSessionKeyFileCipher::UserSessionKeyFileCipher(const std::shared_ptr<filesystem::driver> filesystem) :
		filesystem(filesystem),
		cipher_progress(nullptr)
		{
			if (nullptr == filesystem)
			{
				const std::wstring source { __FUNCTIONW__ };
				const std::wstring description { L"failed to create 'user session key' file cipher class instance" };
				constexpr auto reason = operation_failure::R_INVALID_ARGUMENT;
				constexpr auto severity = operation_failure::S_ERROR;
				throw operation_failure(source, description, reason, severity);
			}
		}

		void UserSessionKeyFileCipher::IEncryptFile(const filesystem::path::file& path, const filesystem::path::file&)
		{
			const filesystem::driver::mode serial_read_write { true, true, true, false };
			const filesystem::driver::share exclusive_access { false, false };
			const auto master = filesystem->open_file(path, serial_read_write, exclusive_access);

			const auto data_size = master->get_size();
			std::vector<uint8_t> master_buffer(data_size);

			const auto bytes_read = master->read(data_size, &master_buffer[0]);
			master_buffer = cryptography::protect_data(&master_buffer[0], bytes_read);
			master->seek(0, filesystem::file::set);
			const auto bytes_written = master->write(&master_buffer[0], bytes_read);

			if (0 != bytes_read)
				ChunkProcessed(bytes_written, data_size);
		}

		void UserSessionKeyFileCipher::IDecryptFile(const filesystem::path::file& path, const filesystem::path::file&)
		{
			const filesystem::driver::mode serial_read_write { true, true, true, false };
			const filesystem::driver::share exclusive_access { false, false };
			const auto master = filesystem->open_file(path, serial_read_write, exclusive_access);

			const auto data_size = master->get_size();
			std::vector<uint8_t> master_buffer(data_size);

			const auto bytes_read = master->read(data_size, &master_buffer[0]);
			master_buffer = cryptography::unprotect_data(&master_buffer[0], bytes_read);
			master->seek(0, filesystem::file::set);
			const auto bytes_written = master->write(&master_buffer[0], bytes_read);

			if (0 != bytes_read)
				ChunkProcessed(bytes_written, data_size);
		}

		std::shared_ptr<FileProgressHandler> UserSessionKeyFileCipher::ISetProgressCallback(std::shared_ptr<FileProgressHandler> handler)
		{
			const auto previous = cipher_progress;
			cipher_progress = handler;
			return previous;
		}

		progress_state UserSessionKeyFileCipher::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			return cipher_progress ? cipher_progress->ChunkProcessed(total_bytes_processed, total_file_size) : progress_quiet;
		}
	}
}
