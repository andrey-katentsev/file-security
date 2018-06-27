#include "AbsoluteSecurityCore.h"

#include <stdexcept>
#include <vector>

#include <cerrno>

#include "KAA/include/cryptography.h" // FUTURE: KAA: remove <windows.h>
#undef EncryptFile
#undef DecryptFile

#include "KAA/include/keywords.h"
#include "KAA/include/load_string.h"
#include "KAA/include/module_context.h"
#include "KAA/include/stdtypes.h"
#include "KAA/include/exception/operation_failure.h"
#include "KAA/include/exception/system_failure.h"
#include "KAA/include/filesystem/driver.h"
#include "KAA/include/filesystem/filesystem.h"

#include "./Core/CoreProgressHandler.h"
#include "CipherProgressDispatcher.h"

#include "FileCipher.h"
#include "FileCipherFactory.h"
#include "KeyStorage.h"
#include "KeyStorageFactory.h"

#include "resource.h"

extern KAA::module_context core_dll;

namespace
{
	void RemoveKey(KAA::filesystem::driver* filesystem, const std::wstring& key_file_path)
	{
		KAA::filesystem::driver::permission write_only(true, false);
		filesystem->set_file_permissions(key_file_path, write_only);
		filesystem->remove_file(key_file_path);
	}
}

namespace KAA
{
	namespace FileSecurity
	{
		AbsoluteSecurityCore::AbsoluteSecurityCore(filesystem::driver* filesystem, const filesystem::path::directory& key_storage_path) :
		m_filesystem(filesystem),
		//m_wiper(new ordinary_file_remover(filesystem)),
		m_cipher(CreateFileCipher(gamma_cipher, filesystem)),
		m_key_storage(CreateKeyStorage(md5_based, key_storage_path.to_wstring())),
		cipher_progress(new CipherProgressDispatcher),
		core_progress(nullptr)
		{
			if(nullptr == filesystem)
			{
				const std::wstring source { __FUNCTIONW__ };
				const std::wstring description { L"Unable to create an absolute security core class instance." };
				const KAA::operation_failure::status_code_t reason = KAA::operation_failure::R_INVALID_ARGUMENT;
				const KAA::operation_failure::severity_t severity = KAA::operation_failure::S_ERROR;
				throw operation_failure(source, description, reason, severity);
			}
		}

		AbsoluteSecurityCore::~AbsoluteSecurityCore()
		{}

		filesystem::path::directory AbsoluteSecurityCore::IGetKeyStoragePath(void) const
		{
			return m_key_storage->GetPath();
		}

		void AbsoluteSecurityCore::ISetKeyStoragePath(const filesystem::path::directory& path)
		{
			m_key_storage->SetPath(path);
		}

		void AbsoluteSecurityCore::IEncryptFile(const std::wstring& file_to_encrypt_path)
		{
			OperationStarted(resources::load_string(IDS_RETRIEVING_KEY_PATH, core_dll.get_module_handle()));
			const std::wstring key_file_path(m_key_storage->GetKeyPathForSpecifiedPath(file_to_encrypt_path));

			const _fsize_t file_to_encrypt_size = get_file_size(*m_filesystem, file_to_encrypt_path);
			const size_t overall_size = 2*file_to_encrypt_size;
			size_t total_processed = 0;

			{
				OperationStarted(resources::load_string(IDS_GENERATING_KEY, core_dll.get_module_handle()));
				const std::auto_ptr< std::vector<uint8_t> > key_data(GenerateKey(file_to_encrypt_size));
				CreateKeyFile(key_file_path, *key_data);
				total_processed += file_to_encrypt_size;
				OverallProgress(total_processed, overall_size);
			}

			{
				OperationStarted(resources::load_string(IDS_ENCRYPTING_FILE, core_dll.get_module_handle()));
				m_cipher->EncryptFile(file_to_encrypt_path, key_file_path);
				total_processed += file_to_encrypt_size;
				OverallProgress(total_processed, overall_size);
			}
		}

