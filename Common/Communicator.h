//
// File: Communicator.h
// Description: software services interface
// Created: January 27, 2014
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2014
//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "KAA/include/filesystem/path.h"

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
			virtual ~Communicator() = default;

			void EncryptFile(const filesystem::path::file&);
			void DecryptFile(const filesystem::path::file&);

			bool IsFileEncrypted(const filesystem::path::file&) const;

			std::vector< std::pair<std::wstring, core_id> > GetAvailableCiphers(void) const;
			core_id GetCipher(void) const;
			void SetCipher(core_id);

			std::vector< std::pair<std::wstring, wipe_method_id> > GetAvailableWipeMethods(void) const;
			wipe_method_id GetWipeMethod(void) const;
			void SetWipeMethod(wipe_method_id);

			filesystem::path::directory GetKeyStoragePath(void) const;
			void SetKeyStoragePath(const filesystem::path::directory&);

			std::shared_ptr<CommunicatorProgressHandler> SetProgressHandler(std::shared_ptr<CommunicatorProgressHandler>);

		private:
			virtual void IEncryptFile(const filesystem::path::file&) = 0;
			virtual void IDecryptFile(const filesystem::path::file&) = 0;

			virtual bool IIsFileEncrypted(const filesystem::path::file&) const = 0;

			virtual std::vector< std::pair<std::wstring, core_id> > IGetAvailableCiphers(void) const = 0;
			virtual core_id IGetCipher(void) const = 0;
			virtual void ISetCipher(core_id) = 0;

			virtual std::vector< std::pair<std::wstring, wipe_method_id> > IGetAvailableWipeMethods(void) const = 0;
			virtual wipe_method_id IGetWipeMethod(void) const = 0;
			virtual void ISetWipeMethod(wipe_method_id) = 0;

			virtual filesystem::path::directory IGetKeyStoragePath(void) const = 0;
			virtual void ISetKeyStoragePath(const filesystem::path::directory&) = 0;

			virtual std::shared_ptr<CommunicatorProgressHandler> ISetProgressHandler(std::shared_ptr<CommunicatorProgressHandler>) = 0;
		};
	}
}
