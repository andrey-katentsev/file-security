#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t FileProgressHandler::ChunkProcessed(uint64_t overall_bytes_processed)
		{
			return IChunkProcessed(overall_bytes_processed);
		}
	}
}
