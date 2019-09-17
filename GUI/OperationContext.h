// 09/02/2014

#pragma once

#include <functional>
#include <string>

#include "../Common/Communicator.h"

namespace KAA
{
	namespace FileSecurity
	{
		struct OperationContext
		{
			std::function<void(void)> run;
			unsigned operation_name_id;
			unsigned success_message_id;
			unsigned failure_message_id;
		};
	}
}
