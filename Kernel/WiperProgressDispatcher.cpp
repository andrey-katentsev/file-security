#include "WiperProgressDispatcher.h"

#include "../Common/CommunicatorProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		WiperProgressDispatcher::WiperProgressDispatcher() :
		communicator_progress(nullptr)
		{}

		WiperProgressDispatcher::WiperProgressDispatcher(std::shared_ptr<CommunicatorProgressHandler> core_progress) :
		communicator_progress(core_progress)
		{}

		std::shared_ptr<CommunicatorProgressHandler> WiperProgressDispatcher::SetProgressHandler(std::shared_ptr<CommunicatorProgressHandler> handler)
		{
			communicator_progress.swap(handler);
			return handler;
		}

		progress_state_t WiperProgressDispatcher::ichunk_processed(_fsize_t total_processed, _fsize_t total_size)
		{
			if(nullptr != communicator_progress)
				return communicator_progress->OperationProgress(total_processed, total_size);
			return progress_state_t::quiet;
		}
	}
}
