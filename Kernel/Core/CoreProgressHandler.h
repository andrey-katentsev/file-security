// Feb 19, 2014

#pragma once

#include <string>
#include <cstdint>

#include "KAA/include/progress_state.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CoreProgressHandler
		{
		public:
			progress_state_t ProcessingStarted(const std::string& name, uint64_t size);
			progress_state_t ChunkProcessed(uint64_t overall_processed);

		private:
			virtual progress_state_t IProcessingStarted(const std::string& name, uint64_t size) = 0;
			virtual progress_state_t IChunkProcessed(uint64_t overall_processed) = 0;
		};
	}
}
