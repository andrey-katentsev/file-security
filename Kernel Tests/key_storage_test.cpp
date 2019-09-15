#include "gtest/gtest.h"
#include "../Kernel/Kernel.h"

using namespace KAA::FileSecurity;

// FUTURE: KAA: mock key storage.
TEST(key_storage, DISABLED_default_path)
{
	const auto core = GetClassObject();
	const auto path = core->GetKeyStoragePath();
	const KAA::filesystem::path::directory default_key_storage_path { LR"(.\keys)" };
	EXPECT_EQ(default_key_storage_path, path);
}
