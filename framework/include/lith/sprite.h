#pragma once

#include "lith/mesh.h"
#include "lith/shader.h"

#include <unordered_map>

class SpriteMesh {
public:
	struct InstanceVertexData {
		vec3 pos; // 0
		vec2 scale; // 8
		float rotation; // 16
		vec2 uvScale; // 20
		vec2 uvOffset; // 28

		//int textureIndex; // 20
	};

	struct QuadVertexData {
		vec2 pos;
		vec2 uv;
	};

	void create();
	void free();
	void draw();
	void clear();

	void addSprite(vec2 xy, vec2 wh, vec2 uvOffset, vec2 uvScale, int textureHandle);

private:
	VertexArray mesh;
    VertexBuffer* instances;
};

class SpriteProgram {
public:
	void create();
	void free();

	void use(const mat4& view, const mat4& proj);
	void setCurrentTexture(int handle);

private:
	ShaderProgram program;
};

class SpriteRenderer {
public:
	void create();
	void free();
	void draw(const mat4& view, const mat4& proj);
	void clear();

	void addSprite(vec2 xy, vec2 wh, int textureHandle);
	void addSprite(vec2 xy, vec2 wh, vec2 uvOffset, vec2 uvScale, int textureHandle);

private:
	SpriteProgram shader;
	std::unordered_map<int, SpriteMesh> mesh;
};