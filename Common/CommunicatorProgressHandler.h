// Feb 16, 2014

#pragma once

#include <string>
#include <cstdint>

#include "KAA/include/progress_state.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CommunicatorProgressHandler
		{
		public:
			progress_state_t OperationStarted(const std::string& name);
			progress_state_t PortionProcessed(uint64_t total_processed, uint64_t total_size);

		private:
			virtual progress_state_t IOperationStarted(const std::string& name) = 0;
			virtual progress_state_t IPortionProcessed(uint64_t total_processed, uint64_t total_size) = 0;
		};
	}
}
