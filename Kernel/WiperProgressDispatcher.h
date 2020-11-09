#pragma once

#include <memory>

#include "KAA/include/filesystem/file_progress_handler.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CommunicatorProgressHandler;

		class WiperProgressDispatcher final : public filesystem::file_progress_handler
		{
		public:
			WiperProgressDispatcher();
			explicit WiperProgressDispatcher(std::shared_ptr<CommunicatorProgressHandler>);

			std::shared_ptr<CommunicatorProgressHandler> SetProgressHandler(std::shared_ptr<CommunicatorProgressHandler>);

		private:
			std::shared_ptr<CommunicatorProgressHandler> communicator_progress;

			progress_state_t ichunk_processed(size_t bytes_processed) override;
		};
	}
}
