#include "CoreProgressDispatcher.h"

#include "../Common/CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		CoreProgressDispatcher::CoreProgressDispatcher() :
		communicator_progress(nullptr)
		{}

		CoreProgressDispatcher::CoreProgressDispatcher(std::shared_ptr<CommunicatorProgressHandler> communicator_progress) :
		communicator_progress(communicator_progress)
		{}

		std::shared_ptr<CommunicatorProgressHandler> CoreProgressDispatcher::SetProgressHandler(std::shared_ptr<CommunicatorProgressHandler> handler)
		{
			communicator_progress.swap(handler);
			return handler;
		}

		progress_state_t CoreProgressDispatcher::IOperationStarted(const std::string& name)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OperationStarted(name);
			return progress_state_t::quiet;
		}

		progress_state_t CoreProgressDispatcher::IChunkProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OperationProgress(total_processed, total_size);
			return progress_state_t::quiet;
		}
	}
}
