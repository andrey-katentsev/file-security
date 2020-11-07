#pragma once

#include <memory>

#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CoreProgressHandler;

		class CipherProgressDispatcher final : public FileProgressHandler
		{
		public:
			CipherProgressDispatcher();
			explicit CipherProgressDispatcher(std::shared_ptr<CoreProgressHandler>);

			std::shared_ptr<CoreProgressHandler> SetProgressHandler(std::shared_ptr<CoreProgressHandler>);

		private:
			std::shared_ptr<CoreProgressHandler> core_progress;

			progress_state_t IChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size) override;
		};
	}
}
