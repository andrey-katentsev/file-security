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

		//std::auto_ptr<FileCipher> CreateFileCipher(cipher_t, filesystem::driver*, progress_routine_t, void*);
		std::auto_ptr<FileCipher> CreateFileCipher(cipher_t, filesystem::driver*);
	}
}
