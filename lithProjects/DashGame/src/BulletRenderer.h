#pragma once

#include "lith/mesh.h"
#include "lith/shader.h"

struct BulletInstance {
	vec2 position;
	float z;

	float radius;
	vec4 color;
	float lifetime;
};

class BulletRenderer {
public:
	void create();
	void draw(const mat4& view, const mat4& proj);
	void free();
	void clear();

	void addBullet(vec2 position, float radius, vec4 color, float lifetime);

private:
	VertexArray mesh;
	ShaderProgram shader;

	VertexBuffer* bullets;

	float zOffset = 0;
};
