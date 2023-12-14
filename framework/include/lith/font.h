#pragma once

#include "lith/text.h"
#include <unordered_map>

struct FontGlyph {
	vec2 posMin;
	vec2 posMax;
	vec2 uvMin;
	vec2 uvMax;

	float advance;
	int index;
	int character;
};

struct FontGenerationInput {
	const char* filepath;
	float generationScale;
	float linePaddingTop;
	float linePaddingBottom;

	FontGenerationInput();
};

struct FontGenerationOutput {
	Texture atlas;
	std::unordered_map<std::pair<int, int>, float> kerning;
	std::unordered_map<uint32_t, FontGlyph> glpyhs;

	float spaceAdvance;
	float lineHeight;
	float topHeight;
	float bottomHeight;
	
	FontGenerationOutput();

	FontGlyph getGlyph(int c) const;
};

class FontGeneratorInterface {
public:
	virtual FontGenerationOutput generate(const FontGenerationInput& config) const = 0;
};

void registerFontGeneratorInterface(FontGeneratorInterface* generator);

struct TextMeshGenerationConfig {
	TextAlign alignX = TextAlignLeft;
	TextAlign alignY = TextAlignBaseline;
	float lineHeightScale = 1.f;
};

class Font : public TextureInterface {
public:
	Font();

	Font& source(const char* filepath);
	Font& scale(float generationScale);
	Font& linePadding(float paddingTop, float paddingBottom);
	Font& characterPadding(float paddingX, float paddingY);

	Font& generate();

	TextMesh createTextMesh(const char* string, const TextMeshGenerationConfig& config) const;
	
	//vec2 calcTextSize(const char* string, const TextMeshGenerationConfig& config);

	void upload() override;
	void download() override;
	void free() override;

	void activate(int unit) const override;
	void activateImage(int unit) const override;

	int getHandle() const override;
	int getWidth() const override;
	int getHeight() const override;
	float getAspect() const override;

	FontGlyph getGlyph(int c) const;
	float getKerning(int c1, int c2) const;

private:
	FontGenerationInput input;
	FontGenerationOutput data;

	float characterPaddingX;
	float characterPaddingY;
	float linePaddingTop;
	float linePaddingBottom;

	int tabSpaceCount;
};

// maybe put this in another file

class FontTextMeshCache {
public:
	TextMesh& getOrCreateTextMesh(const char* string, const TextMeshGenerationConfig& config, const Font& font);

	void clear();
	void forceClear();

private:
	struct CachedTextMesh {
		TextMesh mesh;
		int keepAliveFrameCount;
	};

	std::unordered_map<size_t, CachedTextMesh> strings;
};