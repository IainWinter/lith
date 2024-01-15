#pragma once

#include "entity.h"
#include <string>

class Text : public Entity {
public:
	Text(vec2 pos, float size, const std::string& text);

	void update() override;
	void draw() override;

	void addToWorld() override;

public:
	vec2 pos;
	float size;

	std::string text;
};