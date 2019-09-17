#pragma once

#include <memory>
#include "FileCipher.h"

namespace KAA
{
	namespace filesystem
	{
		class driver;
	}

	namespace FileSecurity
	{
		class FileCipher;
		enum cipher_t
		{
			gamma_cipher,
		};

		std::unique_ptr<FileCipher> CreateFileCipher(cipher_t, std::shared_ptr<filesystem::driver>);
	}
}
