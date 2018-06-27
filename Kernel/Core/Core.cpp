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
		Core::~Core()
		{}

		filesystem::path::directory Core::GetKeyStoragePath(void) const
		{
			return IGetKeyStoragePath();
		}

		void Core::SetKeyStoragePath(const filesystem::path::directory& path)
		{
			return ISetKeyStoragePath(path);
		}

		void Core::EncryptFile(const std::wstring& path)
		{
			return IEncryptFile(path);
		}

		void Core::DecryptFile(const std::wstring& path)
		{
			return IDecryptFile(path);
		}

		bool Core::IsFileEncrypted(const std::wstring& path) const
		{
			return IIsFileEncrypted(path);
		}

		CoreProgressHandler* Core::SetProgressHandler(CoreProgressHandler* handler)
		{
			return ISetProgressHandler(handler);
		}
	}
}
