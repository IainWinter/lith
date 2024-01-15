#include "lith/bytes.h"
#include <cstring>

ByteVector::ByteVector()
	: itemSize(0)
{}

ByteVector::ByteVector(int itemSize)
	: itemSize(itemSize)
{}

void ByteVector::setRaw(int itemSize, int byteCount, void* bytes) {
	this->itemSize = itemSize;
	raw.resize(byteCount);
	memcpy(raw.data(), bytes, byteCount);
}

void ByteVector::clear() {
	raw = {};
}

int ByteVector::size() const {
	return (int)raw.size();
}

int ByteVector::count() const {
	if (itemSize == 0) { // guard here because default constructor should have 0 count
		return 0;
	}

	return size() / itemSize;
}

const void* ByteVector::data() const {
	return raw.data();
}

void* ByteVector::data() {
	return raw.data();
}

int ByteVector::stride() const {
	return itemSize;
}
