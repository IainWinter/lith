#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"
#include <vector>

using namespace glm;

struct UVSphere {
	std::vector<int> index;
	std::vector<vec3> pos;
	std::vector<vec2> uvs;
};

UVSphere MakeUVSphere(int latCount, int lonCount);