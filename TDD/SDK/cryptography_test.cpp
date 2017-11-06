#include "gtest/gtest.h"
#include "SDK/KAA/include/cryptography.h"

TEST(cryptography, gamma)
{
	const int data = 0x1bca4ac8;
	const int key = 0xce30ffae;
	int encrypted_data = 0;
	KAA::cryptography::gamma(&data, &key, &encrypted_data, sizeof(encrypted_data));
	EXPECT_EQ(0xd5fab566, encrypted_data);
}
