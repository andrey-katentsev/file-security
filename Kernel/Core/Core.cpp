//
// File: Core.cpp
// Description: software service provider interface
// Created: January 15, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2014
//

#include "Core.h"

namespace KAA
{
	namespace FileSecurity
	{
		filesystem::path::directory Core::GetKeyStoragePath(void) const
		{
			return IGetKeyStoragePath();
		}

		void Core::SetKeyStoragePath(filesystem::path::directory path)
		{
			return ISetKeyStoragePath(std::move(path));
		}

		void Core::EncryptFile(const filesystem::path::file& path)
		{
			return IEncryptFile(path);
		}

		void Core::DecryptFile(const filesystem::path::file& path)
		{
			return IDecryptFile(path);
		}

		bool Core::IsFileEncrypted(const filesystem::path::file& path) const
		{
			return IIsFileEncrypted(path);
		}

		std::shared_ptr<CoreProgressHandler> Core::SetProgressHandler(std::shared_ptr<CoreProgressHandler> handler)
		{
			return ISetProgressHandler(handler);
		}
	}
}
