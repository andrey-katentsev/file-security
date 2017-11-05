// 28/01/2014

#include "ServerCommunicator.h"

#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <cerrno>

#include "SDK/KAA/include/keywords.h"
#include "SDK/KAA/include/load_string.h"
#include "SDK/KAA/include/module_context.h"
#include "SDK/KAA/include/random.h"
#include "SDK/KAA/include/registry_key.h"
#include "SDK/KAA/include/windows_registry.h"
#include "SDK/KAA/include/exception/windows_api_failure.h"
#undef EncryptFile
#undef DecryptFile
#undef CopyFile
#undef max

#include "SDK/KAA/include/exception/system_failure.h"
#include "SDK/KAA/include/filesystem/driver.h"
#include "SDK/KAA/include/filesystem/filesystem.h"
#include "SDK/KAA/include/filesystem/ordinary_file_remover.h"
#include "SDK/KAA/include/filesystem/simple_overwrite_wiper.h"
#include "SDK/KAA/include/filesystem/split_path.h"
#include "SDK/KAA/include/filesystem/wiper.h"

#include "./Core/Core.h"

#include "CoreFactory.h"
#include "AbsoluteSecurityCore.h"
#include "StrongSecurityCore.h"

#include "RegistryFactory.h"
#include "WiperFactory.h"

#include "CoreProgressDispatcher.h"
#include "WiperProgressDispatcher.h"

#include "../Common/CommunicatorProgressHandler.h"
#include "../Common/Features.h"

#include "resource.h"

extern KAA::module_context core_dll;

namespace
{
	const std::wstring registry_software_sub_key(L"Software\\Hyperlink Software\\File Security");
	const std::wstring registry_wipe_algorithm_value_name(L"WipeMethod");
	const std::wstring registry_core_value_name(L"Engine");
	const std::wstring registry_key_storage_path_value_name(L"KeyStoragePath");

