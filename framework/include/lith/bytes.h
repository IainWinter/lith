#pragma once

#include <vector>

// This is just a vector which can store POD types as a byte array.
// Items have to be the same size.
class ByteVector {
public:
	ByteVector();
	ByteVector(int itemSize);

	void setRaw(int itemSize, int byteCount, void* bytes);
	void clear();

	int size() const;
	int count() const;
	const void* data() const;
	void* data();
	int stride() const;

	template<typename _t>
	ByteVector(const std::initializer_list<_t>& list)
		: itemSize (sizeof(_t))
	{
		addMany(list);
	}

	template<typename _t>
	ByteVector(const std::vector<_t>& list)
		: itemSize (sizeof(_t))
	{
		addMany(list);
	}

	template<typename _t>
	void addMany(const std::initializer_list<_t>& list) {
		for (const _t& item : list) {
			add(item);
		}
	}

	template<typename _t>
	void addMany(const std::vector<_t>& list) {
		for (const _t& item : list) {
			add(item);
		}
	}

	template<typename _t>
	void add(const _t& item) {
		if (itemSize == 0) {
			throw nullptr;
		}

		raw.insert(raw.end(), itemSize, 0);
		back<_t>() = item;
	}

	template<typename _t>
	_t& at(int index) {
		return *(_t*)(raw.data() + index * itemSize);
	}

	template<typename _t>
	_t& back() {
		return *(_t*)(raw.data() + size() - itemSize);
	}

	template<typename _t>
	const _t& back() const {
		return *(const _t*)(raw.data() + size() - itemSize);
	}

private:
	std::vector<char> raw;
	int itemSize;
};

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
	// Assume that all the lists are the same size, so just use the first.
	int itemCount = getFirst(vectors...).size();

	std::vector<bytes<(sizeof(_t) + ...)>> result;
	result.reserve(itemCount);

	for (int i = 0; i < itemCount; i++) {
		result.push_back(packMany(vectors[i]...));
	}

	return result;
}