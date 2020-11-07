// 8/2/14

#pragma once

#include <memory>

namespace KAA
{
	namespace filesystem
	{
		class driver;
		class wiper;
	}

	namespace FileSecurity
	{
		enum class wiper_t
		{
			ordinary_remove,
			simple_overwrite
		};

		std::unique_ptr<filesystem::wiper> QueryWiper(wiper_t, std::shared_ptr<filesystem::driver>);
	}
}