	KAA::FileSecurity::wipe_method_id ToWipeMethodID(const KAA::FileSecurity::wiper_t wipe_algorithm)
	{
		switch(wipe_algorithm)
		{
		case KAA::FileSecurity::ordinary_remove:
			return 0x01;
		case KAA::FileSecurity::simple_overwrite:
			return 0x02;
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	KAA::FileSecurity::wiper_t ToWiperType(const KAA::FileSecurity::wipe_method_id value)
	{
		switch(value)
		{
		case 0x01:
			return KAA::FileSecurity::ordinary_remove;
		case 0x02:
			return KAA::FileSecurity::simple_overwrite;
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	// DEFECT: KAA: DRY violation.
	// DEFECT: KAA: SRP violation.
	KAA::FileSecurity::wiper_t QueryWiperType(KAA::system::registry* registry)
	{
		try
		{
			const KAA::system::registry::key_access query_value = { false, false, false, false, true, false };
			const std::auto_ptr<KAA::system::registry_key> software_root(registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, query_value));
			const DWORD value = software_root->query_dword_value(registry_wipe_algorithm_value_name);
			return ToWiperType(static_cast<KAA::FileSecurity::wipe_method_id>(value));
		}
		catch(const KAA::windows_api_failure& error)
		{
			if(ERROR_FILE_NOT_FOUND == error)
			{
				const KAA::FileSecurity::wiper_t default_wipe_algorithm = KAA::FileSecurity::ordinary_remove;
				const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
				const std::auto_ptr<KAA::system::registry_key> software_root(registry->create_key(KAA::system::registry::current_user, registry_software_sub_key, KAA::system::registry::persistent, set_value));
				software_root->set_dword_value(registry_wipe_algorithm_value_name, ToWipeMethodID(default_wipe_algorithm));
				return default_wipe_algorithm;
			}
			throw;
		}
	}

	// FUTURE: KAA: DRY violation.
	void SaveWiperType(KAA::system::registry* registry, const KAA::FileSecurity::wiper_t wipe_algorithm)
	{
		const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
		const std::auto_ptr<KAA::system::registry_key> software_root(registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, set_value));
		software_root->set_dword_value(registry_wipe_algorithm_value_name, ToWipeMethodID(wipe_algorithm));
	}

	KAA::FileSecurity::core_id ToCoreID(const KAA::FileSecurity::core_t engine)
	{
		switch(engine)
		{
		case KAA::FileSecurity::strong_security:
			return 0x01;
		case KAA::FileSecurity::absolute_security:
			return 0x02;
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	KAA::FileSecurity::core_t ToCoreType(const KAA::FileSecurity::core_id value)
	{
		switch(value)
		{
		case 0x01:
			return KAA::FileSecurity::strong_security;
		case 0x02:
			return KAA::FileSecurity::absolute_security;
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	KAA::FileSecurity::core_t QueryCoreType(KAA::system::registry* const registry)
	{
		try
		{
			const KAA::system::registry::key_access query_value = { false, false, false, false, true, false };
			const std::auto_ptr<KAA::system::registry_key> software_root(registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, query_value));
			const DWORD value = software_root->query_dword_value(registry_core_value_name);
			return ToCoreType(static_cast<KAA::FileSecurity::core_id>(value));
		}
		catch(const KAA::windows_api_failure& error)
		{
			if(ERROR_FILE_NOT_FOUND == error)
			{
				const KAA::FileSecurity::core_t default_core = KAA::FileSecurity::absolute_security; // FUTURE: KAA: introduce and change to strong security.
				const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
				const std::auto_ptr<KAA::system::registry_key> software_root(registry->create_key(KAA::system::registry::current_user, registry_software_sub_key, KAA::system::registry::persistent, set_value));
				software_root->set_dword_value(registry_core_value_name, ToCoreID(default_core));
				return default_core;
			}
			throw;
		}
	}

	void SaveCoreType(KAA::system::registry* const registry, const KAA::FileSecurity::core_t engine)
	{
		const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
		const std::auto_ptr<KAA::system::registry_key> software_root(registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, set_value));
		return software_root->set_dword_value(registry_core_value_name, ToCoreID(engine));
	}

	std::wstring QueryKeyStoragePath(KAA::system::registry* const registry)
	{
		try
		{
			const KAA::system::registry::key_access query_value = { false, false, false, false, true, false };
			const std::auto_ptr<KAA::system::registry_key> software_root(registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, query_value));
			return software_root->query_string_value(registry_key_storage_path_value_name);
		}
		catch(const KAA::windows_api_failure& error)
		{
			if(ERROR_FILE_NOT_FOUND == error)
			{
				const std::wstring default_key_storage_path(L".\\Keys\\");
				const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
				const std::auto_ptr<KAA::system::registry_key> software_root(registry->create_key(KAA::system::registry::current_user, registry_software_sub_key, KAA::system::registry::persistent, set_value));
				software_root->set_string_value(registry_key_storage_path_value_name, default_key_storage_path);
				return default_key_storage_path;
			}
			throw;
		}
	}

	void SaveKeyStoragePath(KAA::system::registry* const registry, const std::wstring& key_storage_path)
	{
		const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
		const std::auto_ptr<KAA::system::registry_key> software_root(registry->open_key(KAA::system::registry::current_user, registry_software_sub_key, set_value));
		return software_root->set_string_value(registry_key_storage_path_value_name, key_storage_path);
	}

	std::auto_ptr<KAA::system::registry> QueryRegistry(const KAA::FileSecurity::registry_t interface_identifier)
	{
		switch(interface_identifier)
		{
		case KAA::FileSecurity::windows_registry:
			return std::auto_ptr<KAA::system::registry>(new KAA::system::windows_registry);
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	std::auto_ptr<KAA::filesystem::wiper> QueryWiper(const KAA::FileSecurity::wiper_t interface_identifier, KAA::filesystem::driver* const filesystem)
	{
		switch(interface_identifier)
		{
		case KAA::FileSecurity::ordinary_remove:
			return std::auto_ptr<KAA::filesystem::wiper>(new KAA::filesystem::ordinary_file_remover(filesystem));
		case KAA::FileSecurity::simple_overwrite:
			{
				const uint8_t aggregate = KAA::cryptography::random() % std::numeric_limits<uint8_t>::max();
				return std::auto_ptr<KAA::filesystem::wiper>(new KAA::filesystem::simple_owerwrite_wiper(filesystem, aggregate));
			}
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	std::auto_ptr<KAA::FileSecurity::Core> QueryCore(const KAA::FileSecurity::core_t interface_id, KAA::filesystem::driver* filesystem, const std::wstring& key_storage_path)
	{
		switch(interface_id)
		{
		case KAA::FileSecurity::strong_security:
			throw std::invalid_argument(__FUNCTION__);
		case KAA::FileSecurity::absolute_security:
			return std::auto_ptr<KAA::FileSecurity::Core>(new KAA::FileSecurity::AbsoluteSecurityCore(filesystem, key_storage_path));
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	void RemoveFileBackup(KAA::filesystem::driver* const filesystem, const std::wstring& path)
	{
		filesystem->remove_file(path);
	}
}

namespace KAA
{
	namespace FileSecurity
	{
		ServerCommunicator::ServerCommunicator(std::auto_ptr<filesystem::driver> filesystem) :
		m_registry(QueryRegistry(windows_registry)),
		m_filesystem(filesystem),
		m_wiper(QueryWiper(QueryWiperType(m_registry.get()), m_filesystem.get())),
		m_core(QueryCore(QueryCoreType(m_registry.get()), m_filesystem.get(), QueryKeyStoragePath(m_registry.get()))),
		core_progress(new CoreProgressDispatcher),
		wiper_progress(new WiperProgressDispatcher),
		server_progress(nullptr)
		{
			try
			{
				m_filesystem->create_directory(m_core->GetKeyStoragePath());
			}
			catch(const KAA::system_failure& error)
			{
				if(EEXIST != error)
					throw;
				// FIX: KAA: this should be a part of exception
				//{
					//const std::wstring message(L"Unable to create " + m_core->GetKeyStoragePath() + L".\nSystem message: " + error.format_message());
					//throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::error);
				//}
			}
		}

		void ServerCommunicator::IEncryptFile(const std::wstring& file_to_encrypt_path)
		{
			const size_t file_size = get_file_size(*m_filesystem.get(), file_to_encrypt_path);
			const size_t overall_size = 3*file_size;

			OperationStarted(resources::load_string(IDS_CREATING_BACKUP, core_dll.get_module_handle()));
			const std::wstring backup_file_path(CreateFileBackup(file_to_encrypt_path));

			OperationStarted(resources::load_string(IDS_ENCRYPTING_FILE, core_dll.get_module_handle()));
			m_core->EncryptFile(file_to_encrypt_path);

			OperationStarted(resources::load_string(IDS_WIPING_FILE, core_dll.get_module_handle()));
			m_wiper->wipe_file(backup_file_path);
		}

		void ServerCommunicator::IDecryptFile(const std::wstring& file_to_decrypt_path)
		{
			const size_t file_size = get_file_size(*m_filesystem.get(), file_to_decrypt_path);
			const size_t overall_size = 3*file_size;

			OperationStarted(resources::load_string(IDS_CREATING_BACKUP, core_dll.get_module_handle()));
			const std::wstring backup_file_path(CreateFileBackup(file_to_decrypt_path));

			OperationStarted(resources::load_string(IDS_DECRYPTING_FILE, core_dll.get_module_handle()));
			m_core->DecryptFile(file_to_decrypt_path);

			OperationStarted(resources::load_string(IDS_REMOVING_BACKUP, core_dll.get_module_handle()));
			m_filesystem->remove_file(backup_file_path);
		}

		bool ServerCommunicator::IIsFileEncrypted(const std::wstring& path) const
		{
			return m_core->IsFileEncrypted(path);
		}

		// FIX: KAA: implement communicator.
		std::vector< std::pair<std::wstring, core_id> > ServerCommunicator::IGetAvailableCiphers(void) const
		{
			throw std::exception(__FUNCTION__);
		}

		core_id ServerCommunicator::IGetCipher(void) const
		{
			return ToCoreID(QueryCoreType(m_registry.get()));
		}

		void ServerCommunicator::ISetCipher(const core_id value)
		{
			const core_t engine = ToCoreType(value);
			const std::wstring current_key_storage_path(m_core->GetKeyStoragePath());
			m_core.reset(QueryCore(engine, m_filesystem.get(), current_key_storage_path).release());
			SaveCoreType(m_registry.get(), engine);
		}

		std::vector< std::pair<std::wstring, wipe_method_id> > ServerCommunicator::IGetAvailableWipeMethods(void) const
		{
			std::vector< std::pair<std::wstring, wipe_method_id> > available_wipe_methods;
			available_wipe_methods.push_back(std::make_pair(resources::load_string(IDS_WIPE_METHOD_A, core_dll.get_module_handle()), ToWipeMethodID(ordinary_remove)));
			available_wipe_methods.push_back(std::make_pair(resources::load_string(IDS_WIPE_METHOD_B, core_dll.get_module_handle()), ToWipeMethodID(simple_overwrite)));
			return available_wipe_methods;
		}

		wipe_method_id ServerCommunicator::IGetWipeMethod(void) const
		{
			return ToWipeMethodID(QueryWiperType(m_registry.get()));
		}

		void ServerCommunicator::ISetWipeMethod(const wipe_method_id value)
		{
			const wiper_t algorithm = ToWiperType(value);
			m_wiper.reset(QueryWiper(algorithm, m_filesystem.get()).release());
			SaveWiperType(m_registry.get(), algorithm);
		}

		std::wstring ServerCommunicator::IGetKeyStoragePath(void) const
		{
			return m_core->GetKeyStoragePath();
		}

		// FUTURE: KAA: consider use SetWorkingDirectory / _wchdir.
		void ServerCommunicator::ISetKeyStoragePath(const std::wstring& new_key_storage_path)
		{
			const std::wstring previous_key_storage_path(m_core->GetKeyStoragePath());

			if(previous_key_storage_path != new_key_storage_path)
			{
				try
				{
					m_filesystem->create_directory(new_key_storage_path);
				}
				catch(const KAA::system_failure& error)
				{
					if(EEXIST != error)
						throw;
				}

				m_core->SetKeyStoragePath(new_key_storage_path);
				SaveKeyStoragePath(m_registry.get(), new_key_storage_path);

				try
				{
					m_filesystem->remove_directory(previous_key_storage_path);
				}
				catch(const KAA::system_failure& error)
				{
					if(ENOENT != error || ENOTEMPTY != error) // FUTURE: KAA: introduce replace key storage feature.
						throw;
					//{
					//const std::wstring message(L"Unable to remove " + previous_key_storage_path + L".\nSystem message: " + error.format_message());
					//throw KAA::FileSecurity::UserReport(message, KAA::FileSecurity::UserReport::warning);
					//}
				}
			}
		}

		CommunicatorProgressHandler* ServerCommunicator::ISetProgressHandler(CommunicatorProgressHandler* handler)
		{
			CommunicatorProgressHandler* previous = server_progress;
			server_progress = handler;

			core_progress->SetProgressHandler(server_progress);
			wiper_progress->SetProgressHandler(server_progress);

			m_core->SetProgressHandler(core_progress.get());
			m_wiper->set_progress_handler(wiper_progress.get());

			return previous;
		}

		std::wstring ServerCommunicator::CreateFileBackup(const std::wstring& path)
		{
			const std::wstring working_directory(KAA::filesystem::split_directory(path));
			const std::wstring backup_file_path(working_directory + m_filesystem->get_temp_filename());
			CopyFile(path, backup_file_path); // FUTURE: KAA: remove incomplete file.
			return backup_file_path;
		}

		void ServerCommunicator::CopyFile(const std::wstring& source_path, const std::wstring& destination_path)
		{
			const KAA::filesystem::driver::mode sequential_read_only(false, true);
			const KAA::filesystem::driver::share exclusive_access(false, false);
			const std::auto_ptr<KAA::filesystem::file> source(m_filesystem->open_file(source_path, sequential_read_only, exclusive_access));

			const KAA::filesystem::driver::create_mode persistent_not_exists;
			const KAA::filesystem::driver::mode sequential_write_only(true, false);
			const KAA::filesystem::driver::permission allow_read_write;
			const std::auto_ptr<KAA::filesystem::file> destination(m_filesystem->create_file(destination_path, persistent_not_exists, sequential_write_only, exclusive_access, allow_read_write));

			{
				const size_t chunk_size = (64 * 1024) - 1;
				std::vector<uint8_t> buffer(chunk_size);

				{
					const _fsize_t source_file_size = source->get_size();

					size_t bytes_read = 0;
					size_t bytes_written = 0;
					size_t total_bytes_written = 0;

					do
					{
						bytes_read = source->read(chunk_size, &buffer[0]);
						bytes_written = destination->write(&buffer[0], bytes_read);
						total_bytes_written += bytes_written;
						PortionProcessed(total_bytes_written, source_file_size);

						if(bytes_read != bytes_written)
						{
							throw std::runtime_error(__FUNCTION__); // DEFECT: KAA: correct?
						}
					} while(bytes_read != 0);
				}
			}

			destination->commit();
		}

		progress_state ServerCommunicator::OperationStarted(const std::wstring& name)
		{
			if(nullptr != server_progress)
				return server_progress->OperationStarted(name);
			return progress_quiet;
		}

		progress_state ServerCommunicator::PortionProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			if(nullptr != server_progress)
				return server_progress->PortionProcessed(total_processed, total_size);
			return progress_quiet;
		}

		progress_state ServerCommunicator::OverallProgress(const uint64_t total_processed, const uint64_t total_size)
		{
			if(nullptr != server_progress)
				return server_progress->OverallProgress(total_processed, total_size);
			return progress_quiet;
		}
	}
}
