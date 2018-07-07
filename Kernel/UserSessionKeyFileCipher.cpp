#include "UserSessionKeyFileCipher.h"

#include "KAA/include/exception/operation_failure.h"

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

		void UserSessionKeyFileCipher::IEncryptFile(const filesystem::path::file& path, const filesystem::path::file& key_path)
		{
			throw operation_failure(__FUNCTIONW__, L"cannot encrypt file", operation_failure::R_NOT_IMPLEMENTED, operation_failure::S_ERROR);
		}

		void UserSessionKeyFileCipher::IDecryptFile(const filesystem::path::file& path, const filesystem::path::file& key_path)
		{
			throw operation_failure(__FUNCTIONW__, L"cannot decrypt file", operation_failure::R_NOT_IMPLEMENTED, operation_failure::S_ERROR);
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
