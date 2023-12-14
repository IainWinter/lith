#include "lith/buffer.h"
#include "gl/glad.h"

#include <string.h>

GLenum getBufferType(BufferType type) {
	switch (type) {
		case BufferTypeArray:         return GL_ARRAY_BUFFER;
		case BufferTypeElementArray:  return GL_ELEMENT_ARRAY_BUFFER;
		case BufferTypeShaderStorage: return GL_SHADER_STORAGE_BUFFER;
	}

	throw nullptr;
}

Buffer::Buffer() 
	: handle (0)
{}

Buffer::Buffer(const BufferData& data) 
	: handle (0)
	, data   (data)
{}

ByteVector& Buffer::bufferData() {
	return data.data;
}

Buffer& Buffer::bind() {
	GLenum type = getBufferType(data.type);

	glBindBuffer(type, handle);
	return *this;
}

Buffer& Buffer::bindBase(int index) {
	GLenum type = getBufferType(data.type);

	glBindBufferBase(type, 0, handle);
	return *this;
}

Buffer& Buffer::upload() {
	if (handle == 0) {
		glGenBuffers(1, &handle);
	}

	GLenum type = getBufferType(data.type);

	glBindBuffer(type, handle);
	glBufferData(type, data.data.size(), data.data.data(), GL_DYNAMIC_DRAW); // hint doesn't matter apparently

	return *this;
}

Buffer& Buffer::download() {
	GLenum type = getBufferType(data.type);

	glBindBuffer(type, handle);
	glGetBufferSubData(type, 0, data.data.size(), data.data.data());
	return *this;
}

void Buffer::free() {
	glDeleteBuffers(1, &handle);
	handle = 0;
	data.data = {};
}

void Buffer::clear() {
	data.data.clear();
}

BufferBuilder& BufferBuilder::type(BufferType type) {
	building.type = type;
	return *this;
}

BufferBuilder& BufferBuilder::data(int itemSize) {
	building.data = ByteVector(itemSize);
	return *this;
}

BufferBuilder& BufferBuilder::data(int itemSize, int byteCount, void* bytes) {
	building.data.setRaw(itemSize, byteCount, bytes);
	return *this;
}

BufferBuilder& BufferBuilder::data(std::initializer_list<int> ints) {
	building.data = ByteVector(ints);
	return *this;
}

BufferBuilder& BufferBuilder::data(std::initializer_list<float> floats) {
	building.data = ByteVector(floats);
	return *this;
}

BufferBuilder& BufferBuilder::data(std::initializer_list<vec2> vec2s) {
	building.data = ByteVector(vec2s);
	return *this;
}

BufferBuilder& BufferBuilder::data(std::initializer_list<vec3> vec3s) {
	building.data = ByteVector(vec3s);
	return *this;
}

BufferBuilder& BufferBuilder::data(std::initializer_list<vec4> vec4s) {
	building.data = ByteVector(vec4s);
	return *this;
}

Buffer BufferBuilder::build() {
	return Buffer(building);
}