		void AbsoluteSecurityCore::IDecryptFile(const std::wstring& file_to_decrypt_path)
		{
			OperationStarted(resources::load_string(IDS_RETRIEVING_KEY_PATH, core_dll.get_module_handle()));
			const std::wstring key_file_path(m_key_storage->GetKeyPathForSpecifiedPath(file_to_decrypt_path));

			const _fsize_t file_to_decrypt_size = get_file_size(*m_filesystem, file_to_decrypt_path);
			const size_t overall_size = 2*file_to_decrypt_size;
			size_t total_processed = 0;

			{
				OperationStarted(resources::load_string(IDS_DECRYPTING_FILE, core_dll.get_module_handle()));
				m_cipher->DecryptFile(file_to_decrypt_path, key_file_path);
				total_processed += file_to_decrypt_size;
				OverallProgress(total_processed, overall_size);
			}

			OperationStarted(resources::load_string(IDS_REMOVING_KEY, core_dll.get_module_handle()));
			RemoveKey(m_filesystem, key_file_path);
			total_processed += file_to_decrypt_size;
			OverallProgress(total_processed, overall_size);
		}

		bool AbsoluteSecurityCore::IIsFileEncrypted(const std::wstring& path) const
		{
			const std::wstring key_file_path(m_key_storage->GetKeyPathForSpecifiedPath(path));
			try
			{
				m_filesystem->get_file_permissions(key_file_path);
				return true;
			}
			catch(const system_failure& error) // DEFECT: KAA: provide filesystem::driver with own exception class type.
			{
				if(ENOENT == error)
					return false;
				throw;
			}
		}

		CoreProgressHandler* AbsoluteSecurityCore::ISetProgressHandler(CoreProgressHandler* core_progress)
		{
			CoreProgressHandler* previous = this->core_progress;
			this->core_progress = core_progress;
			cipher_progress->SetProgressHandler(core_progress);
			m_cipher->SetProgressCallback(cipher_progress.get());
			return previous;
		}

		std::auto_ptr< std::vector<uint8_t> > AbsoluteSecurityCore::GenerateKey(const size_t bytes_to_generate)
		{
			std::auto_ptr< std::vector<uint8_t> > buffer(new std::vector<uint8_t>(bytes_to_generate, 0));

			{
				size_t chunk_size = (64 * 1024) - 1;
				unsigned chunks_total = bytes_to_generate / chunk_size;

				for(unsigned chunk = 0; chunk < chunks_total; ++chunk)
				{
					KAA::cryptography::generate(chunk_size, &(*buffer)[chunk * chunk_size]);
					ChunkProcessed((chunk + 1) * chunk_size, bytes_to_generate);
				}

				size_t last_chunk_size = bytes_to_generate % chunk_size;
				if(0 < last_chunk_size)
				{
					KAA::cryptography::generate(last_chunk_size, &(*buffer)[chunks_total * chunk_size]);
					ChunkProcessed(bytes_to_generate, bytes_to_generate);
				}
			}

			return buffer;
		}

		void AbsoluteSecurityCore::CreateKeyFile(const std::wstring& path, const std::vector<uint8_t>& data)
		{
			const KAA::filesystem::driver::create_mode persistent_not_exist(true, false, false);
			const KAA::filesystem::driver::mode sequential_write_only(true, false);
			const KAA::filesystem::driver::share exclusive_access(false, false);
			const KAA::filesystem::driver::permission read_only_attribute(false, true);
			std::auto_ptr<KAA::filesystem::file> key(m_filesystem->create_file(path, persistent_not_exist, sequential_write_only, exclusive_access, read_only_attribute));
			const size_t bytes_written = key->write(&data[0], data.size());
			if(bytes_written != data.size())
			{
				key.reset();
				m_filesystem->remove_file(path);
				throw std::runtime_error(__FUNCTION__); // FUTURE: KAA: remove incomplete file : whose responsibility?
			}
		}

		progress_state AbsoluteSecurityCore::OperationStarted(const std::wstring& name)
		{
			if(nullptr != core_progress)
				return core_progress->OperationStarted(name);
			return progress_quiet;
		}

		progress_state AbsoluteSecurityCore::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			if(nullptr != core_progress)
				return core_progress->ChunkProcessed(total_bytes_processed, total_file_size);
			return progress_quiet;
		}

		progress_state AbsoluteSecurityCore::OverallProgress(const uint64_t total_bytes_processed, const uint64_t total_size)
		{
			if(nullptr != core_progress)
				return core_progress->OverallProgress(total_bytes_processed, total_size);
			return progress_quiet;
		}
	}
}
