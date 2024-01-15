#include "BulletRenderer.h"

#include "gl/glad.h"

void BulletRenderer::create() {
	vec4 quad[4] = {
		{ vec2(-1, -1), vec2(0, 0) },
		{ vec2(-1,  1), vec2(0, 1) },
		{ vec2( 1,  1), vec2(1, 1) },
		{ vec2( 1, -1), vec2(1, 0) }
	};
        
	mesh = VertexArrayBuilder()
		.topology(TopologyTriangles)
		.index().data({0, 1, 2, 0, 3, 2})
		.buffer(0).data(sizeof(vec4), sizeof(quad), quad)
		.buffer(1).data(sizeof(BulletInstance))
			.host()
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 2)
			.attribute(1).type(AttributeTypeFloat, 2)
		.map(1)
			.instanced()
			.attribute(2).type(AttributeTypeFloat, 2)
			.attribute(3).type(AttributeTypeFloat, 1)
			.attribute(4).type(AttributeTypeFloat, 1)
			.attribute(5).type(AttributeTypeFloat, 4)
			.attribute(6).type(AttributeTypeFloat, 1)
		.build();

    bullets = &mesh.buffer(1);

	shader = ShaderProgramBuilder()
		.vertex(R"(
			#version 330 core

			layout(location = 0) in vec2 position;
			layout(location = 1) in vec2 texcoord;

			layout(location = 2) in vec2 bulletPosition;
			layout(location = 3) in float bulletZ;
			layout(location = 4) in float bulletRadius;
			layout(location = 5) in vec4 bulletColor;
			layout(location = 6) in float bulletLifetime;

			out vec2 frag_uv;
			out vec4 frag_color;
			out float frag_radius;
			out float frag_lifetime;

			uniform mat4 u_view;
			uniform mat4 u_proj;

			void main() {
				frag_uv = texcoord;
				frag_color = bulletColor;
				frag_radius = bulletRadius;
				frag_lifetime = bulletLifetime;

				vec2 pos = position * bulletRadius + bulletPosition;
				gl_Position = u_proj * u_view * vec4(pos, bulletZ, 1);
			}
		)")
		.fragment(R"(
			#version 330 core

			in vec2 frag_uv;
			in vec4 frag_color;
			in float frag_radius;
			in float frag_lifetime;

			out vec4 outColor;

			void main() {
				vec2 uv = frag_uv * 2.0 - 1.0;
				float r = length(uv);

				float d = 1.0 / r - 1.0;
				outColor = vec4(frag_color.rgb, frag_color.a * d);
			}
		)")
		.build()
		.compile();
}

void BulletRenderer::draw(const mat4& view, const mat4& proj) {
	shader.use();
	shader.setf16("u_view", view);
	shader.setf16("u_proj", proj);

	// enable additive blending
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	mesh.upload().draw();
}

void BulletRenderer::free() {
	mesh.free();
	shader.free();
}

void BulletRenderer::clear() {
	mesh.clearInstances();
	zOffset = 1.f;
}

void BulletRenderer::addBullet(vec2 position, float radius, vec4 color, float lifetime) {
	BulletInstance inst;
	inst.position = position;
	inst.z = zOffset;
	inst.radius = radius;
	inst.color = color;
	inst.lifetime = lifetime;

    bullets->data.add(inst);

	zOffset += 0.001f;
}