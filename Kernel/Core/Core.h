//
// File: Core.h
// Description: software service provider interface
// Created: January 15, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2014
//

#pragma once

#include <memory>

#include "KAA/include/filesystem/path.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CoreProgressHandler;

		class Core
		{
		public:
			virtual ~Core() = default;

			filesystem::path::directory GetKeyStoragePath(void) const;
			void SetKeyStoragePath(const filesystem::path::directory&);

			void EncryptFile(const filesystem::path::file& path);
			void DecryptFile(const filesystem::path::file& path);

			bool IsFileEncrypted(const filesystem::path::file& path) const;

			std::shared_ptr<CoreProgressHandler> SetProgressHandler(std::shared_ptr<CoreProgressHandler>);

		private:
			virtual filesystem::path::directory IGetKeyStoragePath(void) const = 0;
			virtual void ISetKeyStoragePath(const filesystem::path::directory&) = 0;

			virtual void IEncryptFile(const filesystem::path::file&) = 0;
			virtual void IDecryptFile(const filesystem::path::file&) = 0;

			virtual bool IIsFileEncrypted(const filesystem::path::file&) const = 0;

			virtual std::shared_ptr<CoreProgressHandler> ISetProgressHandler(std::shared_ptr<CoreProgressHandler>) = 0;
		};
	}
}
