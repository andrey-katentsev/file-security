#include "UserSessionKeyFileCipher.h"

#include <vector>

#include "KAA/include/cryptography/cryptography.h"
#include "KAA/include/exception/operation_failure.h"
#include "KAA/include/filesystem/driver.h"

#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		UserSessionKeyFileCipher::UserSessionKeyFileCipher(std::shared_ptr<filesystem::driver> driver) :
		filesystem(std::move(driver)),
		cipher_progress(nullptr)
		{
			if (!filesystem)
			{
				constexpr auto source = __FUNCTION__ ;
				constexpr auto description = "unable to create user session key based file cipher class instance";
				constexpr auto reason = operation_failure::status_code_t::invalid_argument;
				constexpr auto severity = operation_failure::severity_t::error;
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
			auto previous = cipher_progress;
			cipher_progress = handler;
			return previous;
		}

		progress_state UserSessionKeyFileCipher::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			return cipher_progress ? cipher_progress->ChunkProcessed(total_bytes_processed, total_file_size) : progress_quiet;
		}
	}
}
