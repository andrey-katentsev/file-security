#pragma once

#include "SDK/KAA/include/keywords.h"
#include "SDK/KAA/include/filesystem/file_progress_handler.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CommunicatorProgressHandler;

		class WiperProgressDispatcher : public filesystem::file_progress_handler
		{
		public:
			WiperProgressDispatcher();
			explicit WiperProgressDispatcher(CommunicatorProgressHandler*);

			CommunicatorProgressHandler* SetProgressHandler(CommunicatorProgressHandler*);

		private:
			CommunicatorProgressHandler* communicator_progress;

			progress_state ichunk_processed(_fsize_t total_processed, _fsize_t total_size) override;
		};
	}
}
