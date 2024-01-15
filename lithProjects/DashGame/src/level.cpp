#include "level.h"

#include "lith/log.h"

#include "entity/Player.h"
#include "entity/Enemy.h"
#include "entity/Wall.h"
#include "entity/Door.h"
#include "entity/Text.h"

#include <sstream>
#include <algorithm>

std::string readString(const std::string& str, int* index) {
	int b = str.find_first_of(' ', *index) + 1;
	int e = str.find_first_of(" \n", b);
	*index = e;
	return str.substr(b, e - b);
}

float readFloat(const std::string& str, int* index) {
	int b = str.find_first_of(' ', *index) + 1;
	int e = str.find_first_of(" \n", b);
	*index = e;
	return std::stof(str.substr(b, e - b));
}

int readInt(const std::string& str, int* index) {
	int b = str.find_first_of(' ', *index) + 1;
	int e = str.find_first_of(" \n", b);
	*index = e;
	return std::stoi(str.substr(b, e - b));
}

vec2 readVec2(const std::string& str, int* index) {
	float x = readFloat(str, index);
	float y = readFloat(str, index);
	return vec2(x, y);
}

Scene* createLevelFromText(const std::string& text) {
	Scene* scene = new Scene();
	
	for (int i = 0; i < text.size(); i++) {
		switch (text[i]) {
			case 'L': {
				int id = readInt(text, &i);
				scene->id = id;
				scene->requestId = id;
			
				break;
			}

			case 'd': {
				int id = readInt(text, &i);
				vec2 pos = readVec2(text, &i);

				scene->add(new Door(pos, id));

				break;
			}

			case 'p': {
				vec2 pos = readVec2(text, &i);

				scene->add(new Player(pos));
				break;
			}

			case 'w': {
				vec2 pos = readVec2(text, &i);
				vec2 size = readVec2(text, &i);

				scene->add(new Wall(pos, size));
				break;
			}

			case 't': {
				vec2 pos = readVec2(text, &i);
				float size = readFloat(text, &i);
				std::string str = readString(text, &i);

				std::replace(str.begin(), str.end(), '|', '\n');
				std::replace(str.begin(), str.end(), '_', ' ');

				scene->add(new Text(pos, size, str));
				break;
			}

			case 'e': {
				std::string type = readString(text, &i);
				vec2 pos = readVec2(text, &i);

				if (type == "spinner") {
					SpinnerEnemyProps props;
					props.direction = readFloat(text, &i);
					props.angleOffset = readFloat(text, &i);
					props.rotationsPerSecond = readFloat(text, &i);
					props.shotsPerRotation = readInt(text, &i);

					scene->add(new SpinnerEnemy(pos, props));
				}

				else if (type == "burst") {
					BurstEnemyProps props;
					props.angleOffset = readFloat(text, &i);
					props.numberOfShots = readInt(text, &i);
					props.burstTime = readFloat(text, &i);
					props.burstCooldown = readFloat(text, &i);
				}

				else {
					print("Failed to load level, invalid enemy");
					throw nullptr;
				}

				scene->enemyCount += 1;

				break;
			}
		}
	}

	return scene;
}

std::string createTextFromLevel(const Scene* scene) {
	std::stringstream ss;

	ss << "L " << scene->id << "\n"; 

	for (const Entity* entity : scene->entities) {
		if (const Door* door = dynamic_cast<const Door*>(entity)) {
			ss << "d " << door->id << " " << door->position().x << " " << door->position().y << "\n";
		}

		else if (const Player* player = dynamic_cast<const Player*>(entity)) {
			ss << "p " << player->position().x << " " << player->position().y << "\n";
		}

		else if (const Wall* wall = dynamic_cast<const Wall*>(entity)) {
			ss << "w " << wall->position().x << " " << wall->position().y << " " << wall->size().x << " " << wall->size().y << "\n";
		}

		else if (const Text* text = dynamic_cast<const Text*>(entity)) {
			ss << "t " << text->pos.x << " " << text->pos.y << " " << text->size << " " << text->text << "\n";
		}

		else if (const SpinnerEnemy* enemy = dynamic_cast<const SpinnerEnemy*>(entity)) {
			ss 
				<< "e spinner"
				<< " " << enemy->position().x
				<< " " << enemy->position().y
				<< " " << enemy->props.direction
				<< " " << enemy->props.angleOffset
				<< " " << enemy->props.rotationsPerSecond
				<< " " << enemy->props.shotsPerRotation
				<< "\n";
		}

		else if (const BurstEnemy* enemy = dynamic_cast<const BurstEnemy*>(entity)) {
			ss
				<< "e burst"
				<< " " << enemy->position().x
				<< " " << enemy->position().y
				<< " " << enemy->props.angleOffset
				<< " " << enemy->props.numberOfShots
				<< " " << enemy->props.burstTime
				<< " " << enemy->props.burstCooldown
				<< "\n";
		}
	}

	return ss.str();
}