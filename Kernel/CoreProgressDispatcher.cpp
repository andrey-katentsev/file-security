#include "CoreProgressDispatcher.h"

#include "../Common/CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		CoreProgressDispatcher::CoreProgressDispatcher() :
		communicator_progress(nullptr)
		{}

		CoreProgressDispatcher::CoreProgressDispatcher(CommunicatorProgressHandler* communicator_progress) :
		communicator_progress(communicator_progress)
		{}

		CommunicatorProgressHandler* CoreProgressDispatcher::SetProgressHandler(CommunicatorProgressHandler* communicator_progress)
		{
			CommunicatorProgressHandler* previous = this->communicator_progress;
			this->communicator_progress = communicator_progress;
			return previous;
		}

		progress_state CoreProgressDispatcher::IOperationStarted(const std::wstring& name)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OperationStarted(name);
			return progress_quiet;
		}

		progress_state CoreProgressDispatcher::IChunkProcessed(const uint64_t total_processed, const uint64_t total_size)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->PortionProcessed(total_processed, total_size);
			return progress_quiet;
		}

		progress_state CoreProgressDispatcher::IOverallProgress(const uint64_t total_processed, const uint64_t total_size)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OverallProgress(total_processed, total_size);
			return progress_quiet;
		}
	}
}
