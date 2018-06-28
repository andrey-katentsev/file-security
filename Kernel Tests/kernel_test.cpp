#include "gtest/gtest.h"
#include "../Kernel/Kernel.h"

using namespace KAA::FileSecurity;

TEST(kernel, successfully_creates_and_destroys)
{
	// FUTURE: KAA: test uses real filesystem and registry - mock environment.
	EXPECT_NO_THROW(const auto core = GetClassObject());
}

TEST(kernel, creates_valid_instance)
{
	const auto core = GetClassObject();
	EXPECT_NE(nullptr, core.get());
}
