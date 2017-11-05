#include "CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state CoreProgressHandler::OperationStarted(const std::wstring& name)
		{
			return IOperationStarted(name);
		}

		progress_state CoreProgressHandler::ChunkProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			return IChunkProcessed(total_processed, total_size);
		}

		progress_state CoreProgressHandler::OverallProgress(const uint64_t total_processed, const uint64_t total_size)
		{
			return IOverallProgress(total_processed, total_size);
		}
	}
}
