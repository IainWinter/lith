#pragma once

#include "lith/math.h"
#include "lith/mesh.h"
#include "lith/shader.h"
#include "lith/texture.h"

enum TextAlign {
	// horizontal alignment
	TextAlignLeft,
	TextAlignRight,

	// vertical alignment
	TextAlignTop,
	TextAlignBottom,
	TextAlignBaseline,

	// this is shared
	TextAlignCenter,
};

struct TextMeshGlyph {
	vec2 posMin;
	vec2 posMax;
	vec2 uvMin;
	vec2 uvMax;

	int textureHandle;
};

class TextMesh {
public:
	struct QuadVertexData {
		vec2 pos;
		vec2 uv;
	};

	void create();
	void free();
	void upload();
	void draw();
	void clear();

	void addGlyph(const TextMeshGlyph& glyph);

	void setPixelPerfect(bool pixelPerfect);

private:
	VertexArray mesh;
	bool isPixelPerfect = false;
};

class TextProgram {
public:
	void create();
	void free();

	void use(const mat4& view, const mat4& proj);
	void setTextSize(float textSize);
	void setTextPosition(vec2 textPosition);

private:
	ShaderProgram program;
};

class TextRenderer {
public:
	void create();
	void free();
	void draw(const mat4& view, const mat4& proj);
	void clear();

	// with this could just add the string as a char
	// need to link the renderer with font?
	// but that also stinks
	// maybe this is fine
	void addString(vec2 textPosition, float textSize, const TextureInterface* fontTexture, const TextMesh& mesh);

private:
	struct TextMeshInstance {
		const TextureInterface* font;
		TextMesh mesh;
		float textSize;
		vec2 textPosition;
	};

	TextProgram shader;

	std::vector<TextMeshInstance> instances;
};