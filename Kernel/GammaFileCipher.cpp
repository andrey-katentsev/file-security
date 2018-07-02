#include "GammaFileCipher.h"

#include <stdexcept>
#include <string>
#include <vector>

//#include <cstdlib>

#include "KAA/include/convert.h"
#include "KAA/include/cryptography.h"
#undef EncryptFile
#undef DecryptFile

#include "KAA/include/keywords.h"
#include "KAA/include/stdtypes.h"
//#include "C:/SDK/KAA/include/exception/system_failure.h"
#include "KAA/include/filesystem/driver.h"
#include "KAA/include/filesystem/path.h"
//#include "C:/SDK/KAA/include/filesystem/split_path.h"
//#include "C:/SDK/KAA/include/RAII/invalid_parameter_handler.h"
#include "KAA/include/exception/operation_failure.h"

#include "FileProgressHandler.h"

namespace
{
	//void allow_execution(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t) { return; }

	/*std::wstring get_temp_filename(void)
	{
		long random = 0;
		KAA::cryptography::generate(sizeof(random), &random); // FUTURE: KAA: consider use _wmktemp_s instead.
		const std::wstring temp_file_extension(L".tmp");
		return KAA::convert::to_wstring(random, 16) + temp_file_extension;
	}*/

	//void process_chunk(KAA::filesystem::file* const master, KAA::filesystem::file* const key, /*void* master_buffer, void* key_buffer, */const size_t chunk_size)
	/*{
		std::vector<uint8_t> master_buffer(chunk_size);
		std::vector<uint8_t> key_buffer(chunk_size);

		//master->read(chunk_size, master_buffer);
		//key->read(chunk_size, key_buffer);
		//KAA::cryptography::gamma(master_buffer, key_buffer, master_buffer, chunk_size);
		//master->seek(-chunk_size, KAA::filesystem::file::current);
		//master->write(master_buffer, chunk_size);
		master->read(chunk_size, &master_buffer[0]);
		key->read(chunk_size, &key_buffer[0]);
		KAA::cryptography::gamma(&master_buffer[0], &key_buffer[0], &master_buffer[0], chunk_size);
		master->seek(-chunk_size, KAA::filesystem::file::current);
		master->write(&master_buffer[0], chunk_size);
	}*/
}

	// FIX: KAA: take look at exception raising.
	/*void Encrypt(const std::wstring& file, const std::wstring& key)
	{
		std::wstring temp_file(L"./");
		temp_file += *GetTempFileName();
		{
			KAA::RAII::handle<int, int> file_handle(OpenFile(file), _close);
			KAA::RAII::handle<int, int> key_handle(OpenFile(key), _close);
			KAA::RAII::handle<int, int> temp_handle(CreateFile(temp_file), _close);
			//long file_size = KAA::File::GetSize(file_handle);
			long file_size = _filelength(file_handle);
			vector<byte> file_buffer(file_size, 0), key_buffer(file_size, 0);
			long bytes_read = _read(file_handle, &file_buffer[0], file_size);
			bytes_read = _read(key_handle, &key_buffer[0], file_size);
			cryptography::gamma(&file_buffer[0], &key_buffer[0], &file_buffer[0], file_size);
			if(file_size != _write(temp_handle, &file_buffer[0], file_size))
				throw KAA::Error::CWin32Exception(ERROR_BAD_LENGTH, 0, KAA::Error::S_ERROR, true, L"Overall encrypted file size does not match specified file size.", L"KAA::Security::Encrypt");
		}
		if(0 != _wremove(file.c_str()))
			throw KAA::Error::CWin32Exception(ERROR_ACCESS_DENIED, 0, KAA::Error::S_ERROR, true, L"Unable to remove specified file.", L"KAA::Security::Encrypt");
		if(0 != _wrename(temp_file.c_str(), file.c_str()))
			throw KAA::Error::CWin32Exception(ERROR_ACCESS_DENIED, 0, KAA::Error::S_ERROR, true, L"Unable to rename encrypted file.", L"KAA::Security::Encrypt");
	}*/

