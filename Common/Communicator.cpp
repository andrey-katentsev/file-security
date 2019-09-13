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
		void Communicator::EncryptFile(const filesystem::path::file& path)
		{
			return IEncryptFile(path);
		}

		void Communicator::DecryptFile(const filesystem::path::file& path)
		{
			return IDecryptFile(path);
		}

		bool Communicator::IsFileEncrypted(const filesystem::path::file& path) const
		{
			return IIsFileEncrypted(path);
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
		
		void Communicator::SetKeyStoragePath(filesystem::path::directory path)
		{
			return ISetKeyStoragePath(std::move(path));
		}

		std::shared_ptr<CommunicatorProgressHandler> Communicator::SetProgressHandler(std::shared_ptr<CommunicatorProgressHandler> handler)
		{
			return ISetProgressHandler(std::move(handler));
		}
	}
}
