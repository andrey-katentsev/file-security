#include "FileCipherFactory.h"

#include <stdexcept>

#include "KAA/include/exception/operation_failure.h"

#include "GammaFileCipher.h"

namespace KAA
{
	namespace filesystem
	{
		class driver;
	}

	namespace FileSecurity
	{
		std::auto_ptr<FileCipher> CreateFileCipher(const cipher_t type, const std::shared_ptr<filesystem::driver> filesystem)
		{
			switch(type)
			{
			case gamma_cipher:
				return std::auto_ptr<FileCipher>(new GammaFileCipher(filesystem));
			default:
				{
					constexpr auto source { __FUNCTIONW__ };
					constexpr auto description { L"cannot create file cipher class instance: specified type is not supported" };
					constexpr auto reason = operation_failure::R_INVALID_ARGUMENT;
					constexpr auto severity = operation_failure::S_ERROR;
					throw operation_failure(source, description, reason, severity);
				}
			}
		}
	}
}
