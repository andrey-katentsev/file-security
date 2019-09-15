#include "CipherProgressDispatcher.h"

#include "./Core/CoreProgressHandler.h"

namespace KAA
{
	namespace FileSecurity
	{
		CipherProgressDispatcher::CipherProgressDispatcher() :
		core_progress(nullptr)
		{}

		CipherProgressDispatcher::CipherProgressDispatcher(std::shared_ptr<CoreProgressHandler> core_progress) :
		core_progress(core_progress)
		{}

		std::shared_ptr<CoreProgressHandler> CipherProgressDispatcher::SetProgressHandler(std::shared_ptr<CoreProgressHandler> handler)
		{
			auto previous = std::move(core_progress);
			core_progress = std::move(handler);
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
