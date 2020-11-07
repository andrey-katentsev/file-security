#include "CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t CoreProgressHandler::OperationStarted(const std::wstring& name)
		{
			return IOperationStarted(name);
		}

		progress_state_t CoreProgressHandler::ChunkProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			return IChunkProcessed(total_processed, total_size);
		}

		progress_state_t CoreProgressHandler::OverallProgress(const uint64_t total_processed, const uint64_t total_size)
		{
			return IOverallProgress(total_processed, total_size);
		}
	}
}
