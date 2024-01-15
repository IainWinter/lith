#include "lith/sprite.h"

// can remove if passing texture through TextureInterface and call activate
#include "gl/glad.h"

void SpriteMesh::create() {
	QuadVertexData quad[4] = {
		{ vec2(0, 0), vec2(0, 0) },
		{ vec2(0, 1), vec2(0, 1) },
		{ vec2(1, 1), vec2(1, 1) },
		{ vec2(1, 0), vec2(1, 0) }
	};
        
	mesh = VertexArrayBuilder()
		.topology(TopologyTriangles)
		.index().data({0, 1, 2, 0, 3, 2})
		.buffer(0).data(sizeof(QuadVertexData), sizeof(quad), quad)
		.buffer(1).data(sizeof(InstanceVertexData))
			.host()
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 2)
			.attribute(1).type(AttributeTypeFloat, 2)
		.map(1)
			.instanced()
			.attribute(2).type(AttributeTypeFloat, 3)
			.attribute(3).type(AttributeTypeFloat, 2)
			.attribute(4).type(AttributeTypeFloat, 1)
			.attribute(5).type(AttributeTypeFloat, 2)
			.attribute(6).type(AttributeTypeFloat, 2)
		.build();

    instances = &mesh.buffer(1);
}

void SpriteMesh::free() {
	mesh.free();
}

void SpriteMesh::draw() {
	mesh.upload().draw();
}

void SpriteMesh::clear() {
	mesh.clearInstances();
}

void SpriteMesh::addSprite(vec2 xy, vec2 wh, vec2 uvOffset, vec2 uvScale, int textureHandle) {
	InstanceVertexData instance;
	instance.pos = vec3(xy, 0.f);
	instance.scale = wh;
	instance.uvOffset = uvOffset;
	instance.uvScale = uvScale;

    instances->data.add(instance);
}

void SpriteProgram::create() {
	const char* vertexShaderSource = R"(
		#version 330 core

		layout (location = 0) in vec2 pos;
		layout (location = 1) in vec2 uv;
		layout (location = 2) in vec3 instancePos;
		layout (location = 3) in vec2 instanceScale;
		layout (location = 4) in float instanceRotation;
		layout (location = 5) in vec2 instanceUvScale;
		layout (location = 6) in vec2 instanceUvOffset;

		uniform mat4 view;
		uniform mat4 proj;
  
		out vec2 fragUv;

		mat4 calcInstanceModel() {
			float sr = sin(instanceRotation);
			float cr = cos(instanceRotation);

			float m00 = instanceScale.x *  cr;
			float m10 = instanceScale.x *  sr;
			float m01 = instanceScale.y * -sr;
			float m11 = instanceScale.y *  cr;

			float m03 = instancePos.x;
			float m13 = instancePos.y;
			float m23 = instancePos.z;

			return mat4
			(
				m00, m10, 0.0, 0.0,
				m01, m11, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				m03, m13, m23, 1.0
			);
		}

		void main() {
			mat4 model = calcInstanceModel();
			gl_Position = proj * view * model * vec4(pos, 0.0, 1.0);
			fragUv = uv * instanceUvScale + instanceUvOffset;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 330 core

		in vec2 fragUv;

		uniform sampler2D texture;

		out vec4 outColor;

		void main() {
			vec4 color = texture2D(texture, fragUv);
			if (color.a < 0.01) {
				discard;
			}

			outColor = color;
		}
	)";

	program = ShaderProgramBuilder()
		.vertex(vertexShaderSource)
		.fragment(fragmentShaderSource)
		.build()
		.compile();
}

void SpriteProgram::free() {
	program.free();
}

void SpriteProgram::setCurrentTexture(int handle) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, handle);

	program.seti("sprite", 0);
}

void SpriteProgram::use(const mat4& view, const mat4& proj) {
	program.use();
	program.setf16("view", view);
	program.setf16("proj", proj);
}

void SpriteRenderer::create() {
	shader.create();
}

void SpriteRenderer::free() {
	shader.free();
	for (auto& [_, m] : mesh) {
		m.free();
	}
}

void SpriteRenderer::draw(const mat4& view, const mat4& proj) {
	shader.use(view, proj);

	for (auto& [textureHandle, m] : mesh) {
		shader.setCurrentTexture(textureHandle);
		m.draw();
	}
}

void SpriteRenderer::clear() {
	for (auto& [_, m] : mesh) {
		m.clear();
	}
}

void SpriteRenderer::addSprite(vec2 xy, vec2 wh, int textureHandle) {
	addSprite(xy, wh, vec2(0, 0), vec2(1, 1), textureHandle);
}

void SpriteRenderer::addSprite(vec2 xy, vec2 wh, vec2 uvOffset, vec2 uvScale, int textureHandle)
{
	if (mesh.count(textureHandle) == 0) {
		mesh[textureHandle].create(); // this makes this not able to be called from a DLL or other thread
	}

	mesh[textureHandle].addSprite(xy, wh, uvOffset, uvScale, textureHandle);
}