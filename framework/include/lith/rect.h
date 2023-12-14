#pragma once

#include "lith/mesh.h"
#include "lith/shader.h"

class RectMesh {
public:
	struct InstanceVertexData {
		vec3 pos; // 0
		vec2 scale; // 8
		float rotation; // 16
		float strokeThickness; // 20
		vec4 strokeColor; // 24
		vec4 fillColor; // 40
	};

	struct QuadVertexData {
		vec2 pos;
		vec2 uv;
	};

	void create();
	void free();
	void draw();
	void clear();

	void addRect(vec2 xy, vec2 wh, float rotation, vec4 fill, vec4 stroke, float strokeThickness);

private:
	VertexArray mesh;
    VertexBuffer* instances;
};

class FilledSDFProgram {
public:
	void create();
	void free();

	void use(const mat4& view, const mat4& proj, float pixelDensity);

private:
	ShaderProgram program;
};

class RectRenderer {
public:
	void create();
	void free();
	void draw(const mat4& view, const mat4& proj, float pixelDensity);
	void clear();

	void addRect(vec2 xy, vec2 wh, float rotation, vec4 fill, vec4 stroke, float strokeThickness);

private:
	FilledSDFProgram shader;
	RectMesh mesh;
};