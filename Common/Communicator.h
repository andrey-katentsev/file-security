//
// File: Communicator.h
// Description: software services interface
// Created: January 27, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2014
//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Features.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CommunicatorProgressHandler;

		// FUTURE: KAA: try to separate progress handling from classes (looks like SRP violation).
		class Communicator
		{
		public:
			virtual ~Communicator();

			void EncryptFile(const std::wstring& file_to_encrypt_path);
			void DecryptFile(const std::wstring& file_to_decrypt_path);

			bool IsFileEncrypted(const std::wstring& file_path) const;

			std::vector< std::pair<std::wstring, core_id> > GetAvailableCiphers(void) const;
			core_id GetCipher(void) const;
			void SetCipher(core_id);

			std::vector< std::pair<std::wstring, wipe_method_id> > GetAvailableWipeMethods(void) const;
			wipe_method_id GetWipeMethod(void) const;
			void SetWipeMethod(wipe_method_id);

			std::wstring GetKeyStoragePath(void) const;
			void SetKeyStoragePath(const std::wstring& key_storage_path);

			CommunicatorProgressHandler* SetProgressHandler(CommunicatorProgressHandler*);

		private:
			virtual void IEncryptFile(const std::wstring& file_to_encrypt_path) = 0;
			virtual void IDecryptFile(const std::wstring& file_to_decrypt_path) = 0;

			virtual bool IIsFileEncrypted(const std::wstring& file_path) const = 0;

			virtual std::vector< std::pair<std::wstring, core_id> > IGetAvailableCiphers(void) const = 0;
			virtual core_id IGetCipher(void) const = 0;
			virtual void ISetCipher(core_id) = 0;

			virtual std::vector< std::pair<std::wstring, wipe_method_id> > IGetAvailableWipeMethods(void) const = 0;
			virtual wipe_method_id IGetWipeMethod(void) const = 0;
			virtual void ISetWipeMethod(wipe_method_id) = 0;

			virtual std::wstring IGetKeyStoragePath(void) const = 0;
			virtual void ISetKeyStoragePath(const std::wstring& key_storage_path) = 0;

			virtual CommunicatorProgressHandler* ISetProgressHandler(CommunicatorProgressHandler*) = 0;
		};
	}
}
