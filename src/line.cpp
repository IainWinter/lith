#include "lith/line.h"

void LineMesh::create() {
	mesh = VertexArrayBuilder()
		.topology(TopologyLines)
		.buffer(0).host().data(sizeof(LineVertex))
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 3)
			.attribute(1).type(AttributeTypeFloat, 4)
		.build();
}

void LineMesh::free() {
	mesh.free();
}

void LineMesh::draw() {
	mesh.upload().draw();
}

void LineMesh::clear() {
	mesh.clear();
}

void LineMesh::addLine(vec3 a, vec3 b, vec4 stroke, float strokeThickness) {
	mesh.bufferData(0).addMany({
		LineVertex{ a, stroke }, 
		LineVertex{ b, stroke }
	});
}

void LineShaderProgram::create() {
	const char* vertexShaderSource = R"(
		#version 330 core

		layout (location = 0) in vec3 pos;
		layout (location = 1) in vec4 stroke;

		uniform mat4 view;
		uniform mat4 proj;

		out vec4 fragStroke;

		void main() {
			gl_Position = proj * view * vec4(pos, 1.0);
			fragStroke = stroke;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 330 core

		in vec4 fragStroke;
		out vec4 outColor;

		void main() {
			outColor = fragStroke;
		}
	)";

	program = ShaderProgramBuilder()
		.vertex(vertexShaderSource)
		.fragment(fragmentShaderSource)
		.build()
		.compile();
}

void LineShaderProgram::free() {
	program.free();
}

void LineShaderProgram::use(const mat4& view, const mat4& proj) {
	program.use();
	program.setf16("view", view);
	program.setf16("proj", proj);
}

void LineRenderer::create() {
	shader.create();
	mesh.create();
}

void LineRenderer::free() {
	shader.free();
	mesh.free();
}

void LineRenderer::draw(const mat4& view, const mat4& proj) {
	shader.use(view, proj);
	mesh.draw();
}

void LineRenderer::clear() {
	mesh.clear();
}

void LineRenderer::addLine(vec3 a, vec3 b, vec4 stroke, float strokeThickness) {
	mesh.addLine(a, b, stroke, strokeThickness);
}