#include "entity/Text.h"

#include "lith/sketchapi.h"

Text::Text(vec2 pos, float size, const std::string& text) {
	this->pos = pos;
	this->size = size;
	this->text = text;
}

void Text::update() {
}

void Text::draw() {
	textSize(size);
	textAlign(TextAlignCenter, TextAlignCenter);
	::text(text.c_str(), pos.x, pos.y);
}

void Text::addToWorld() {
}