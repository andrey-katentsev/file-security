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
			progress_state_t OperationStarted(const std::string& name, uint64_t size);
			// TODO: KAA: replace overall_processed with chunk_processed.
			progress_state_t OperationProgress(uint64_t overall_processed);

		private:
			virtual progress_state_t IOperationStarted(const std::string& name, uint64_t size) = 0;
			virtual progress_state_t IOperationProgress(uint64_t overall_processed) = 0;
		};
	}
}
