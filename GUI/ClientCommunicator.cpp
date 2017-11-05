#include "ClientCommunicator.h"

#include "SDK/KAA/include/format_string.h"
#include "SDK/KAA/include/load_string.h"
#include "SDK/KAA/include/exception/failure.h"
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
}

namespace KAA
{
	namespace FileSecurity
	{
		ClientCommunicator::ClientCommunicator() :
		m_communicator(KAA::FileSecurity::GetClassObject())
		{}

		void ClientCommunicator::IEncryptFile(const std::wstring& file_to_encrypt_path)
		{
			try
			{
				return m_communicator->EncryptFile(file_to_encrypt_path);
			}
			catch(const failure& error)
			{
				const std::wstring message(CombineMessage(IDS_UNABLE_TO_COMPLETE_ENCRYPT_FILE_OPERATION, error.get_system_message()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::warning);
			}
		}

		void ClientCommunicator::IDecryptFile(const std::wstring& file_to_decrypt_path)
		{
			try
			{
				return m_communicator->DecryptFile(file_to_decrypt_path);
			}
			catch(const failure& error)
			{
				const std::wstring message(CombineMessage(IDS_UNABLE_TO_COMPLETE_DECRYPT_FILE_OPERATION, error.get_system_message()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::warning);
			}
		}

		bool ClientCommunicator::IIsFileEncrypted(const std::wstring& path) const
		{
			try
			{
				return m_communicator->IsFileEncrypted(path);
			}
			catch(const failure& error)
			{
				const std::wstring message(CombineMessage(IDS_UNABLE_TO_DETERMINE_FILE_STATE, error.get_system_message()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::warning);
			}
		}

		std::vector< std::pair<std::wstring, core_id> > ClientCommunicator::IGetAvailableCiphers(void) const
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

		std::vector< std::pair<std::wstring, wipe_method_id> > ClientCommunicator::IGetAvailableWipeMethods(void) const
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

		std::wstring ClientCommunicator::IGetKeyStoragePath(void) const
		{
			try
			{
				return m_communicator->GetKeyStoragePath();
			}
			catch(const failure& error)
			{
				const std::wstring message(CombineMessage(IDS_UNABLE_TO_RETRIEVE_KEY_STORAGE_PATH, error.get_system_message()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::error);
			}
		}

		void ClientCommunicator::ISetKeyStoragePath(const std::wstring& key_storage_path)
		{
			try
			{
				return m_communicator->SetKeyStoragePath(key_storage_path);
			}
			catch(const failure& error)
			{
				const std::wstring message_format(CombineMessage(IDS_UNABLE_TO_CREATE_KEY_STORAGE_AT_FORMAT, error.get_system_message()));
				const std::wstring message(KAA::format_string(message_format, key_storage_path.c_str()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::error);
			}
		}

		CommunicatorProgressHandler* ClientCommunicator::ISetProgressHandler(CommunicatorProgressHandler* handler)
		{
			try
			{
				return m_communicator->SetProgressHandler(handler);
			}
			catch(const failure& error)
			{
				const std::wstring message(CombineMessage(IDS_UNABLE_TO_SETUP_PROGRESS_HANDLER, error.get_system_message()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::error);
			}
		}

		Communicator& GetCommunicator(void)
		{
			try
			{
				static ClientCommunicator dll_api;
				return dll_api;
			}
			catch(const failure& error)
			{
				const std::wstring message(CombineMessage(IDS_UNABLE_TO_CREATE_COMMUNICATOR, error.get_system_message()));
				throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::error);
			}
		}
	}
}
