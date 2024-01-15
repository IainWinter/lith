#pragma once

#include <unordered_map>

struct SliderData {
	float x;
	float y;
	float length;

	float sliderHeight = .05f;

	float grabX;
	float grabWidth = .2f;
	float grabHeight = .2f;
	float grabWidthVelocityMod = .5f;
	float grabMinWidth = .06f;
	float grabMaxWidth = .8f;
	
	bool isDragging = false;
	float dragOffsetX = 0.f;
	float velocity = 0.f;

	float minValue;
	float maxValue;

	int lastFrameUsed;
};

struct UIContext {
	std::unordered_map<void*, SliderData> sliders;
	int frame = 0;
};

void registerUIContext(UIContext* context);
void lithTickUI();

bool isRectHovered(float x, float y, float width, float height);

bool slider(float* value, float min, float max, float x, float y, float length);
bool sliderBehavior(float* value, SliderData* data);
bool sliderWithText(const char* format, float* value, float min, float max, float x, float y, float length);

bool sliderInt(int* value, int min, int max, float x, float y, float length);
bool sliderWithTextInt(const char* format, int* value, int min, int max, float x, float y, float length);
