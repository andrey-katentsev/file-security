#include "CWDRestorer.h"

namespace KAA
{
	namespace FileSecurity
	{
		CWDRestorer::CWDRestorer() : path(m_filesystem.get_current_working_directory())
		{}

		CWDRestorer::~CWDRestorer()
		{
			// FIX: KAA: exception unsafe!
			m_filesystem.set_current_working_directory(path);
		}
	}
}
