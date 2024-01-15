#include "v2/sketch.h"
#include "v2/plane.h"

VertexArray mesh;
ShaderProgram program;

void setup() {
	Plane plane = MakePlane(1, 1);

	mesh = VertexArrayBuilder()
		.buffer(0).data(pack(plane.pos, plane.uvs))
		.index().data(plane.index)
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 3)
			.attribute(1).type(AttributeTypeFloat, 2)
		.build()
		.upload();

	program = ShaderProgramBuilder()
		.vertex(R"(
			#version 330 core
			layout(location = 0) in vec3 pos;
			layout(location = 1) in vec2 uv;
			out vec2 v_uv;
			void main() {
				gl_Position = vec4(pos, 1.0);
				v_uv = uv;
			}
		)")
		.fragment(R"(
			#version 330 core
			in vec2 v_uv;
			out vec4 outColor;

			uniform float u_time;
			uniform float u_radius;
			uniform vec3 u_color;

			void main() {
				vec2 uv = v_uv * 2.0 - 1.0;
				float r = length(uv);
				float a = atan(uv.y, uv.x);

				r += sin(u_time * 2.0 + r * 10.0) * .1;

				float d = .01 / abs(r - u_radius);
				vec3 color = u_color * d;

				outColor = vec4(color, 1.0);
			}
		)")
		.build()
		.compile();

	size(900, 900);

	totalTime = 0;
}

void draw() {
	if (program.use()) {
		program.setf("u_time", totalTime);
		program.setf("u_radius", .5);
		program.setf3("u_color", vec3(.1, .05, .3));
		mesh.draw();
	}
}
