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
		std::unique_ptr<FileCipher> CreateFileCipher(const cipher_t type, std::shared_ptr<filesystem::driver> filesystem)
		{
			switch(type)
			{
			case gamma_cipher:
				return std::make_unique<GammaFileCipher>(std::move(filesystem));
			default:
					constexpr auto source = __FUNCTION__;
					constexpr auto description = "cannot create file cipher class instance: specified type is not supported";
					constexpr auto reason = operation_failure::status_code_t::invalid_argument;
					constexpr auto severity = operation_failure::severity_t::error;
					throw operation_failure(source, description, reason, severity);
			}
		}
	}
}
