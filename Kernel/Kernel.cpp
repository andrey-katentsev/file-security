//
// File: kernel.cpp
// Description: file security processing unit
// Created: May 17, 2013
// Author: Andrey A. Katentsev
//
// Copyright © Hyperlink Software, 2013
//

#include "Kernel.h"
#include "KAA/include/filesystem/crt_file_system.h"
#include "ServerCommunicator.h"

namespace KAA
{
	namespace FileSecurity
	{
		std::unique_ptr<Communicator> GetClassObject(void)
		{
			auto filesystem = std::make_shared<filesystem::crt_file_system>();
			return std::make_unique<ServerCommunicator>(std::move(filesystem));
		}
	}
}
