#include "gtest/gtest.h"
#include "../Kernel/Kernel.h"

using namespace KAA::FileSecurity;

TEST(key_storage, default_path)
{
	const auto core = GetClassObject();
	const auto path = core->GetKeyStoragePath();
	const KAA::filesystem::path::directory default_key_storage_path { LR"(.\keys)" };
	EXPECT_EQ(default_key_storage_path, path);
}
