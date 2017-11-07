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

		// REMARKS: do not takes ownership over provided filesystem::driver*.
		// NOTE: Vernam Cipher / One-Time Pad
		class AbsoluteSecurityCore : public Core
		{
		public:
			AbsoluteSecurityCore(filesystem::driver*, const std::wstring& key_storage_path);
			~AbsoluteSecurityCore();

		private:
			filesystem::driver* m_filesystem;
			std::auto_ptr<FileCipher> m_cipher;
			std::auto_ptr<KeyStorage> m_key_storage;
			std::auto_ptr<CipherProgressDispatcher> cipher_progress;

			CoreProgressHandler* core_progress;

			std::wstring IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(const std::wstring&) override;

			void IEncryptFile(const std::wstring&) override;
			void IDecryptFile(const std::wstring&) override;

			bool IIsFileEncrypted(const std::wstring& path) const override;

			CoreProgressHandler* ISetProgressHandler(CoreProgressHandler*) override;

			std::auto_ptr< std::vector<uint8_t> > GenerateKey(size_t bytes_to_generate);
			void CreateKeyFile(const std::wstring& path, const std::vector<uint8_t>& data);

			progress_state OperationStarted(const std::wstring& name);
			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);
			progress_state OverallProgress(uint64_t total_bytes_processed, uint64_t total_size);

			// FUTURE: C++11: = delete
			AbsoluteSecurityCore(const AbsoluteSecurityCore&);
			AbsoluteSecurityCore& operator = (const AbsoluteSecurityCore&);
		};
	}
}
