// 28/01/2014

#pragma once

#include <memory>

#include "SDK/KAA/include/keywords.h"
#include "SDK/KAA/include/progress_state.h"
#include "SDK/KAA/include/stdtypes.h"

#include "../Common/Communicator.h"

namespace KAA
{
	namespace system
	{
		class registry;
	}

	namespace filesystem
	{
		class driver;
		class wiper;
	}

	namespace FileSecurity
	{
		class Core;
		class CoreProgressDispatcher;
		class WiperProgressDispatcher;

		class ServerCommunicator : public Communicator
		{
		public:
			ServerCommunicator(std::auto_ptr<filesystem::driver>);

		private:
			std::auto_ptr<system::registry> m_registry;
			std::auto_ptr<filesystem::driver> m_filesystem;
			std::auto_ptr<filesystem::wiper> m_wiper;
			std::auto_ptr<Core> m_core;

			std::auto_ptr<CoreProgressDispatcher> core_progress;
			std::auto_ptr<WiperProgressDispatcher> wiper_progress;

			CommunicatorProgressHandler* server_progress;

			void IEncryptFile(const std::wstring& file_to_encrypt_path) override;
			void IDecryptFile(const std::wstring& file_to_decrypt_path) override;

			bool IIsFileEncrypted(const std::wstring& file_path) const override;

			std::vector< std::pair<std::wstring, core_id> > IGetAvailableCiphers(void) const override;
			core_id IGetCipher(void) const override;
			void ISetCipher(core_id) override;

			std::vector< std::pair<std::wstring, wipe_method_id> > IGetAvailableWipeMethods(void) const override;
			wipe_method_id IGetWipeMethod(void) const override;
			void ISetWipeMethod(wipe_method_id) override;

			std::wstring IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(const std::wstring& key_storage_path) override;

			CommunicatorProgressHandler* ISetProgressHandler(CommunicatorProgressHandler*) override;

			std::wstring CreateFileBackup(const std::wstring& path);
			void CopyFile(const std::wstring& source_path, const std::wstring& destination_path);

			progress_state OperationStarted(const std::wstring& name);
			progress_state PortionProcessed(uint64_t total_processed, uint64_t total_size);
			progress_state OverallProgress(uint64_t total_processed, uint64_t total_size);
		};
	}
}
