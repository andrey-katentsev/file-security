#include "AbsoluteSecurityCore.h"

// FIX: TODO: throw operation_failure.
#include <stdexcept>
#include <cerrno>

#include "KAA/include/load_string.h"
#include "KAA/include/unicode.h"
#include "KAA/include/cryptography/cryptography.h"
#include "KAA/include/cryptography/random.h"
#include "KAA/include/dll/module_context.h"
#include "KAA/include/exception/operation_failure.h"
#include "KAA/include/exception/system_failure.h"
#include "KAA/include/filesystem/driver.h"
#include "KAA/include/filesystem/filesystem.h"

#include "./Core/CoreProgressHandler.h"
#include "CipherProgressDispatcher.h"

// FUTURE: KAA: remove <windows.h>
#undef EncryptFile
#undef DecryptFile

#include "FileCipher.h"
#include "FileCipherFactory.h"
#include "KeyStorage.h"
#include "KeyStorageFactory.h"

#include "resource.h"

extern KAA::dll::module_context core_dll;

namespace
{
	void RemoveKeyFile(KAA::filesystem::driver& filesystem, const KAA::filesystem::path::file& path)
	{
		KAA::filesystem::driver::permission write_only(true, false);
		filesystem.set_file_permissions(path, write_only);
		filesystem.remove_file(path);
	}
}

namespace KAA
{
	using namespace unicode;
	namespace FileSecurity
	{
		AbsoluteSecurityCore::AbsoluteSecurityCore(std::shared_ptr<filesystem::driver> filesystem, filesystem::path::directory key_storage_path) :
		m_filesystem(std::move(filesystem)),
		m_cipher(CreateFileCipher(gamma_cipher, m_filesystem)),
		m_key_storage(CreateKeyStorage(key_storage_t::md5_based, m_filesystem, std::move(key_storage_path))),
		cipher_progress(new CipherProgressDispatcher),
		core_progress(nullptr)
		{
			// KAA: filesystem already verified by cipher and key storage.
		}

		AbsoluteSecurityCore::~AbsoluteSecurityCore() = default;

		filesystem::path::directory AbsoluteSecurityCore::IGetKeyStoragePath(void) const
		{
			return m_key_storage->GetPath();
		}

		void AbsoluteSecurityCore::ISetKeyStoragePath(filesystem::path::directory path)
		{
			return m_key_storage->SetPath(std::move(path));
		}

		void AbsoluteSecurityCore::IEncryptFile(const filesystem::path::file& path)
		{
			OperationStarted(to_UTF8(resources::load_string(IDS_RETRIEVING_KEY_PATH, core_dll.get_module_handle())));

			const auto file_to_encrypt_size = get_file_size(*m_filesystem, path);

			auto key_path = m_filesystem->get_temp_filename(m_key_storage->GetPath());
			{
				OperationStarted(to_UTF8(resources::load_string(IDS_GENERATING_KEY, core_dll.get_module_handle())));
				// DEFECT: KAA: what if 1 GiB size?
				const auto key_data = GenerateKey(file_to_encrypt_size);
				CreateKeyFile(key_path, key_data);
			}
			{
				OperationStarted(to_UTF8(resources::load_string(IDS_ENCRYPTING_FILE, core_dll.get_module_handle())));
				m_cipher->EncryptFile(path, key_path);
			}
			m_filesystem->rename_file(key_path, m_key_storage->GetKeyPathForSpecifiedPath(path));
		}

		void AbsoluteSecurityCore::IDecryptFile(const filesystem::path::file& path)
		{
			OperationStarted(to_UTF8(resources::load_string(IDS_RETRIEVING_KEY_PATH, core_dll.get_module_handle())));

			const auto key_path = m_key_storage->GetKeyPathForSpecifiedPath(path);
			{
				OperationStarted(to_UTF8(resources::load_string(IDS_DECRYPTING_FILE, core_dll.get_module_handle())));
				m_cipher->DecryptFile(path, key_path);
			}
			{
				OperationStarted(to_UTF8(resources::load_string(IDS_REMOVING_KEY, core_dll.get_module_handle())));
				RemoveKeyFile(*m_filesystem, key_path);
			}
		}

		bool AbsoluteSecurityCore::IIsFileEncrypted(const filesystem::path::file& path) const
		{
			const auto key_file_path = m_key_storage->GetKeyPathForSpecifiedPath(path);
			return filesystem::file_exists(*m_filesystem, key_file_path);
		}

		std::shared_ptr<CoreProgressHandler> AbsoluteSecurityCore::ISetProgressHandler(std::shared_ptr<CoreProgressHandler> handler)
		{
			core_progress.swap(handler);
			cipher_progress->SetProgressHandler(core_progress);
			m_cipher->SetProgressCallback(cipher_progress);
			return handler;
		}

		std::vector<uint8_t> AbsoluteSecurityCore::GenerateKey(const size_t bytes_to_generate)
		{
			std::vector<uint8_t> buffer(bytes_to_generate, 0U);
			{
				constexpr auto chunk_size = 64U * 1024U; // 64 KiB
				const auto chunks_total = bytes_to_generate / chunk_size;
				for(auto chunk = 0U; chunk < chunks_total; ++chunk)
				{
					KAA::cryptography::generate(chunk_size, &buffer[chunk * chunk_size]);
					const auto bytes_processed = (static_cast<uint64_t>(chunk) + 1) * chunk_size;
					ChunkProcessed(bytes_processed, bytes_to_generate);
				}

				size_t last_chunk_size = bytes_to_generate % chunk_size;
				if(0 < last_chunk_size)
				{
					KAA::cryptography::generate(last_chunk_size, &buffer[chunks_total * chunk_size]);
					ChunkProcessed(bytes_to_generate, bytes_to_generate);
				}
			}
			return buffer;
		}

		void AbsoluteSecurityCore::CreateKeyFile(const filesystem::path::file& path, const std::vector<uint8_t>& data)
		{
			const KAA::filesystem::driver::create_mode persistent_not_exist(true, false, false);
			const KAA::filesystem::driver::mode sequential_write_only(true, false);
			const KAA::filesystem::driver::share exclusive_access(false, false);
			const KAA::filesystem::driver::permission read_only_attribute(false, true);
			auto key = m_filesystem->create_file(path, persistent_not_exist, sequential_write_only, exclusive_access, read_only_attribute);
			const size_t bytes_written = key->write(&data[0], data.size());
			if(bytes_written != data.size())
			{
				key.reset();
				RemoveKeyFile(*m_filesystem, path);
				throw std::runtime_error(__FUNCTION__); // FUTURE: KAA: remove incomplete file : whose responsibility?
			}
		}

		progress_state_t AbsoluteSecurityCore::OperationStarted(const std::string& name)
		{
			if(nullptr != core_progress)
				return core_progress->ProcessingStarted(name);
			return progress_state_t::quiet;
		}

		progress_state_t AbsoluteSecurityCore::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			if(nullptr != core_progress)
				return core_progress->ChunkProcessed(total_bytes_processed, total_file_size);
			return progress_state_t::quiet;
		}
	}
}
