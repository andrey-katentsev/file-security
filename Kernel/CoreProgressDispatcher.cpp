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

		progress_state_t CoreProgressDispatcher::IProcessingStarted(const std::string& name, uint64_t size)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OperationStarted(name, size);
			return progress_state_t::quiet;
		}

		progress_state_t CoreProgressDispatcher::IChunkProcessed(uint64_t overall_processed)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OperationProgress(overall_processed);
			return progress_state_t::quiet;
		}
	}
}
