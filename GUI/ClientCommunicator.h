#pragma once

#include <memory>

#include "../Common/Communicator.h"

namespace KAA
{
	namespace FileSecurity
	{
		// THROWS: UserReport
		// SAFE GUARANTEE: basic
		// SIDE EFFECTS: -
		class ClientCommunicator final : public Communicator
		{
		public:
			ClientCommunicator();
			ClientCommunicator(const ClientCommunicator&) = delete;
			ClientCommunicator(ClientCommunicator&&) = delete;
			~ClientCommunicator() = default;

			ClientCommunicator& operator = (const ClientCommunicator&) = delete;
			ClientCommunicator& operator = (ClientCommunicator&&) = delete;

		private:
			std::unique_ptr<Communicator> m_communicator;

			void IEncryptFile(const filesystem::path::file&) override;
			void IDecryptFile(const filesystem::path::file&) override;

			bool IIsFileEncrypted(const filesystem::path::file&) const override;

			std::vector<std::pair<std::wstring, core_id>> IGetAvailableCiphers(void) const override;
			core_id IGetCipher(void) const override;
			void ISetCipher(core_id) override;

			std::vector<std::pair<std::wstring, wipe_method_id>> IGetAvailableWipeMethods(void) const override;
			wipe_method_id IGetWipeMethod(void) const override;
			void ISetWipeMethod(wipe_method_id) override;

			filesystem::path::directory IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(filesystem::path::directory) override;

			std::shared_ptr<CommunicatorProgressHandler> ISetProgressHandler(std::shared_ptr<CommunicatorProgressHandler>) override;
		};

		Communicator& GetCommunicator(void);
	}
}
