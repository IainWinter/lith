#pragma once

#include "BulletRenderer.h"

extern BulletRenderer* g_bulletRenderer;

void registerBulletRenderer(BulletRenderer* bulletRenderer);


// some random helpers

inline bool rectHovered(vec2 mouse, vec2 pos, vec2 size) {
	return mouse.x >= pos.x 
		&& mouse.x <= pos.x + size.x 
		&& mouse.y >= pos.y 
		&& mouse.y <= pos.y + size.y;
}

inline float roundToHalf(float x) {
	return floor(x * 2.f + .5f) / 2.f;
}

inline vec2 roundToHalf(vec2 x) {
	return vec2(roundToHalf(x.x), roundToHalf(x.y));
}