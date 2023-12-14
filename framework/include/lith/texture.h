#pragma once

#include "lith/typedef.h"
#include "lith/color.h"

enum TextureFormat {
	TextureFormatR = 1,
	TextureFormatRG,
	TextureFormatRGB,
	TextureFormatRGBA,
	TextureFormatDepth,
	TextureFormatInt,
	TextureFormatFloat,
};

enum TextureFilter {
	TextureFilterNearest,
	TextureFilterLinear,
};

enum TextureWrap {
	TextureWrapClamp,
	TextureWrapRepeat
};

GLenum getTextureFormat(TextureFormat format);
GLenum getTextureFormatInternal(TextureFormat format);
GLenum getTextureChannelType(TextureFormat format);
GLenum getTextureFilter(TextureFilter filter);
GLenum getTextureWrap(TextureWrap wrap);

int getChannelCount(TextureFormat format);
int getChannelSize(TextureFormat format);
int getPixelStride(TextureFormat format);

class TextureInterface {
public:
	virtual ~TextureInterface() = default;

	virtual void upload() = 0;
	virtual void download() = 0;
	virtual void free() = 0;
	virtual void activate(int unit) const = 0;
	virtual void activateImage(int unit) const = 0;
	virtual int getHandle() const = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual float getAspect() const = 0;
};

class Texture : public TextureInterface {
public:
	Texture();

	Texture& source(const char* filepath);
	Texture& source(const char* data, TextureFormat format, int width, int height);
	Texture& source(TextureFormat format, int width, int height);

	Texture& filter(TextureFilter filter);
	Texture& wrap(TextureWrap wrap);

	Texture& filter(TextureFilter min, TextureFilter mag);
	Texture& wrap(TextureWrap s, TextureWrap t);

	bool inbounds(int x) const;
	bool inbounds(int x, int y) const;

	int index(int x) const;
	int index(int x, int y) const;

	const color& get(int index) const;
	color& get(int index);

	void upload() override;
	void download() override;
	void free() override;
	void activate(int unit) const override;
	void activateImage(int unit) const override;

	int getHandle() const override;
	int getWidth() const override;
	int getHeight() const override;
	float getAspect() const override;

private:
	GLuint handle;
	GLenum type;
	
	char* data;

	int width;
	int height;

	int channelCount;

	TextureFormat format;

	TextureFilter minFilter;
	TextureFilter magFilter;

	TextureWrap sWrap;
	TextureWrap tWrap;
};