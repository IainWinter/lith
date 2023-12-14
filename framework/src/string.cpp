#include "lith/string.h"

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

std::string trim(const std::string& input) {
	size_t start = input.find_first_not_of(" \t\r\n");

	if (start == std::string::npos) {
		return "";
	}

	size_t end = input.find_last_not_of(" \t\r\n");
	size_t length = end - start + 1;

	return input.substr(start, length);
}