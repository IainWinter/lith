#include "SketchRenderBackend.h"
#include "lith/lens.h"
#include "gl/glad.h"

void SketchRenderBackend::create() {
	m_line.create();
	m_rect.create();
	m_text.create();
}

void SketchRenderBackend::free() {
	m_line.free();
	m_rect.free();
	m_text.free();
}

void SketchRenderBackend::clear() {
	m_line.clear();
	m_rect.clear();
	m_text.clear();

	m_textCache.clear();
}

void SketchRenderBackend::draw() {
	glViewport(0, 0, m_width, m_height);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mat4 view = m_lens.GetViewMatrix();
	mat4 proj = m_lens.GetProjectionMatrix();

	m_line.draw(view, proj);
	m_rect.draw(view, proj, m_pixelDensity);
	m_text.draw(view, proj);
}

std::pair<int, int> SketchRenderBackend::getViewportSize() const {
	return { m_width, m_height };
}

float SketchRenderBackend::getPixelDensity() const {
	return m_pixelDensity;
}

const CameraLens& SketchRenderBackend::getCamera() const {
	return m_lens;
}

void SketchRenderBackend::setViewport(float width, float height) {
	m_width = width;
	m_height = height;

	m_lens = lens_Orthographic(height, width / height, -10, 10);
	m_lens.position = vec3(m_lens.ScreenSize() / 2.f, 0);
}

void SketchRenderBackend::setPixelDensity(float density) {
	m_pixelDensity = density;
}

void SketchRenderBackend::setCamera(const CameraLens& lens) {
	m_lens = lens;
}

void SketchRenderBackend::line(vec3 positionBegin, vec3 positionEnd, vec4 stroke) {
	m_line.addLine(positionBegin, positionEnd, stroke, 1.f);	
}

void SketchRenderBackend::rect(vec2 position, vec2 size, float rotation, vec4 fill, vec4 stroke, float strokeThickness) {
	m_rect.addRect(position, size, rotation, fill, stroke, strokeThickness);
}

void SketchRenderBackend::text(vec2 position, float size, TextMeshGenerationConfig alignment, const Font& font, const std::string& text) {
	TextMesh& mesh = m_textCache.getOrCreateTextMesh(text.c_str(), alignment, font);
	m_text.addString(position, size, &font, mesh);
}