#pragma once

#include <memory>

#include "KAA/include/keywords.h"

#include "../Common/Communicator.h"

namespace KAA
{
	namespace FileSecurity
	{
		// THROWS: UserReport
		// SAFE GUARANTEE: basic
		// SIDE EFFECTS: -
		class ClientCommunicator : public Communicator
		{
		public:
			ClientCommunicator();
			ClientCommunicator(const ClientCommunicator&) = delete;
			ClientCommunicator(ClientCommunicator&&) = delete;
			~ClientCommunicator() = default;

			ClientCommunicator& operator = (const ClientCommunicator&) = delete;
			ClientCommunicator& operator = (ClientCommunicator&&) = delete;

		private:
			std::auto_ptr<Communicator> m_communicator;

			void IEncryptFile(const std::wstring& file_to_encrypt_path) override;
			void IDecryptFile(const std::wstring& file_to_decrypt_path) override;

			bool IIsFileEncrypted(const std::wstring& file_path) const override;

			std::vector< std::pair<std::wstring, core_id> > IGetAvailableCiphers(void) const override;
			core_id IGetCipher(void) const override;
			void ISetCipher(core_id) override;

			std::vector< std::pair<std::wstring, wipe_method_id> > IGetAvailableWipeMethods(void) const override;
			wipe_method_id IGetWipeMethod(void) const override;
			void ISetWipeMethod(wipe_method_id) override;

			filesystem::path::directory IGetKeyStoragePath(void) const override;
			void ISetKeyStoragePath(const filesystem::path::directory&) override;

			CommunicatorProgressHandler* ISetProgressHandler(CommunicatorProgressHandler*) override;
		};

		Communicator& GetCommunicator(void);
	}
}
