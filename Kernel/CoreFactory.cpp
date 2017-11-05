#include "CoreFactory.h"
/*
#include <stdexcept>

#include "AbsoluteSecurityCore.h"
#include "StrongSecurityCore.h"

namespace KAA
{
	namespace FileSecurity
	{
		std::auto_ptr<Core> CoreFactory::QueryInterface(const core_t interface_identifier)
		{
			switch(interface_identifier)
			{
			case absolute_security:
				return std::auto_ptr<Core>(new AbsoluteSecurityCore);
			case strong_security:
			default:
				throw std::invalid_argument(__FUNCTION__);
			}
		}
	}
}
*/