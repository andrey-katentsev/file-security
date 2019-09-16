#include "ClientCommunicator.h"

#include "KAA/include/format_string.h"
#include "KAA/include/load_string.h"
#include "KAA/include/exception/failure.h"
#undef EncryptFile
#undef DecryptFile

#include "../Kernel/Kernel.h"
#include "../Common/UserReport.h"

#include "resource.h"

namespace
{
	std::wstring CombineMessage(const unsigned requested_string_index, const std::wstring& system_message)
	{
		return KAA::resources::load_string(requested_string_index) + L'\n' + system_message;
	}

	__declspec(noreturn) void ThrowUserReport(const KAA::failure& error, const KAA::FileSecurity::UserReport::severity_t severity, const UINT text_id)
	{
		const auto message = CombineMessage(text_id, error.get_system_message());
		throw KAA::FileSecurity::UserReport(message, severity);
	}
}

namespace KAA
{
	namespace FileSecurity
	{
		ClientCommunicator::ClientCommunicator() :
		m_communicator(KAA::FileSecurity::GetClassObject())
		{}

		void ClientCommunicator::IEncryptFile(const filesystem::path::file& path)
		try
		{
			return m_communicator->EncryptFile(path);
		}
		catch(const failure& error)
		{
			ThrowUserReport(error, UserReport::warning, IDS_UNABLE_TO_COMPLETE_ENCRYPT_FILE_OPERATION);
		}

		void ClientCommunicator::IDecryptFile(const filesystem::path::file& path)
		try
		{
			return m_communicator->DecryptFile(path);
		}
		catch(const failure& error)
		{
			ThrowUserReport(error, UserReport::warning, IDS_UNABLE_TO_COMPLETE_DECRYPT_FILE_OPERATION);
		}

		bool ClientCommunicator::IIsFileEncrypted(const filesystem::path::file& path) const
		try
		{
			return m_communicator->IsFileEncrypted(path);
		}
		catch(const failure& error)
		{
			ThrowUserReport(error, UserReport::warning, IDS_UNABLE_TO_DETERMINE_FILE_STATE);
		}

		std::vector<std::pair<std::wstring, core_id>> ClientCommunicator::IGetAvailableCiphers(void) const
		{
			return m_communicator->GetAvailableCiphers();
		}

		core_id ClientCommunicator::IGetCipher(void) const
		{
			return m_communicator->GetCipher();
		}

		void ClientCommunicator::ISetCipher(const core_id cipher)
		{
			return m_communicator->SetCipher(cipher);
		}

		std::vector<std::pair<std::wstring, wipe_method_id>> ClientCommunicator::IGetAvailableWipeMethods(void) const
		{
			return m_communicator->GetAvailableWipeMethods();
		}

		wipe_method_id ClientCommunicator::IGetWipeMethod(void) const
		{
			return m_communicator->GetWipeMethod();
		}

		void ClientCommunicator::ISetWipeMethod(const wipe_method_id algorithm)
		{
			return m_communicator->SetWipeMethod(algorithm);
		}

		filesystem::path::directory ClientCommunicator::IGetKeyStoragePath(void) const
		try
		{
			return m_communicator->GetKeyStoragePath();
		}
		catch(const failure& error)
		{
			ThrowUserReport(error, UserReport::error, IDS_UNABLE_TO_RETRIEVE_KEY_STORAGE_PATH);
		}

		void ClientCommunicator::ISetKeyStoragePath(filesystem::path::directory path)
		try
		{
			return m_communicator->SetKeyStoragePath(path);
		}
		catch(const failure& error)
		{
			const auto message_format = CombineMessage(IDS_UNABLE_TO_CREATE_KEY_STORAGE_AT_FORMAT, error.get_system_message());
			const auto message = KAA::format_string(message_format, path.to_wstring().c_str());
			throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::error);
		}

		std::shared_ptr<CommunicatorProgressHandler> ClientCommunicator::ISetProgressHandler(std::shared_ptr<CommunicatorProgressHandler> handler)
		try
		{
			return m_communicator->SetProgressHandler(std::move(handler));
		}
		catch(const failure& error)
		{
			ThrowUserReport(error, UserReport::error, IDS_UNABLE_TO_SETUP_PROGRESS_HANDLER);
		}

		Communicator& GetCommunicator(void)
		try
		{
			static ClientCommunicator dll_api;
			return dll_api;
		}
		catch(const failure& error)
		{
			ThrowUserReport(error, UserReport::error, IDS_UNABLE_TO_CREATE_COMMUNICATOR);
		}
	}
}
