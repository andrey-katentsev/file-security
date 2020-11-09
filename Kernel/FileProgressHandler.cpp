#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t FileProgressHandler::ChunkProcessed(uint64_t size)
		{
			return IChunkProcessed(size);
		}
	}
}
