#include "CoreFactory.h"

#include <stdexcept>

#include "AbsoluteSecurityCore.h"
#include "StrongSecurityCore.h"

namespace KAA
{
	namespace FileSecurity
	{
		std::unique_ptr<Core> QueryCore(const core_t interface_identifier, std::shared_ptr<filesystem::driver> filesystem, filesystem::path::directory key_storage_path)
		{
			switch (interface_identifier)
			{
			case strong_security:
				throw std::invalid_argument(__FUNCTION__);
			case absolute_security:
				return std::make_unique<AbsoluteSecurityCore>(std::move(filesystem), std::move(key_storage_path));
			default:
				throw std::invalid_argument(__FUNCTION__);
			}
		}

		/*std::auto_ptr<Core> CoreFactory::QueryInterface(const core_t interface_identifier)
		{
			switch(interface_identifier)
			{
			case absolute_security:
				return std::auto_ptr<Core>(new AbsoluteSecurityCore);
			case strong_security:
			default:
				throw std::invalid_argument(__FUNCTION__);
			}
		}*/
	}
}
