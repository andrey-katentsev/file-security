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

namespace KAA
{
	namespace FileSecurity
	{
		class CoreProgressHandler;

		class Core
		{
		public:
			virtual ~Core();

			std::wstring GetKeyStoragePath(void) const;
			void SetKeyStoragePath(const std::wstring& directory_path);

			void EncryptFile(const std::wstring& path);
			void DecryptFile(const std::wstring& path);

			bool IsFileEncrypted(const std::wstring& path) const;

			CoreProgressHandler* SetProgressHandler(CoreProgressHandler*);

		private:
			virtual std::wstring IGetKeyStoragePath(void) const = 0;
			virtual void ISetKeyStoragePath(const std::wstring& directory_path) = 0;

			virtual void IEncryptFile(const std::wstring& path) = 0;
			virtual void IDecryptFile(const std::wstring& path) = 0;

			virtual bool IIsFileEncrypted(const std::wstring& path) const = 0;

			virtual CoreProgressHandler* ISetProgressHandler(CoreProgressHandler*) = 0;
		};
	}
}
