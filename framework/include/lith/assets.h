#pragma once

// assets manager
// set the root path of assets
// scan the directory

// find the common types of files
// .bank -> fmod loader

// also allow loading files by name without proper path

#include <string>
#include <vector>

// may want to not expose this
#include <filesystem>

struct AssetBankFile {
	std::filesystem::path path;
	int dependencies;
};

class AssetBank {
public:
	AssetBank() = default;

	AssetBank& root(const char* assetDirectoryPath);
	
	// Populate the bank with a list of all files in the root directory
	AssetBank& scan();

	// This is super simple right now, could make a system like the JobSystem.
	// basically just have a number which represents the sort order of assets
	// when looked up
	AssetBank& dependency(const char* filename);

	// After adding dependencies, sort the files in order of dependencies
	AssetBank& freeze();

	std::string find(const char* filename);
	std::vector<std::string> findByExtension(const char* extension);

private:
	std::filesystem::path m_root;
	std::vector<AssetBankFile> m_files;
};