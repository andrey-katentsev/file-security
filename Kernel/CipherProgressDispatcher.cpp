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

		progress_state_t CipherProgressDispatcher::IChunkProcessed(uint64_t size)
		{
			if(nullptr != core_progress)
				return core_progress->ChunkProcessed(size);
			return progress_state_t::quiet;
		}
	}
}
