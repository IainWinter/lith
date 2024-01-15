#pragma once

#include <vector>
#include <initializer_list>

#include "lith/math.h"
#include "lith/bytes.h"
#include "lith/typedef.h"

enum BufferType {
	BufferTypeArray,
	BufferTypeElementArray,
	BufferTypeShaderStorage,
};

GLenum getBufferType(BufferType type);

struct BufferData {
	ByteVector data;
	BufferType type;
};

class Buffer {
public:
	Buffer();
	Buffer(const BufferData& data);

	ByteVector& bufferData();

	Buffer& bind();
	Buffer& bindBase(int index);

	Buffer& upload();
	Buffer& download();

	void free();
	void clear();

private:
	GLuint handle;
	BufferData data;
};

class BufferBuilder {
public:
	BufferBuilder& type(BufferType type);

	BufferBuilder& data(int itemSize);
	BufferBuilder& data(int itemSize, int byteCount, void* bytes);
	BufferBuilder& data(std::initializer_list<int> ints);
	BufferBuilder& data(std::initializer_list<float> floats);
	BufferBuilder& data(std::initializer_list<vec2> vec2s);
	BufferBuilder& data(std::initializer_list<vec3> vec3s);
	BufferBuilder& data(std::initializer_list<vec4> vec4s);

	template<typename _t>
	BufferBuilder& data(const std::vector<_t>& vector) {
		return data((int)sizeof(_t), (int)(vector.size() * sizeof(_t)), (void*)vector.data());
	}

	Buffer build();

private:
	BufferData building;
};