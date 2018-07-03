#pragma once

#include <memory>
#include <vector> // private

#include "KAA/include/keywords.h"
#include "KAA/include/progress_state.h"
#include "KAA/include/stdtypes.h"

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
			AbsoluteSecurityCore(std::shared_ptr<filesystem::driver>, const filesystem::path::directory& key_storage_path);
			AbsoluteSecurityCore(const AbsoluteSecurityCore&) = delete;
			AbsoluteSecurityCore(AbsoluteSecurityCore&&) = delete;
			~AbsoluteSecurityCore() = default;

			AbsoluteSecurityCore& operator = (const AbsoluteSecurityCore&) = delete;
			AbsoluteSecurityCore& operator = (AbsoluteSecurityCore&&) = delete;

		private:
			std::shared_ptr<filesystem::driver> m_filesystem;
			std::auto_ptr<FileCipher> m_cipher;
			std::auto_ptr<KeyStorage> m_key_storage;
			std::auto_ptr<CipherProgressDispatcher> cipher_progress;

			CoreProgressHandler* core_progress;

			filesystem::path::directory IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(const filesystem::path::directory&) override;

			void IEncryptFile(const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&) override;

			bool IIsFileEncrypted(const filesystem::path::file&) const override;

			CoreProgressHandler* ISetProgressHandler(CoreProgressHandler*) override;

			std::auto_ptr< std::vector<uint8_t> > GenerateKey(size_t bytes_to_generate);
			void CreateKeyFile(const std::wstring& path, const std::vector<uint8_t>& data);

			progress_state OperationStarted(const std::wstring& name);
			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
			progress_state OverallProgress(uint64_t total_bytes_processed, uint64_t total_size);
		};
	}
}
