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
		std::auto_ptr<Communicator> GetClassObject(void)
		{
			std::auto_ptr<filesystem::driver> filesystem(new filesystem::crt_file_system);
			return std::auto_ptr<Communicator>(new ServerCommunicator(filesystem));
		}
	}
}
