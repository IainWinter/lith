#include "v2/sketch.h"

#include "v2/shader.h"
#include "v2/plane.h"
#include "v2/mesh.h"
#include "v2/lens.h"
#include "v2/font.h"
#include "v2/interpolation.h"

#define SIZE 1000

VertexArray mesh;
ShaderProgram shader;

ShaderProgram gameOfLifeKernel;

Texture textureFront;
Texture textureBack;

// Use these to swap between the front and back textures
Texture* front;
Texture* back;

CameraLens lens;

Font arial;

void setup() {
	size(800, 800);
	vsync(false);

	Plane plane = MakePlane(1, 1);

	mesh = VertexArrayBuilder()
		.buffer(0).data(pack(plane.pos, plane.uvs))
		.index().data(plane.index)
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 3)
			.attribute(1).type(AttributeTypeFloat, 2)
		.build()
		.upload();

	shader = ShaderProgramBuilder()
		.vertex(R"(
			#version 330 core
			layout(location = 0) in vec3 pos;
			layout(location = 1) in vec2 uv;
			out vec2 v_uv;

			uniform mat4 u_projection;
			uniform mat4 u_view;

			void main() {
				v_uv = uv;
				gl_Position = u_projection * u_view * vec4(pos, 1.0);
			}
		)")
		.fragment(R"(
			#version 330 core

			in vec2 v_uv;
			out vec4 color;

			uniform sampler2D u_state;

			void main() {
				float isFilled = texture(u_state, v_uv).r;
				color = vec4(vec3(isFilled), 1.0);
			}
		)")
		.build()
		.compile();

	gameOfLifeKernel = ShaderProgramBuilder()
		.compute(R"(
			#version 430 core
			
			layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;
			
			layout(binding = 0, r8) readonly  uniform image2D u_front;
			layout(binding = 1, r8) writeonly uniform image2D u_back;
			layout(location = 0)     uniform int u_size;

			int getCell(ivec2 cell, int dx, int dy) {
				ivec2 index = cell + ivec2(dx, dy);

				if (index.x <       0) index.x = u_size  - -index.x; 
				if (index.x >= u_size) index.x = index.x - u_size; 
				if (index.y <       0) index.y = u_size  - -index.y; 
				if (index.y >= u_size) index.y = index.y - u_size; 

				return int(imageLoad(u_front, index).r > 0);
			}

			void main() {
   				ivec2 cellIndex = ivec2(gl_GlobalInvocationID.xy);
				float value = imageLoad(u_front, cellIndex).r;

				int neighbors = 0;

				neighbors += getCell(cellIndex, -1, -1);
				neighbors += getCell(cellIndex,  0, -1);
				neighbors += getCell(cellIndex,  1, -1);
				neighbors += getCell(cellIndex, -1,  0);
				neighbors += getCell(cellIndex,  1,  0);
				neighbors += getCell(cellIndex, -1,  1);
				neighbors += getCell(cellIndex,  0,  1);
				neighbors += getCell(cellIndex,  1,  1);

				if (value == 1.0) {
					value = (neighbors == 2 || neighbors == 3) ? 1.0 : 0.0;
				}

				if (value == 0.0 && (neighbors == 3)) {
					value = 1.0;
				}

				imageStore(u_back, cellIndex, vec4(value, 0, 0, 0));
			}
		)")
		.build()
		.compile();

	textureFront
		.source(TextureFormatR, SIZE, SIZE)
		.filter(TextureFilterNearest)
		.upload();

	textureBack
		.source(TextureFormatR, SIZE, SIZE)
		.filter(TextureFilterNearest)
		.upload();

	front = &textureFront;
	back = &textureBack;

	arial
		.source("C:/Windows/Fonts/seguisb.ttf")
		.scale(32)
		.generate()
		.upload();

	textFont(arial);
	textAlign(TextAlignLeft, TextAlignTop);
	textSize(.1);
	stroke(255);
	fill(255);

	lens = lens_Orthographic(2, 2, -1, 1);
}

bool running = false;

void draw() {
	lens.aspect = width / (float)height;
	camera(lens);

	if (mousePressedOnce) {
		front->download();
	}

	if (mousePressed) {
		vec2 start = vec2(pmouseX, pmouseY);
		vec2 end = vec2(mouseX, mouseY);

		for (LinearInterpolator itr(start, end, sqrt(1)); itr.more(); itr.next()) {
			vec2 pos = itr.current();

			int x = pos.x;
			int y = pos.y;

			if (front->inbounds(x, y)) {
				front->get(front->index(x, y)).r = 255;
			}
		}
		
		front->upload();
	}

	else {
		if (keyCodeOnce == KEY_P) {
			running = !running;
			v2Log("Running: {}", running ? "true" : "false");
		}

		if (running || keyCodeOnce == KEY_Space) {
			front->activateImage(0);
			back->activateImage(1);

			gameOfLifeKernel.use();
			gameOfLifeKernel.seti("u_front", 0);
			gameOfLifeKernel.seti("u_back", 1);
			gameOfLifeKernel.seti("u_size", SIZE);
			gameOfLifeKernel.dispatch(SIZE/4, SIZE/4, 1);
		
			std::swap(front, back);
		}
	}

	front->activate(0);

	shader.use();
	shader.seti("u_state", 0);
	shader.setf16("u_projection", lens.GetProjectionMatrix());
	shader.setf16("u_view", lens.GetViewMatrix());
	mesh.draw();

	textSize(.1);
	text(running ? "Press P to pause" : "Press P to run", -.95, .95);
	text("Press Space to step once", -.95, .95 - .1);

	textSize(.0225);
	text("v2", -.95, -.95);
}
