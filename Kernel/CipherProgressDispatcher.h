#pragma once

#include "KAA/include/keywords.h"

#include "FileProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		class CoreProgressHandler;

		class CipherProgressDispatcher : public FileProgressHandler
		{
		public:
			CipherProgressDispatcher();
			explicit CipherProgressDispatcher(CoreProgressHandler*);

			CoreProgressHandler* SetProgressHandler(CoreProgressHandler*);

		private:
			CoreProgressHandler* core_progress;

			progress_state IChunkProcessed(uint64_t total_bytes_processed, uint64_t total_file_size) override;
		};
	}
}
