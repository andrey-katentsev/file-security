#include "gtest/gtest.h"

#include "C:/SDK/KAA/include/filesystem/split_path.h"

using KAA::filesystem::split_drive;
using KAA::filesystem::split_directory;
using KAA::filesystem::split_filename;
using KAA::filesystem::split_extension;

namespace
{
	const std::wstring lfs_path(L"D:\\User\\Hyperlink\\eBooks\\windows.txt");
	const std::wstring lfs_drive(L"D:");
	const std::wstring lfs_directory(L"D:\\User\\Hyperlink\\eBooks\\");

	const std::wstring filename(L"windows.txt");
	const std::wstring extension(L".txt");
}

TEST(lfs_path, split_drive)
{
	EXPECT_EQ(lfs_drive, split_drive(lfs_path));
}

TEST(lfs_path, split_directory)
{
	EXPECT_EQ(lfs_directory, split_directory(lfs_path));
}

TEST(lfs_path, split_filename)
{
	EXPECT_EQ(filename, split_filename(lfs_path));
}

TEST(lfs_path, split_extension)
{
	EXPECT_EQ(extension, split_extension(lfs_path));
}

namespace
{
	const std::wstring unc_path(L"\\\\HYPERLINK\\eBooks\\windows.txt");
	const std::wstring unc_drive;
	const std::wstring unc_directory(L"\\\\HYPERLINK\\eBooks\\");
}

TEST(unc_path, split_drive)
{
	EXPECT_EQ(unc_drive, split_drive(unc_path));
}

TEST(unc_path, split_directory)
{
	EXPECT_EQ(unc_directory, split_directory(unc_path));
}

TEST(unc_path, split_filename)
{
	EXPECT_EQ(filename, split_filename(unc_path));
}

TEST(unc_path, split_extension)
{
	EXPECT_EQ(extension, split_extension(unc_path));
}

namespace
{
	const std::wstring long_unc_path(L"\\\\?\\D:\\User\\Hyperlink\\eBooks\\windows.txt");
	const std::wstring long_unc_drive;
	const std::wstring long_unc_directory(L"\\\\?\\D:\\User\\Hyperlink\\eBooks\\");
}

TEST(long_unc_path, split_drive)
{
	EXPECT_EQ(long_unc_drive, split_drive(long_unc_path));
}

TEST(long_unc_path, split_directory)
{
	EXPECT_EQ(long_unc_directory, split_directory(long_unc_path));
}

TEST(long_unc_path, split_filename)
{
	EXPECT_EQ(filename, split_filename(long_unc_path));
}

TEST(long_unc_path, split_extension)
{
	EXPECT_EQ(extension, split_extension(long_unc_path));
}

namespace
{
	const std::wstring uncw_path(L"\\\\?\\UNC\\HYPERLINK\\eBooks\\windows.txt");
	const std::wstring uncw_drive;
	const std::wstring uncw_directory(L"\\\\?\\UNC\\HYPERLINK\\eBooks\\");
}

TEST(uncw_path, split_drive)
{
	EXPECT_EQ(uncw_drive, split_drive(uncw_path));
}

TEST(uncw_path, split_directory)
{
	EXPECT_EQ(uncw_directory, split_directory(uncw_path));
}

TEST(uncw_path, split_filename)
{
	EXPECT_EQ(filename, split_filename(uncw_path));
}

TEST(uncw_path, split_extension)
{
	EXPECT_EQ(extension, split_extension(uncw_path));
}
