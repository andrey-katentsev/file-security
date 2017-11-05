#include "FileCipherFactory.h"

#include <stdexcept>

#include "SDK/KAA/include/exception/operation_failure.h"

#include "GammaFileCipher.h"

namespace KAA
{
	namespace filesystem
	{
		class driver;
	}

	namespace FileSecurity
	{
		std::auto_ptr<FileCipher> CreateFileCipher(const cipher_t type, filesystem::driver* const filesystem)
		{
			switch(type)
			{
			case gamma_cipher:
				return std::auto_ptr<FileCipher>(new GammaFileCipher(filesystem));
			default:
				{
					const std::wstring source { __FUNCTIONW__ };
					const std::wstring description { L"Unable to create a specified file cipher class instance." };
					const KAA::operation_failure::status_code_t reason = KAA::operation_failure::R_INVALID_ARGUMENT;
					const KAA::operation_failure::severity_t severity = KAA::operation_failure::S_ERROR;
					throw operation_failure(source, description, reason, severity);
				}
			}
		}
	}
}
