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
			progress_state OperationStarted(const std::wstring& name);
			progress_state PortionProcessed(uint64_t total_processed, uint64_t total_size);
			progress_state OverallProgress(uint64_t total_processed, uint64_t total_size);

		private:
			virtual progress_state IOperationStarted(const std::wstring& name) = 0;
			virtual progress_state IPortionProcessed(uint64_t total_processed, uint64_t total_size) = 0;
			virtual progress_state IOverallProgress(uint64_t total_processed, uint64_t total_size) = 0;
		};
	}
}
