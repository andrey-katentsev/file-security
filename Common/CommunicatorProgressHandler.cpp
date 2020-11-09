#include "CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t CommunicatorProgressHandler::OperationStarted(const std::string& name)
		{
			return IOperationStarted(name);
		}

		progress_state_t CommunicatorProgressHandler::OperationProgress(const uint64_t overall_processed, const uint64_t total_size)
		{
			return IOperationProgress(overall_processed, total_size);
		}
	}
}
