#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"
#include <vector>

using namespace glm;

struct Icosphere {
	std::vector<int> index;
	std::vector<vec3> pos;
	std::vector<vec2> uvs;
};

Icosphere MakeIcosphere(int resolution);