#pragma once

#include "level.h"

Scene* getScene(int id);
const std::unordered_map<int, Scene*>& getAllScenes();

Scene* reloadScene(int id);

void addScene(const char* text);
void addAllScenes();

void clearScenes();