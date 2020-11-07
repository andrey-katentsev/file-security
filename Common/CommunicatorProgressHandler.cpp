#include "CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t CommunicatorProgressHandler::OperationStarted(const std::string& name)
		{
			return IOperationStarted(name);
		}

		progress_state_t CommunicatorProgressHandler::PortionProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			return IPortionProcessed(total_processed, total_size);
		}

		progress_state_t CommunicatorProgressHandler::OverallProgress(const uint64_t total_processed, const uint64_t total_size)
		{
			return IOverallProgress(total_processed, total_size);
		}
	}
}
