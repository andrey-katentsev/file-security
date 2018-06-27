//
// File: Communicator.cpp
// Description: software services interface
// Created: January 27, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2014
//

#include "Communicator.h"

namespace KAA
{
	namespace FileSecurity
	{
		Communicator::~Communicator()
		{}
		
		void Communicator::EncryptFile(const std::wstring& file_to_encrypt_path)
		{
			return IEncryptFile(file_to_encrypt_path);
		}

		void Communicator::DecryptFile(const std::wstring& file_to_decrypt_path)
		{
			return IDecryptFile(file_to_decrypt_path);
		}

		bool Communicator::IsFileEncrypted(const std::wstring& file_path) const
		{
			return IIsFileEncrypted(file_path);
		}

		std::vector< std::pair<std::wstring, core_id> > Communicator::GetAvailableCiphers(void) const
		{
			return IGetAvailableCiphers();
		}
		
		core_id Communicator::GetCipher(void) const
		{
			return IGetCipher();
		}

		void Communicator::SetCipher(const core_id cipher)
		{
			return ISetCipher(cipher);
		}

		std::vector< std::pair<std::wstring, wipe_method_id> > Communicator::GetAvailableWipeMethods(void) const
		{
			return IGetAvailableWipeMethods();
		}

		wipe_method_id Communicator::GetWipeMethod(void) const
		{
			return IGetWipeMethod();
		}

		void Communicator::SetWipeMethod(const wipe_method_id algorithm)
		{
			return ISetWipeMethod(algorithm);
		}

		filesystem::path::directory Communicator::GetKeyStoragePath(void) const
		{
			return IGetKeyStoragePath();
		}
		
		void Communicator::SetKeyStoragePath(const filesystem::path::directory& path)
		{
			return ISetKeyStoragePath(path);
		}

		CommunicatorProgressHandler* Communicator::SetProgressHandler(CommunicatorProgressHandler* handler)
		{
			return ISetProgressHandler(handler);
		}
	}
}
