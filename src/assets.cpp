#include "lith/assets.h"

#include <assert.h>
#include <algorithm>

using namespace std::filesystem;

AssetBank& AssetBank::root(const char* assetDirectoryPath) {
	m_root = assetDirectoryPath;
	return *this;
}

AssetBank& AssetBank::scan() {
	// reset files
	m_files.clear();

	// add all files in directory
	// could add a filter for the file types
	for (path p : recursive_directory_iterator(m_root)) {
		if (is_regular_file(p)) {
			m_files.push_back({ p, 0 });
		}
	}

	return *this;
}

AssetBank& AssetBank::dependency(const char* filename) {
	auto itr = std::find_if(m_files.begin(), m_files.end(), 
		[filename](const auto& file) { return file.path.filename() == filename; });

	if (itr == m_files.end()) {
		assert(false && "File not found");
		return *this;
	}

	itr->dependencies++;
	return *this;
}

AssetBank& AssetBank::freeze() {
	std::sort(m_files.begin(), m_files.end(), 
		[](const auto& a, const auto& b) { return a.dependencies < b.dependencies; });

	return *this;
}

std::string AssetBank::find(const char* filename) {
	for (const AssetBankFile& file : m_files) {
		if (file.path.filename() == filename) {
			return file.path.string();
		}
	}

	assert(false && "Asset not found");
	return "";
}

std::vector<std::string> AssetBank::findByExtension(const char* extension) {
	std::vector<std::string> found;

	// return the list of files sorted by the number of
	// dependencies. Need to call freeze first

	for (const AssetBankFile& file : m_files) {
		if (file.path.extension() == extension) {
			found.push_back(file.path.string());
		}
	}

	return found;
}