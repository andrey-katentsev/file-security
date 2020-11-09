#include "CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		progress_state_t CommunicatorProgressHandler::OperationStarted(const std::string& name, uint64_t size)
		{
			return IOperationStarted(name, size);
		}

		progress_state_t CommunicatorProgressHandler::OperationProgress(const uint64_t processed)
		{
			return IOperationProgress(processed);
		}
	}
}
