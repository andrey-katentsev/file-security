#include "CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state CommunicatorProgressHandler::OperationStarted(const std::wstring& name)
		{
			return IOperationStarted(name);
		}

		progress_state CommunicatorProgressHandler::PortionProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			return IPortionProcessed(total_processed, total_size);
		}

		progress_state CommunicatorProgressHandler::OverallProgress(const uint64_t total_processed, const uint64_t total_size)
		{
			return IOverallProgress(total_processed, total_size);
		}
	}
}
