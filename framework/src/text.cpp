#include "lith/text.h"
#include "gl/glad.h"

void TextMesh::create() {
	mesh = VertexArrayBuilder()
		.topology(TopologyTriangles)
		.buffer(0).data(sizeof(QuadVertexData))
			.host()
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 2)
			.attribute(1).type(AttributeTypeFloat, 2)
		.build();
}

void TextMesh::free() {
	mesh.free();
}

void TextMesh::upload() {
	mesh.upload();
}

void TextMesh::draw() {
	mesh.upload().draw();
}

void TextMesh::clear() {
	mesh.clearInstances();
}

void TextMesh::addGlyph(const TextMeshGlyph& glyph) {
	QuadVertexData v1;
	v1.pos = vec2(glyph.posMin.x, glyph.posMin.y);
	v1.uv = vec2(glyph.uvMin.x, glyph.uvMin.y);

	QuadVertexData lith;
	lith.pos = vec2(glyph.posMin.x, glyph.posMax.y);
	lith.uv = vec2(glyph.uvMin.x, glyph.uvMax.y);

	QuadVertexData v3;
	v3.pos = vec2(glyph.posMax.x, glyph.posMax.y);
	v3.uv = vec2(glyph.uvMax.x, glyph.uvMax.y);

	QuadVertexData v4;
	v4.pos = vec2(glyph.posMax.x, glyph.posMin.y);
	v4.uv = vec2(glyph.uvMax.x, glyph.uvMin.y);

	mesh.buffer(0).data.addMany({ 
		v1, v3, lith,
		v1, v4, v3
	});
}

void TextMesh::setPixelPerfect(bool pixelPerfect) {
	this->isPixelPerfect = pixelPerfect;
}

void TextProgram::create() {
	const char* vertexShaderSource = R"(
		#version 330 core

		layout (location = 0) in vec2 pos;
		layout (location = 1) in vec2 uv;

		uniform vec2 textPosition;
		uniform float textSize;

		uniform mat4 view;
		uniform mat4 proj;
  
		out vec2 fragUv;

		void main() {
			gl_Position = proj * view * vec4(textPosition + pos * textSize, 1.0, 1.0); // hacky way to put ontop of everything else
			fragUv = uv;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 330 core

		in vec2 fragUv;
		out vec4 outColor;

		uniform sampler2D msdf;
		uniform vec4 bgColor;
		uniform vec4 fgColor;

		float median(float r, float g, float b) {
			return max(min(r, g), min(max(r, g), b));
		}

		float screenPxRange() {
			const float pxRange = 2.0;
			vec2 unitRange = vec2(pxRange) / vec2(textureSize(msdf, 0));
			vec2 screenTexSize = vec2(1.0) / fwidth(fragUv);
			return max(0.5 * dot(unitRange, screenTexSize), 1.0);
		}

		void main() {
			vec3 msd = texture(msdf, fragUv).rgb;
			float sd = median(msd.r, msd.g, msd.b);
			float screenPxDistance = screenPxRange() * (sd - 0.5);
			float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

			outColor = mix(bgColor, fgColor, opacity);
		}
	)";

	program = ShaderProgramBuilder()
		.vertex(vertexShaderSource)
		.fragment(fragmentShaderSource)
		.build()
		.compile();
}

void TextProgram::free() {
	program.free();
}

void TextProgram::use(const mat4& view, const mat4& proj) {
	program.use();
	program.setf16("view", view);
	program.setf16("proj", proj);

	program.setf4("bgColor", vec4(0));
	program.setf4("fgColor", vec4(1));
	program.setf("pxRange", 2.f);        // 2.0 comes from Font.cpp:26

	program.seti("msdf", 0);
}

void TextProgram::setTextSize(float textSize) {
	program.setf("textSize", textSize);
}

void TextProgram::setTextPosition(vec2 textPosition) {
	program.setf2("textPosition", textPosition);
}

void TextRenderer::create() {
	shader.create();
}

void TextRenderer::free() {
	shader.free();
	for (TextMeshInstance& inst : instances) {
		inst.mesh.free();
	}
	instances = {};
}

void TextRenderer::draw(const mat4& view, const mat4& proj) {
	shader.use(view, proj);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const TextureInterface* font = nullptr;

	for (TextMeshInstance& inst : instances) {
		if (font != inst.font) {
			font = inst.font;
			font->activate(0);
		}

		shader.setTextSize(inst.textSize);
		shader.setTextPosition(inst.textPosition);
		inst.mesh.draw();
	}
}

void TextRenderer::clear() {
	for (TextMeshInstance& inst : instances) {
		inst.mesh.clear();
	}
	instances = {};
}

void TextRenderer::addString(vec2 textPosition, float textSize, const TextureInterface* fontTexture, const TextMesh& mesh) {
	auto where = std::upper_bound(instances.begin(), instances.end(), fontTexture,
		[](const TextureInterface* me, const TextMeshInstance& instance) { return me > instance.font; });

	TextMeshInstance inst;
	inst.font = fontTexture;
	inst.mesh = mesh;
	inst.textSize = textSize;
	inst.textPosition = textPosition;

	instances.insert(where, inst);
}