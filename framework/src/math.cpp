#include "lith/math.h"

float lerpf(float a, float b, float w) {
	return a + w * (b - a);
}

vec2 lerp(const vec2& a, const vec2& b, float w) {
	return a + w * (b - a);
}

vec3 lerp(const vec3& a, const vec3& b, float w) {
	return a + w * (b - a);
}

vec4 lerp(const vec4& a, const vec4& b, float w) {
	return a + w * (b - a);
}

float clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

vec2 clamp(vec2 x, const vec2& min, const vec2& max) {
	x.x = clamp(x.x, min.x, max.x);
	x.y = clamp(x.y, min.y, max.y);
	return x;
}

vec3 clamp(vec3 x, const vec3& min, const vec3& max) {
	x.x = clamp(x.x, min.x, max.x);
	x.y = clamp(x.y, min.y, max.y);
	x.z = clamp(x.z, min.z, max.z);
	return x;
}

vec4 clamp(vec4 x, const vec4& min, const vec4& max) {
	x.x = clamp(x.x, min.x, max.x);
	x.y = clamp(x.y, min.y, max.y);
	x.z = clamp(x.z, min.z, max.z);
	x.w = clamp(x.w, min.w, max.w);
	return x;
}

vec2 safe_normalize(const vec2& p) {
	float n = sqrt((float)(p.x * p.x + p.y * p.y));
	return (n == 0) ? vec2(0.f, 0.f) : vec2(p.x / n, p.y / n);
}

vec2 limit(const vec2& x, float max) {
	float d = length(x);
	if (d > max) return x / d * max;
	return x;
}

vec2 rotate(const vec2& v, float a) {
	float s = sin(a);
	float c = cos(a);
	return vec2(v.x * c - v.y * s, v.x * s + v.y * c);
}

float angle(vec2 v) {
	return atan2(v.y, v.x);
}

vec2 on_unit(float a) {
	return vec2(cos(a), sin(a));
}

vec3 on_unit3(float phi, float theta) {
	float x = sin(phi) * cos(theta);
	float y = sin(phi) * sin(theta);
	float z = cos(phi);

	return vec3(x, y, z);
}

float aspect(const vec2& v) {
	return v.x / v.y;
}

vec2 right(vec2 v) {
	return vec2(v.y, -v.x);
}

vec2 left(vec2 v) {
	return vec2(-v.y, v.x);
}