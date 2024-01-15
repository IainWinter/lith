#include "lith/rect.h"

void RectMesh::create() {
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
			.attribute(5).type(AttributeTypeFloat, 1)
			.attribute(6).type(AttributeTypeFloat, 4)
			.attribute(7).type(AttributeTypeFloat, 4)
		.build();

    instances = &mesh.buffer(1);
}

void RectMesh::free() {
	mesh.free();
}

void RectMesh::draw() {
	mesh.upload().draw();
}

void RectMesh::clear() {
	mesh.clearInstances();
}

void RectMesh::addRect(vec2 xy, vec2 wh, float rotation, vec4 fill, vec4 stroke, float strokeThickness) {
	InstanceVertexData instance;
	instance.pos = vec3(xy, 0.f);
	instance.scale = wh;
	instance.rotation = rotation;
	instance.fillColor = fill;
	instance.strokeColor = stroke;
	instance.strokeThickness = strokeThickness;

    instances->data.add(instance);
}

void FilledSDFProgram::create() {
	const char* vertexShaderSource = R"(
		#version 330 core

		layout (location = 0) in vec2 pos;
		layout (location = 1) in vec2 uv;
		layout (location = 2) in vec3 instancePos;
		layout (location = 3) in vec2 instanceScale;
		layout (location = 4) in float instanceRotation;
		layout (location = 5) in float instanceEdgeThickness;
		layout (location = 6) in vec4 instanceStroke;
		layout (location = 7) in vec4 instanceFill;

		uniform mat4 view;
		uniform mat4 proj;
        uniform float pixelDensity;
  
		out vec2 fragUv;
		out vec2 fragThickness;
		out vec4 fragStroke;
		out vec4 fragFill;

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
			fragUv = uv;
			fragThickness = instanceEdgeThickness / instanceScale / pixelDensity;
			fragStroke = instanceStroke;
			fragFill = instanceFill;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 330 core

		in vec2 fragUv;
		in vec2 fragThickness;
		in vec4 fragStroke;
		in vec4 fragFill;

		out vec4 outColor;

		float edgeDistance() {
			vec2 st = fragUv;
			vec2 bl = step(fragThickness, st);       // bottom-left
			vec2 tr = step(fragThickness, 1.0 - st); // top-right

			return bl.x * bl.y * tr.x * tr.y;
		}

		void main() {
			float blendFill = edgeDistance();
			vec4 color = mix(fragStroke, fragFill, blendFill);

			outColor = color;
		}
	)";

	program = ShaderProgramBuilder()
		.vertex(vertexShaderSource)
		.fragment(fragmentShaderSource)
		.build()
		.compile();
}

void FilledSDFProgram::free() {
	program.free();
}

void FilledSDFProgram::use(const mat4& view, const mat4& proj, float pixelDensity) {
	program.use();
	program.setf16("view", view);
	program.setf16("proj", proj);
    program.setf("pixelDensity", pixelDensity);
}

void RectRenderer::create() {
	shader.create();
	mesh.create();
}

void RectRenderer::free() {
	shader.free();
	mesh.free();
}

void RectRenderer::draw(const mat4& view, const mat4& proj, float pixelDensity) {
	shader.use(view, proj, pixelDensity);
	mesh.draw();
}

void RectRenderer::clear() {
	mesh.clear();
}

void RectRenderer::addRect(vec2 xy, vec2 wh, float rotation, vec4 fill, vec4 stroke, float strokeThickness) {
	mesh.addRect(xy, wh, rotation, fill, stroke, strokeThickness);
}