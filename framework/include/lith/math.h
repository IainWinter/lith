#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

using namespace glm;

#define wPI  3.14159265358f
#define w2PI 6.28318530717f

float lerpf(float a, float b, float w);
vec2 lerp(const vec2 &a, const vec2 &b, float w);
vec3 lerp(const vec3 &a, const vec3 &b, float w);
vec4 lerp(const vec4 &a, const vec4 &b, float w);

float clamp(float x, float min, float max);
vec2 clamp(vec2 x, const vec2 &min, const vec2 &max);
vec3 clamp(vec3 x, const vec3 &min, const vec3 &max);
vec4 clamp(vec4 x, const vec4 &min, const vec4 &max);

vec2 safe_normalize(const vec2 &p);
vec2 limit(const vec2 &x, float max);

vec2 rotate(const vec2 &v, float a);
float angle(vec2 v);

vec2 on_unit(float a);
vec3 on_unit3(float phi, float theta);

float aspect(const vec2 &v);

vec2 right(vec2 v);
vec2 left(vec2 v);

// some simple has functions for pairs, should move

#include <functional> // for hash, remove from math!!!

namespace std
{
    template <class T1, class T2>
    struct hash<pair<T1, T2>> {
        std::size_t operator() (const std::pair<T1, T2>& pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    template <>
    struct hash<ivec2> {
        size_t operator()(const ivec2& x) const {
            return (size_t(x.x) << 32) + size_t(x.y);
        }
    };

    template <>
    struct hash<vec2> {
        size_t operator()(const vec2& x) const {
            return (size_t(x.x) << 32) + size_t(x.y);
        }
    };
}