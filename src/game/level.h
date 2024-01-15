#pragma once

#include "entity.h"
#include <string>

Scene* createLevelFromText(const std::string& text);
std::string createTextFromLevel(const Scene* scene);