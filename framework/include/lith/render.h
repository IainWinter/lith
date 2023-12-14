#pragma once

#include "lith/math.h"
#include "lith/font.h"
#include "lith/lens.h"
#include "lith/texture.h"

#include <string>

class RenderBackendInterface {
public:
	virtual void create() = 0;
	virtual void free() = 0;

	virtual void clear() = 0;
	virtual void draw() = 0;

	virtual std::pair<int, int> getViewportSize() const = 0;
	virtual float getPixelDensity() const = 0;
	virtual const CameraLens& getCamera() const = 0;

	virtual void setViewport(float width, float height) = 0;
	virtual void setPixelDensity(float density) = 0;
	virtual void setCamera(const CameraLens& lens) = 0;

	virtual void line(vec3 positionBegin, vec3 positionEnd, vec4 stroke) = 0;
	virtual void rect(vec2 position, vec2 size, float rotation, vec4 fill, vec4 stroke, float strokeThickness) = 0;
	virtual void text(vec2 position, float size, TextMeshGenerationConfig alignment, const Font& font, const std::string& text) = 0;
	
	// put in sprite, should change it to use interface first
};