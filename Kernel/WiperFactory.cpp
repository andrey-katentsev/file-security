//
// WiperFactory.cpp
// Description: filesystem wiper factory
// Created: September 18, 2019
// Author: Andrey A. Katentsev
//
// Copyright © Andrey A. Katentsev, 2019
//

#include "WiperFactory.h"

#include <stdexcept>
#include <limits>

#include "KAA/include/random.h"
#include "KAA/include/filesystem/ordinary_file_remover.h"
#include "KAA/include/filesystem/simple_overwrite_wiper.h"

namespace KAA
{
	namespace FileSecurity
	{
		std::unique_ptr<filesystem::wiper> QueryWiper(const wiper_t interface_identifier, std::shared_ptr<filesystem::driver> filesystem)
		{
			switch (interface_identifier)
			{
			case ordinary_remove:
				return std::make_unique<filesystem::ordinary_file_remover>(std::move(filesystem));
			case simple_overwrite:
			{
				const uint8_t aggregate = cryptography::random() % std::numeric_limits<uint8_t>::max();
				return std::make_unique<filesystem::simple_owerwrite_wiper>(std::move(filesystem), aggregate);
			}
			default:
				throw std::invalid_argument(__FUNCTION__);
			}
		}
	}
}
