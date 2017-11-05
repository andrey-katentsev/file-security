#include "CipherProgressDispatcher.h"

#include "./Core/CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		CipherProgressDispatcher::CipherProgressDispatcher() :
		core_progress(nullptr)
		{}

		CipherProgressDispatcher::CipherProgressDispatcher(CoreProgressHandler* core_progress) :
		core_progress(core_progress)
		{}

		CoreProgressHandler* CipherProgressDispatcher::SetProgressHandler(CoreProgressHandler* core_progress)
		{
			CoreProgressHandler* previous = this->core_progress;
			this->core_progress = core_progress;
			return previous;
		}

		progress_state CipherProgressDispatcher::IChunkProcessed(const uint64_t total_bytes_processed, const uint64_t total_file_size)
		{
			if(nullptr != core_progress)
				return core_progress->ChunkProcessed(total_bytes_processed, total_file_size);
			return progress_quiet;
		}
	}
}
