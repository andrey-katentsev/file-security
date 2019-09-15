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
			progress_state OperationStarted(const std::wstring& name);
			progress_state ChunkProcessed(uint64_t total_processed, uint64_t total_size);
			progress_state OverallProgress(uint64_t total_processed, uint64_t total_size);

		private:
			virtual progress_state IOperationStarted(const std::wstring& name) = 0;
			virtual progress_state IChunkProcessed(uint64_t total_processed, uint64_t total_size) = 0;
			virtual progress_state IOverallProgress(uint64_t total_processed, uint64_t total_size) = 0;
		};
	}
}
