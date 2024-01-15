#pragma once

#include "lith/render.h"

#include "lith/line.h"
#include "lith/rect.h"
#include "lith/text.h"
//#include "lith/sprite.h"

#include "lith/font.h"

class SketchRenderBackend : public RenderBackendInterface {
public:
	void create() override;
	void free() override;

	void clear() override;
	void draw() override;

	std::pair<int, int> getViewportSize() const override;
	float getPixelDensity() const override;
	const CameraLens& getCamera() const override;

	void setViewport(float width, float height) override;
	void setPixelDensity(float density) override;
	void setCamera(const CameraLens& lens) override;

	void line(vec3 positionBegin, vec3 positionEnd, vec4 stroke) override;
	void rect(vec2 position, vec2 size, float rotation, vec4 fill, vec4 stroke, float strokeThickness) override;
	void text(vec2 position, float size, TextMeshGenerationConfig alignment, const Font& font, const std::string& text) override;
	
	// put in sprite, should change it to use interface first

	// allow access to simple data

private:
	int m_width;
	int m_height;
	float m_pixelDensity;

	CameraLens m_lens;

	LineRenderer m_line;
	RectRenderer m_rect;
	TextRenderer m_text;
	//SpriteRenderer* sprite;

	FontTextMeshCache m_textCache;
};