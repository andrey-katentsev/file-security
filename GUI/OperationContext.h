// 09/02/2014

#pragma once

#include <string>

#include "../Common/Communicator.h"

namespace KAA
{
	namespace FileSecurity
	{
		struct OperationContext
		{
			void (*m_requested_operation)(void*);
			void* m_context;
			unsigned operation_name_id;
			unsigned success_message_id;
			unsigned failure_message_id;
			//const std::wstring* m_parameter;
			//Communicator* m_communicator;
			//void (Communicator::*m_operation)(const std::wstring&);
		};
	}
}
