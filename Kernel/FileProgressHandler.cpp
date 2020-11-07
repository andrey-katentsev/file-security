#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t FileProgressHandler::ChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			return IChunkProcessed(total_bytes_processed, total_file_size);
		}
	}
}
