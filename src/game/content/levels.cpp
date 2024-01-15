#include "content/levels.h"

#include <vector>
#include <unordered_map>

// a list of all levels, to be parsed into a map
// and their scenes later
// the game is so small, could just load all entities at once
// OR just parse the id and load the entities later
// prob will only have a few MB ata the most tho

static std::vector<const char*> levelTestData = {
	R"(
		L 0
		p -4 -4
		d 1 3.5 3.5
		t 0 .2 .6 Controls|Arrow_Keys|Space
		w 0 5 10 0.5
		w 0 -5 10 0.5
		w -5 0 0.5 10.5
		w 5 0 0.5 10.5
	)",
	R"(
		L 1
		p -3.5 0
		d 2 -3.5 -3.5
		e spinner 2 0 1 0.5 4 12
		w -2 0 0.5 3
		w 5 0 0.5 10.5
		w 0 -5 10 0.5
		w -5 0 0.5 10.5
		w 0 5 10 0.5
		w -3.5 1.5 3.5 0.5
	)",
	R"(
		L 2
		p -6 0
		d 3 6 0
		e spinner 0 -2.5 -1 0.5 4 12
		e spinner 0 2.5 1 0.5 4 12  
		w 7.5 0 0.5 14.5
		w 0 7 15 0.5
		w -7.5 0 0.5 14.5
		w 0 -7 15 0.5
	)",
	R"(
		L 3
		p -7 0
		d 4 6.5 -3.5
		w -9 0 0.5 10.5
		w 0 -5 18.5 0.5
		w 0 -2.5 0.5 2.5
		w 9 0 0.5 10.5
		w 0 5 18.5 0.5
		e spinner 2.5 -2.5 1 0 4 12
		e spinner 2.5 2.5 -1 0 4 12
		e spinner 6.5 1.5 -1 0.5 4 12
		e spinner -2.5 -2.5 1 0.5 4 12
	)",
	R"(
		L 4
		p -4 4
		d 5 4 4
		w 0 5 10 0.5
		w 0 -5 10 0.5
		w -5 0 0.5 10.5
		w 5 0 0.5 10.5
		w -2.5 -2.5 2 0.5
		e spinner 0 -2 -1 0.5 4 12
		e spinner 2 0 1 0.5 4 12
		e spinner -2 0 -1 0.5 4 12
		e spinner 0 2 1 0.5 4 12
	)",
	R"(
		L 5
		p -15.5 -2
		e burst 0 1 0 24 1.5 1
		e burst 0 -3.5 0 24 1.5 1
		e burst 12.5 3.5 0 24 1.5 1
		e burst -8 3.5 0 24 1.5 1
		e burst -3.5 -6.5 0 24 1.5 1
		e burst 10.5 -4.5 0 24 1.5 1
		e burst 11.5 -0.5 0 24 1.5 1
		e burst 0.5 4.5 0 24 1.5 1
		e burst -6 6 0 24 1.5 1
		e burst 10 3 0 24 1.5 1
	)",
	R"(
		L 6
		p -6.5 -3.5
		d 7 -6 2.5
		e spinner 5 2.5 -1 1 13.6528 26
		e spinner -3 2.5 1 1 10.3194 8
		e spinner -3 -3 1 0.5 4 12
		w 0 -5 16 0.5
		w 0 5 16 0.5
		w -8 0 0.5 10.5
		w 8 0 0.5 10.5
		w -5 -3.5 0.5 3
		w -4.5 0 6.5 0.5
		w -1 2.5 0.5 2
	)",
	R"(
		L 9
		p -8 0
		d 0 8 0
		e spinner 0 0 1 0.5 9 24
		e spinner -4 0 1 0.5 9 12
		e spinner 4 0 1 0.5 9 12
		w 0 5 20 0.5
		w 0 -5 20.5 0.5
		w 2 0 0.5 3
		w -2 0 0.5 3
		w 10 0 0.5 10.5
		w -10 0 0.5 10.5
	)"
};

std::unordered_map<int, Scene*> scenes;

Scene* getScene(int id) {
	return scenes.at(id);
}

const std::unordered_map<int, Scene*>& getAllScenes() {
	return scenes;
}

Scene* reloadScene(int id) {
	delete scenes.at(id);
	scenes.at(id) = createLevelFromText(levelTestData[id]);
	return scenes.at(id);
}

void addScene(const char* text) {
	Scene* scene = createLevelFromText(text);

	if (scenes.count(scene->id) > 0) {
		delete scenes.at(scene->id);
	}

	scenes[scene->id] = scene;
}

void addAllScenes() {
	clearScenes();

	for (const char* text : levelTestData) {
		addScene(text);
	}	
}

void clearScenes() {
	for (auto& [id, scene] : scenes) {
		delete scene;
	}

	scenes.clear();
}