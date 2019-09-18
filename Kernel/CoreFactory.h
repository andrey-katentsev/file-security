#pragma once

#include <memory>
//#include <unknwn.h>

namespace KAA
{
	namespace filesystem
	{
		class driver;
		namespace path
		{
			class directory;
		}
	}

	namespace FileSecurity
	{
		class Core;
		enum core_t
		{
			strong_security,
			absolute_security
		};

		std::unique_ptr<Core> QueryCore(core_t, std::shared_ptr<filesystem::driver>, filesystem::path::directory key_storage_path);

		/*class CoreFactory : public IUnknown
		{
		public:
			std::unique_ptr<Core> QueryInterface(core_t);
		};*/
	}
}
