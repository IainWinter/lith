#pragma once

#include "lith/mesh.h"
#include "lith/shader.h"

class LineMesh {
public:
	struct LineVertex {
		vec3 pos;
		vec4 stroke;
	};

	void create();
	void free();
	void draw();
	void clear();

	void addLine(vec3 a, vec3 b, vec4 stroke, float strokeThickness);

private:
	VertexArray mesh;
};

struct LineShaderProgram {
	void create();
	void free();

	void use(const mat4& view, const mat4& proj);

private:
	ShaderProgram program;
};

class LineRenderer {
public:
	void create();
	void free();
	void draw(const mat4& view, const mat4& proj);
	void clear();

	void addLine(vec3 a, vec3 b, vec4 stroke, float strokeThickness);

private:
	LineShaderProgram shader;
	LineMesh mesh;
};