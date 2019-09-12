// 28/01/2014

#include "ServerCommunicator.h"

#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <cerrno>

#include "KAA/include/load_string.h"
#include "KAA/include/random.h"
#include "KAA/include/registry_key.h"
#include "KAA/include/windows_registry.h"
#include "KAA/include/dll/module_context.h"
#include "KAA/include/exception/windows_api_failure.h"
#undef EncryptFile
#undef DecryptFile
#undef CopyFile
#undef max

#include "KAA/include/exception/system_failure.h"
#include "KAA/include/filesystem/driver.h"
#include "KAA/include/filesystem/filesystem.h"
#include "KAA/include/filesystem/ordinary_file_remover.h"
#include "KAA/include/filesystem/simple_overwrite_wiper.h"
#include "KAA/include/filesystem/wiper.h"

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

extern KAA::dll::module_context core_dll;

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
	KAA::FileSecurity::wiper_t QueryWiperType(KAA::system::registry& registry)
	try
	{
		const KAA::system::registry::key_access query_value = { false, false, false, false, true, false };
		const auto software_root = registry.open_key(KAA::system::registry::current_user, registry_software_sub_key, query_value);
		const DWORD value = software_root->query_dword_value(registry_wipe_algorithm_value_name);
		return ToWiperType(static_cast<KAA::FileSecurity::wipe_method_id>(value));
	}
	catch(const KAA::windows_api_failure& error)
	{
		if(ERROR_FILE_NOT_FOUND == error)
		{
			const KAA::FileSecurity::wiper_t default_wipe_algorithm = KAA::FileSecurity::ordinary_remove;
			const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
			const auto software_root = registry.create_key(KAA::system::registry::current_user, registry_software_sub_key, KAA::system::registry::persistent, set_value);
			software_root->set_dword_value(registry_wipe_algorithm_value_name, ToWipeMethodID(default_wipe_algorithm));
			return default_wipe_algorithm;
		}
		throw;
	}

	// FUTURE: KAA: DRY violation.
	void SaveWiperType(KAA::system::registry& registry, const KAA::FileSecurity::wiper_t wipe_algorithm)
	{
		const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
		const auto software_root = registry.open_key(KAA::system::registry::current_user, registry_software_sub_key, set_value);
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

	KAA::FileSecurity::core_t QueryCoreType(KAA::system::registry& registry)
	try
	{
		const KAA::system::registry::key_access query_value = { false, false, false, false, true, false };
		const auto software_root = registry.open_key(KAA::system::registry::current_user, registry_software_sub_key, query_value);
		const DWORD value = software_root->query_dword_value(registry_core_value_name);
		return ToCoreType(static_cast<KAA::FileSecurity::core_id>(value));
	}
	catch(const KAA::windows_api_failure& error)
	{
		if(ERROR_FILE_NOT_FOUND == error)
		{
			const KAA::FileSecurity::core_t default_core = KAA::FileSecurity::absolute_security; // FUTURE: KAA: introduce and change to strong security.
			const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
			const auto software_root = registry.create_key(KAA::system::registry::current_user, registry_software_sub_key, KAA::system::registry::persistent, set_value);
			software_root->set_dword_value(registry_core_value_name, ToCoreID(default_core));
			return default_core;
		}
		throw;
	}

	void SaveCoreType(KAA::system::registry& registry, const KAA::FileSecurity::core_t engine)
	{
		const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
		const auto software_root = registry.open_key(KAA::system::registry::current_user, registry_software_sub_key, set_value);
		return software_root->set_dword_value(registry_core_value_name, ToCoreID(engine));
	}

	KAA::filesystem::path::directory QueryKeyStoragePath(KAA::system::registry& registry)
	try
	{
		const KAA::system::registry::key_access query_value = { false, false, false, false, true, false };
		const auto software_root = registry.open_key(KAA::system::registry::current_user, registry_software_sub_key, query_value);
		return KAA::filesystem::path::directory { software_root->query_string_value(registry_key_storage_path_value_name) };
	}
	catch(const KAA::windows_api_failure& error)
	{
		if(ERROR_FILE_NOT_FOUND == error)
		{
			const KAA::filesystem::path::directory default_key_storage_path { LR"(.\keys)" };
			const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
			const auto software_root = registry.create_key(KAA::system::registry::current_user, registry_software_sub_key, KAA::system::registry::persistent, set_value);
			software_root->set_string_value(registry_key_storage_path_value_name, default_key_storage_path.to_wstring());
			return default_key_storage_path;
		}
		throw;
	}

	void SaveKeyStoragePath(KAA::system::registry& registry, const KAA::filesystem::path::directory& path)
	{
		const KAA::system::registry::key_access set_value = { false, false, false, false, false, true };
		const auto software_root = registry.open_key(KAA::system::registry::current_user, registry_software_sub_key, set_value);
		return software_root->set_string_value(registry_key_storage_path_value_name, path.to_wstring());
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

	std::auto_ptr<KAA::filesystem::wiper> QueryWiper(const KAA::FileSecurity::wiper_t interface_identifier, const std::shared_ptr<KAA::filesystem::driver> filesystem)
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

	std::auto_ptr<KAA::FileSecurity::Core> QueryCore(const KAA::FileSecurity::core_t interface_id, std::shared_ptr<KAA::filesystem::driver> filesystem, KAA::filesystem::path::directory key_storage_path)
	{
		switch(interface_id)
		{
		case KAA::FileSecurity::strong_security:
			throw std::invalid_argument(__FUNCTION__);
		case KAA::FileSecurity::absolute_security:
			return std::auto_ptr<KAA::FileSecurity::Core>(new KAA::FileSecurity::AbsoluteSecurityCore(filesystem, std::move(key_storage_path)));
		default:
			throw std::invalid_argument(__FUNCTION__);
		}
	}

	void RemoveFileBackup(KAA::filesystem::driver* const filesystem, const std::wstring& path)
	{
		filesystem->remove_file(KAA::filesystem::path::file { path });
	}
}

namespace KAA
{
	namespace FileSecurity
	{
		ServerCommunicator::ServerCommunicator(const std::shared_ptr<filesystem::driver> filesystem) :
		m_registry(QueryRegistry(windows_registry)),
		m_filesystem(filesystem),
		m_wiper(QueryWiper(QueryWiperType(*m_registry), m_filesystem)),
		m_core(QueryCore(QueryCoreType(*m_registry), m_filesystem, QueryKeyStoragePath(*m_registry))),
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

		void ServerCommunicator::IEncryptFile(const filesystem::path::file& path)
		{
			const auto file_size = get_file_size(*m_filesystem.get(), path);
			const size_t overall_size = 3*file_size;

			OperationStarted(resources::load_string(IDS_CREATING_BACKUP, core_dll.get_module_handle()));
			const auto backup = BackupFile(path);

			OperationStarted(resources::load_string(IDS_ENCRYPTING_FILE, core_dll.get_module_handle()));
			m_core->EncryptFile(path);

			OperationStarted(resources::load_string(IDS_WIPING_FILE, core_dll.get_module_handle()));
			m_wiper->wipe_file(backup);
		}

		void ServerCommunicator::IDecryptFile(const filesystem::path::file& path)
		{
			const auto file_size = get_file_size(*m_filesystem.get(), path);
			const size_t overall_size = 3*file_size;

			OperationStarted(resources::load_string(IDS_CREATING_BACKUP, core_dll.get_module_handle()));
			const auto backup = BackupFile(path);

			OperationStarted(resources::load_string(IDS_DECRYPTING_FILE, core_dll.get_module_handle()));
			m_core->DecryptFile(path);

			OperationStarted(resources::load_string(IDS_REMOVING_BACKUP, core_dll.get_module_handle()));
			m_filesystem->remove_file(backup);
		}

		bool ServerCommunicator::IIsFileEncrypted(const filesystem::path::file& path) const
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
			return ToCoreID(QueryCoreType(*m_registry));
		}

		void ServerCommunicator::ISetCipher(const core_id value)
		{
			const core_t engine = ToCoreType(value);
			auto current_key_storage_path = m_core->GetKeyStoragePath();
			m_core.reset(QueryCore(engine, m_filesystem, std::move(current_key_storage_path)).release());
			SaveCoreType(*m_registry, engine);
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
			return ToWipeMethodID(QueryWiperType(*m_registry));
		}

		void ServerCommunicator::ISetWipeMethod(const wipe_method_id value)
		{
			const wiper_t algorithm = ToWiperType(value);
			m_wiper.reset(QueryWiper(algorithm, m_filesystem).release());
			SaveWiperType(*m_registry, algorithm);
		}

		filesystem::path::directory ServerCommunicator::IGetKeyStoragePath(void) const
		{
			return m_core->GetKeyStoragePath();
		}

		// FUTURE: KAA: consider use SetWorkingDirectory / _wchdir.
		void ServerCommunicator::ISetKeyStoragePath(const filesystem::path::directory& new_key_storage_path)
		{
			const auto previous_key_storage_path = m_core->GetKeyStoragePath();
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
				SaveKeyStoragePath(*m_registry, new_key_storage_path);

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

		std::shared_ptr<CommunicatorProgressHandler> ServerCommunicator::ISetProgressHandler(std::shared_ptr<CommunicatorProgressHandler> handler)
		{
			const auto previous = server_progress;
			server_progress = handler;

			core_progress->SetProgressHandler(server_progress);
			wiper_progress->SetProgressHandler(server_progress);

			m_core->SetProgressHandler(core_progress);
			m_wiper->set_progress_handler(wiper_progress);

			return previous;
		}

		filesystem::path::file ServerCommunicator::BackupFile(const filesystem::path::file& path)
		{
			const auto working_directory = path.get_directory();
			const auto backup_file_path = working_directory + m_filesystem->get_temp_filename();
			CopyFile(path, backup_file_path); // FUTURE: KAA: remove incomplete file.
			return backup_file_path;
		}

		void ServerCommunicator::CopyFile(const filesystem::path::file& source_path, const filesystem::path::file& destination_path)
		{
			const KAA::filesystem::driver::mode sequential_read_only(false, true);
			const KAA::filesystem::driver::share exclusive_access(false, false);
			const auto source = m_filesystem->open_file(source_path, sequential_read_only, exclusive_access);

			const KAA::filesystem::driver::create_mode persistent_not_exists;
			const KAA::filesystem::driver::mode sequential_write_only(true, false);
			const KAA::filesystem::driver::permission allow_read_write;
			const auto destination = m_filesystem->create_file(destination_path, persistent_not_exists, sequential_write_only, exclusive_access, allow_read_write);

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
