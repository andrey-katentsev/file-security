#pragma once

#include "KAA/include/keywords.h"

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
			explicit CoreProgressDispatcher(CommunicatorProgressHandler*);

			CommunicatorProgressHandler* SetProgressHandler(CommunicatorProgressHandler*);

		private:
			CommunicatorProgressHandler* communicator_progress;

			progress_state IOperationStarted(const std::wstring& name) override;
			progress_state IChunkProcessed(uint64_t total_processed, uint64_t total_size) override;
			progress_state IOverallProgress(uint64_t total_processed, uint64_t total_size) override;
		};
	}
}
