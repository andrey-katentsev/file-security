//
// File: Core.h
// Description: software service provider interface
// Created: January 15, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2014
//

#pragma once

#include <string>

#include "KAA/include/filesystem/path.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CoreProgressHandler;

		class Core
		{
		public:
			virtual ~Core();

			filesystem::path::directory GetKeyStoragePath(void) const;
			void SetKeyStoragePath(const filesystem::path::directory&);

			void EncryptFile(const std::wstring& path);
			void DecryptFile(const std::wstring& path);

			bool IsFileEncrypted(const std::wstring& path) const;

			CoreProgressHandler* SetProgressHandler(CoreProgressHandler*);

		private:
			virtual filesystem::path::directory IGetKeyStoragePath(void) const = 0;
			virtual void ISetKeyStoragePath(const filesystem::path::directory&) = 0;

			virtual void IEncryptFile(const std::wstring& path) = 0;
			virtual void IDecryptFile(const std::wstring& path) = 0;

			virtual bool IIsFileEncrypted(const std::wstring& path) const = 0;

			virtual CoreProgressHandler* ISetProgressHandler(CoreProgressHandler*) = 0;
		};
	}
}
