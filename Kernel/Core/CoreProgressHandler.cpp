#include "CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t CoreProgressHandler::OperationStarted(const std::string& name)
		{
			return IOperationStarted(name);
		}

		progress_state_t CoreProgressHandler::ChunkProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			return IChunkProcessed(total_processed, total_size);
		}
	}
}
