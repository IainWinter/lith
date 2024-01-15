#include "lith/texture.h"
#include "lith/io.h"
#include "gl/glad.h"
#include <cstdlib>
#include <cstring>

// should replace with a array lookup to remove branch

GLenum getTextureFormat(TextureFormat format) {
	switch (format) {
		case TextureFormatR:     return GL_RED;
		case TextureFormatRG:    return GL_RG;
		case TextureFormatRGB:   return GL_RGB;
		case TextureFormatRGBA:  return GL_RGBA;
		case TextureFormatDepth: return GL_DEPTH_COMPONENT;
		case TextureFormatInt:   return GL_RGBA_INTEGER;
		case TextureFormatFloat: return GL_RGBA;
	}

	return GL_NONE;
}

GLenum getTextureFormatInternal(TextureFormat format) {
	switch (format) {
		case TextureFormatR:     return GL_R8;
		case TextureFormatRG:    return GL_RG8;
		case TextureFormatRGB:   return GL_RGB8;
		case TextureFormatRGBA:  return GL_RGBA8;
		case TextureFormatDepth: return GL_DEPTH_COMPONENT32;
		case TextureFormatInt:   return GL_RGBA32I;
		case TextureFormatFloat: return GL_RGBA32F;
	}

	throw nullptr;
}

GLenum getTextureChannelType(TextureFormat format) {
	switch (format) {
		case TextureFormatR:     return GL_UNSIGNED_BYTE;
		case TextureFormatRG:    return GL_UNSIGNED_BYTE;
		case TextureFormatRGB:   return GL_UNSIGNED_BYTE;
		case TextureFormatRGBA:  return GL_UNSIGNED_BYTE;
		case TextureFormatDepth: return GL_FLOAT;
		case TextureFormatInt:   return GL_INT;
		case TextureFormatFloat: return GL_FLOAT;
	}

	throw nullptr;
}

GLenum getTextureFilter(TextureFilter filter) {
	switch (filter) {
		case TextureFilterNearest: return GL_NEAREST;
		case TextureFilterLinear:  return GL_LINEAR;
	}

	throw nullptr;
}

GLenum getTextureWrap(TextureWrap wrap) {
	switch (wrap) {
		case TextureWrapClamp:  return GL_CLAMP_TO_EDGE;
		case TextureWrapRepeat: return GL_REPEAT;
	}

	throw nullptr;
}

int getChannelCount(TextureFormat format) {
	switch (format) {
		case TextureFormatR:     return 1;
		case TextureFormatRG:    return 2;
		case TextureFormatRGB:   return 3;
		case TextureFormatRGBA:  return 4;
		case TextureFormatDepth: return 1;
		case TextureFormatInt:   return 1;
		case TextureFormatFloat: return 1;
	}

	throw nullptr;
}

int getChannelSize(TextureFormat format) {
	switch (format) {
		case TextureFormatR:     return 1;
		case TextureFormatRG:    return 1;
		case TextureFormatRGB:   return 1;
		case TextureFormatRGBA:  return 1;
		case TextureFormatDepth: return 4;
		case TextureFormatInt:   return 4;
		case TextureFormatFloat: return 4;
	}

	throw nullptr;
}

int getPixelStride(TextureFormat format) {
	return getChannelCount(format) * getChannelSize(format);
}

Texture::Texture()
	: handle       (0)
	, type         (GL_TEXTURE_2D)
	, data         (nullptr)
	, width        (0)
	, height       (0)
	, channelCount (0)
	, format       (TextureFormatRGBA)
	, minFilter    (TextureFilterLinear)
	, magFilter    (TextureFilterLinear)
	, sWrap        (TextureWrapClamp)
	, tWrap        (TextureWrapClamp)
{}

Texture& Texture::source(const char* filepath) {
	lithImageData img = lithLoadImage(filepath);

	format = (TextureFormat)img.channels;
	width = img.width;
	height = img.height;
	channelCount = img.channels;
	data = img.buffer;

	return *this;
}

Texture& Texture::source(const char* data, TextureFormat format, int width, int height) {
	int size = width * height * getPixelStride(format);

	this->format = format;
	this->width = width;
	this->height = height;
	this->channelCount = getChannelCount(format);
	this->data = (char*)malloc(size);
	
	if (this->data)
		memcpy(this->data, data, size);

	return *this;
}

Texture& Texture::source(TextureFormat format, int width, int height) {
	int size = width * height * getPixelStride(format);

	this->format = format;
	this->width = width;
	this->height = height;
	this->channelCount = getChannelCount(format);
	this->data = (char*)malloc(size);

	if (this->data)
		memset(this->data, 0, size);

	return *this;
}

Texture& Texture::filter(TextureFilter filter) {
	return Texture::filter(filter, filter);
}

Texture& Texture::wrap(TextureWrap wrap) {
	return Texture::wrap(wrap, wrap);
}

Texture& Texture::filter(TextureFilter min, TextureFilter mag) {
	minFilter = min;
	magFilter = mag;

	return *this;
}

Texture& Texture::wrap(TextureWrap s, TextureWrap t) {
	sWrap = s;
	tWrap = t;

	return *this;
}

bool Texture::inbounds(int x) const {
	return x >= 0 && x < width;
}

bool Texture::inbounds(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

int Texture::index(int x) const {
	return x * channelCount;
}

int Texture::index(int x, int y) const {
	return (x + y * width) * channelCount;
}

const color& Texture::get(int index) const {
	return *(color*)(data + index);
}

color& Texture::get(int index) {
	return *(color*)(data + index);
}

void Texture::upload() {
	if (!handle) {
		glGenTextures(1, &handle);
	}

	glBindTexture(type, handle);

	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getTextureFilter(minFilter));
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, getTextureFilter(magFilter));

	glTexParameteri(type, GL_TEXTURE_WRAP_S, getTextureWrap(sWrap));
	glTexParameteri(type, GL_TEXTURE_WRAP_T, getTextureWrap(tWrap));

	GLenum iformat = getTextureFormatInternal(format);
	GLenum tformat = getTextureFormat(format);
	GLenum ctype = getTextureChannelType(format);

	// I've never had to use this but whatever. Should only have to 
	// enable if the texture isn't a power of 2
	// Is this per texture or global?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// iformat here was using tformat
	// why?
	glTexImage2D(type, 0, iformat, width, height, 0, tformat, ctype, data);
}

void Texture::download() {
	GLenum tformat = getTextureFormat(format);
	GLenum ctype = getTextureChannelType(format);

	glBindTexture(type, handle);
	glGetTexImage(type, 0, tformat, ctype, data);
}

void Texture::free() {
	glDeleteTextures(1, &handle);
	handle = 0;

	::free(data);
	data = nullptr;
}

void Texture::activate(int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(type, handle);
}

void Texture::activateImage(int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindImageTexture(unit, handle, 0, GL_FALSE, 0, GL_READ_WRITE, getTextureFormatInternal(format));
}

int Texture::getHandle() const {
	return handle;
}

int Texture::getWidth() const {
	return width;
}

int Texture::getHeight() const {
	return height;
}

float Texture::getAspect() const {
	return width / (float)height;
}