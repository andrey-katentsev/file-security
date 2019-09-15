#pragma once

#include <cstdint>

#include "KAA/include/progress_state.h"

namespace KAA
{
	namespace FileSecurity
	{
		class FileProgressHandler
		{
		public:
			progress_state ChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size);

		private:
			virtual progress_state IChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size) = 0;
		};
	}
}