namespace KAA
{
	namespace FileSecurity
	{
		//GammaFileCipher::GammaFileCipher() : m_filesystem(create_file_system(crt_api))
		//GammaFileCipher::GammaFileCipher(filesystem::driver* filesystem, progress_routine_t progress_notify, void* const progress_context) :
		//GammaFileCipher::GammaFileCipher(filesystem::driver* filesystem, const cipher_progress_callback* handler) :
		//FileCipher(progress_notify, progress_context),
		//FileCipher(handler),
		GammaFileCipher::GammaFileCipher(const std::shared_ptr<filesystem::driver> filesystem) :
		m_filesystem(filesystem),
		cipher_progress(nullptr)
		{
			if(nullptr == filesystem)
			{
				const std::wstring source { __FUNCTIONW__ };
				const std::wstring description { L"Unable to create a gamma file cipher class instance." };
				const KAA::operation_failure::status_code_t reason = KAA::operation_failure::R_INVALID_ARGUMENT;
				const KAA::operation_failure::severity_t severity = KAA::operation_failure::S_ERROR;
				throw operation_failure(source, description, reason, severity);
			}
		}

		void GammaFileCipher::IEncryptFile(const filesystem::path::file& path, const filesystem::path::file& key_path)
		{
			const filesystem::driver::mode random_read_write(true, true, true, true);
			const filesystem::driver::share exclusive_access(false, false);

			const std::auto_ptr<filesystem::file> master(m_filesystem->open_file(path.to_wstring(), random_read_write, exclusive_access));

			const filesystem::driver::mode sequential_read_only(false);

			const std::auto_ptr<filesystem::file> key(m_filesystem->open_file(key_path.to_wstring(), sequential_read_only, exclusive_access));

			const _fsize_t master_data_size = master->get_size();

			const size_t chunk_size = (64 * 1024) - 1;
			//const unsigned chunks_total = master_data_size / chunk_size;

			std::vector<uint8_t> master_buffer(chunk_size);
			std::vector<uint8_t> key_buffer(chunk_size);

			size_t bytes_read = 0;
			size_t total_bytes_written = 0;

			{
				bool stop = false;
				bool chunk_processed = false;
				progress_state progress = progress_continue;

				do 
				{
					{
						bytes_read = master->read(chunk_size, &master_buffer[0]);
						key->read(bytes_read, &key_buffer[0]);
						cryptography::gamma(&master_buffer[0], &key_buffer[0], &master_buffer[0], bytes_read);
						master->seek(-static_cast<_off_t>(bytes_read), filesystem::file::current);
						total_bytes_written += master->write(&master_buffer[0], bytes_read);
					}
					{
						//if(nullptr != progress_notifier)
						//(*progress_notifier)(master_data_size, bytes_written, progress_context);
						chunk_processed = ( 0 != bytes_read );
						if(chunk_processed && ( progress_quiet != progress ))
							progress = ChunkProcessed(total_bytes_written, master_data_size);
						stop = ( !chunk_processed ) || ( progress_cancel == progress ) || ( progress_stop == progress );
					}
				} while(!stop);
				//while(0 != bytes_read);
			}

			//for(unsigned chunk = 0; chunk < chunks_total; ++chunk)
			//{
				/*master->read(chunk_size, &master_buffer[0]);
				key->read(chunk_size, &key_buffer[0]);
				cryptography::gamma(&master_buffer[0], &key_buffer[0], &master_buffer[0], chunk_size);
				master->seek(-chunk_size, filesystem::file::current);
				master->write(&master_buffer[0], chunk_size);*/
				//process_chunk(master.get(), key.get(), /*&master_buffer[0], &key_buffer[0], */chunk_size);
			//}

			/*master->read(last_chunk_size, &master_buffer[0]);
			key->read(last_chunk_size, &key_buffer[0]);
			cryptography::gamma(&master_buffer[0], &key_buffer[0], &master_buffer[0], last_chunk_size);
			master->seek(-last_chunk_size, filesystem::file::current);
			master->write(&master_buffer[0], last_chunk_size);*/
			//const size_t last_chunk_size = master_data_size % chunk_size;
			//process_chunk(master.get(), key.get(), /*&master_buffer[0], &key_buffer[0], */last_chunk_size);
		}

		void GammaFileCipher::IDecryptFile(const filesystem::path::file& path, const filesystem::path::file& key)
		{
			return EncryptFile(path, key);
		}

		FileProgressHandler* GammaFileCipher::ISetProgressCallback(FileProgressHandler* handler)
		{
			FileProgressHandler* previous = cipher_progress;
			cipher_progress = handler;
			return previous;
		}

		progress_state GammaFileCipher::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			if(nullptr != cipher_progress)
				return cipher_progress->ChunkProcessed(total_bytes_processed, total_file_size);
			return progress_quiet;
		}
	}
}
