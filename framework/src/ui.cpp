#include "lith/ui.h"
#include "lith/sketchapi.h"

#include <cstdio>

static UIContext* ctx;

void registerUIContext(UIContext* context) {
	ctx = context;
}

void lithTickUI() {
	auto itr = ctx->sliders.begin();
	while (itr != ctx->sliders.end()) {
		if (itr->second.lastFrameUsed != ctx->frame) {
			itr = ctx->sliders.erase(itr);
		}
		else {
			++itr;
		}
	}
	
	ctx->frame += 1;
}

bool isRectHovered(float x, float y, float width, float height) {
	return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}

bool slider(float* value, float min, float max, float x, float y, float length) {
	SliderData& data = ctx->sliders[value];
	data.x = x;
	data.y = y;
	data.length = length;
	data.minValue = min;
	data.maxValue = max;
	data.lastFrameUsed = ctx->frame;

	return sliderBehavior(value, &data);
}

bool sliderBehavior(float* value, SliderData* s) {
	float valueScaled = (*value - s->minValue) / (s->maxValue - s->minValue);
	float grabWidth = clamp(s->grabWidth + s->velocity * s->grabWidthVelocityMod, s->grabMinWidth, s->grabMaxWidth);
	float grabY = s->y - s->grabHeight / 2;

	s->grabX = s->x - grabWidth / 2 + s->length * valueScaled;

	rect(vec2(s->x, s->y - s->sliderHeight / 2), vec2(s->length, s->sliderHeight));
	rect(vec2(s->grabX, grabY), vec2(grabWidth, s->grabHeight));

	if (!s->isDragging) {
		s->isDragging = mousePressedOnce && isRectHovered(s->grabX, grabY, grabWidth, s->grabHeight);

		if (s->isDragging) {
			s->dragOffsetX = mouseX - s->grabX - grabWidth / 2;
		}
	}

	if (s->isDragging && !mousePressed) {
		s->isDragging = false;
	}

	const float lastValue = valueScaled;

	if (s->isDragging) {
		valueScaled = clamp((mouseX - s->x - s->dragOffsetX) / s->length, 0, 1);
	}

	s->velocity = lerpf(s->velocity + (valueScaled - lastValue), 0, deltaTime * 12);

	*value = valueScaled * (s->maxValue - s->minValue) + s->minValue;

	return s->isDragging;
}

bool sliderWithText(const char* format, float* value, float min, float max, float x, float y, float length) {
	bool valueChanged = slider(value, min, max, x, y, length - 1);

	char buffer[64];
	int charsWritten = snprintf(buffer, sizeof(buffer), format, *value);

	textSize(.3f);
	textAlign(TextAlignLeft, TextAlignCenter);
	text(buffer, x + length - .5, y);

	return valueChanged;
}

bool sliderInt(int* value, int min, int max, float x, float y, float length) {
	float valueF = (float)*value;
	bool valueChanged = slider(&valueF, (float)min, (float)max, x, y, length);
	*value = (int)round(valueF);
	return valueChanged;
}

bool sliderWithTextInt(const char* format, int* value, int min, int max, float x, float y, float length) {
	bool valueChanged = sliderInt(value, min, max, x, y, length - 1);

	char buffer[64];
	int charsWritten = snprintf(buffer, sizeof(buffer), format, *value);

	textSize(.3f);
	textAlign(TextAlignLeft, TextAlignCenter);
	text(buffer, x + length - .5f, y);

	return valueChanged;
}
