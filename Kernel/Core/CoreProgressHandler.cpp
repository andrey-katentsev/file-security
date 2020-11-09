#include "CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t CoreProgressHandler::ProcessingStarted(const std::string& name, uint64_t size)
		{
			return IProcessingStarted(name, size);
		}

		progress_state_t CoreProgressHandler::ChunkProcessed(uint64_t size)
		{
			return IChunkProcessed(size);
		}
	}
}
