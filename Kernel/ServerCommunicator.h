// 28/01/2014

#pragma once

#include <memory>

#include "KAA/include/progress_state.h"
#include "KAA/include/stdtypes.h"

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

		class ServerCommunicator final : public Communicator
		{
		public:
			explicit ServerCommunicator(std::shared_ptr<filesystem::driver>);
			ServerCommunicator(const ServerCommunicator&) = delete;
			ServerCommunicator(ServerCommunicator&&) = delete;
			~ServerCommunicator() = default;

			ServerCommunicator& operator = (const ServerCommunicator&) = delete;
			ServerCommunicator& operator = (ServerCommunicator&&) = delete;

		private:
			std::auto_ptr<system::registry> m_registry;
			std::shared_ptr<filesystem::driver> m_filesystem;
			std::auto_ptr<filesystem::wiper> m_wiper;
			std::auto_ptr<Core> m_core;

			std::shared_ptr<CoreProgressDispatcher> core_progress;
			std::shared_ptr<WiperProgressDispatcher> wiper_progress;

			std::shared_ptr<CommunicatorProgressHandler> server_progress;

			void IEncryptFile(const filesystem::path::file& path) override;
			void IDecryptFile(const filesystem::path::file& path) override;

			bool IIsFileEncrypted(const filesystem::path::file& path) const override;

			std::vector< std::pair<std::wstring, core_id> > IGetAvailableCiphers(void) const override;
			core_id IGetCipher(void) const override;
			void ISetCipher(core_id) override;

			std::vector< std::pair<std::wstring, wipe_method_id> > IGetAvailableWipeMethods(void) const override;
			wipe_method_id IGetWipeMethod(void) const override;
			void ISetWipeMethod(wipe_method_id) override;

			filesystem::path::directory IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(const filesystem::path::directory&) override;

			std::shared_ptr<CommunicatorProgressHandler> ISetProgressHandler(std::shared_ptr<CommunicatorProgressHandler>) override;

			filesystem::path::file BackupFile(const filesystem::path::file& path);
			void CopyFile(const filesystem::path::file& source, const filesystem::path::file& destination);

			progress_state OperationStarted(const std::wstring& name);
			progress_state PortionProcessed(uint64_t total_processed, uint64_t total_size);
			progress_state OverallProgress(uint64_t total_processed, uint64_t total_size);
		};
	}
}
