#pragma once

#include <vector>
#include <cstdint>

#include "KAA/include/progress_state.h"

#include "./Core/Core.h"

namespace KAA
{
	namespace filesystem
	{
		class driver;
	}

	namespace FileSecurity
	{
		class FileCipher;
		class KeyStorage;

		class CoreProgressHandler;
		class CipherProgressDispatcher;

		// NOTE: Vernam Cipher / One-Time Pad
		class AbsoluteSecurityCore final : public Core
		{
		public:
			AbsoluteSecurityCore(std::shared_ptr<filesystem::driver>, filesystem::path::directory key_storage_path);
			AbsoluteSecurityCore(const AbsoluteSecurityCore&) = delete;
			AbsoluteSecurityCore(AbsoluteSecurityCore&&) = delete;
			~AbsoluteSecurityCore();

			AbsoluteSecurityCore& operator = (const AbsoluteSecurityCore&) = delete;
			AbsoluteSecurityCore& operator = (AbsoluteSecurityCore&&) = delete;

		private:
			std::shared_ptr<filesystem::driver> m_filesystem;
			std::unique_ptr<FileCipher> m_cipher;
			std::unique_ptr<KeyStorage> m_key_storage;
			std::shared_ptr<CipherProgressDispatcher> cipher_progress;

			std::shared_ptr<CoreProgressHandler> core_progress;

			filesystem::path::directory IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(filesystem::path::directory) override;

			void IEncryptFile(const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&) override;

			bool IIsFileEncrypted(const filesystem::path::file&) const override;

			std::shared_ptr<CoreProgressHandler> ISetProgressHandler(std::shared_ptr<CoreProgressHandler>) override;

			std::vector<uint8_t> GenerateKey(size_t bytes_to_generate);
			void CreateKeyFile(const filesystem::path::file& path, const std::vector<uint8_t>& data);

			progress_state_t OperationStarted(const std::string& name, uint64_t file_size);
			progress_state_t ChunkProcessed(uint64_t overall_bytes_processed);
		};
	}
}
