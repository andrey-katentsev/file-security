// Feb 16, 2014

#pragma once

#include <memory>

namespace KAA
{
	namespace system
	{
		class registry;
	}

	namespace FileSecurity
	{
		enum registry_t
		{
			windows_registry
		};

		// FUTURE: KAA: consider to move this to SDK.
		std::unique_ptr<system::registry> QueryRegistry(registry_t);
	}
}
