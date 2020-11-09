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
			core_progress.swap(handler);
			return handler;
		}

		progress_state_t CipherProgressDispatcher::IChunkProcessed(const uint64_t overall_bytes_processed)
		{
			if(nullptr != core_progress)
				return core_progress->ChunkProcessed(overall_bytes_processed);
			return progress_state_t::quiet;
		}
	}
}
