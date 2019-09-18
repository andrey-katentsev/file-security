//
// File: RegistryFactory.cpp
// Description: system registry factory
// Created: September 17, 2019
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2019
//

#include "RegistryFactory.h"
#include <stdexcept>
#include "KAA/include/windows_registry.h"

namespace KAA
{
	namespace FileSecurity
	{
		std::unique_ptr<system::registry> QueryRegistry(const registry_t interface_identifier)
		{
			switch (interface_identifier)
			{
			case windows_registry:
				return std::make_unique<system::windows_registry>();
			default:
				throw std::invalid_argument(__FUNCTION__); // TODO: KAA: refine description.
			}
		}
	}
}
