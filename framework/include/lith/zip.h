#pragma once

#include <tuple>
#include <vector>
#include <array>

template<int _size>
struct bytes {
	char data[_size];
};

template<typename _first, typename... _t>
const _first& getFirst(const _first& first, const _t&... rest) {
	return first;
}

template<typename _t>
void packSingle(char* bytes, int& offsetOUT, const _t& value) {
	memcpy(bytes + offsetOUT, &value, sizeof(_t));
	offsetOUT += sizeof(_t);
}

template<typename... _t>
bytes<(sizeof(_t) + ...)> packMany(const _t&... values) {
	bytes<(sizeof(_t) + ...)> result;

	int currentOffset = 0;
	(packSingle(result.data, currentOffset, values), ...);

	return result;
}

template<typename... _t>
std::vector<bytes<(sizeof(_t) + ...)>> pack(const std::vector<_t>&... vectors) {
	std::vector<bytes<(sizeof(_t) + ...)>> result;

	// Assume that all the lists are the same size
	// and just use the first.
	int itemCount = getFirst(vectors...).size();

	result.reserve(itemCount);

	for (int i = 0; i < itemCount; i++) {
		result.push_back(packMany(vectors[i]...));
	}

	return result;
}
