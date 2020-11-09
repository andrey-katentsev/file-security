#pragma once

#include <memory>

#include "./Core/CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CommunicatorProgressHandler;
		class CoreProgressDispatcher final : public CoreProgressHandler
		{
		public:
			CoreProgressDispatcher();
			explicit CoreProgressDispatcher(std::shared_ptr<CommunicatorProgressHandler>);

			std::shared_ptr<CommunicatorProgressHandler> SetProgressHandler(std::shared_ptr<CommunicatorProgressHandler>);

		private:
			std::shared_ptr<CommunicatorProgressHandler> communicator_progress;

			progress_state_t IProcessingStarted(const std::string& name) override;
			progress_state_t IChunkProcessed(uint64_t total_processed, uint64_t total_size) override;
		};
	}
}
