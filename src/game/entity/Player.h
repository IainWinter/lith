#pragma once

#include "entity.h"
#include <unordered_set>

class Player : public Entity {
public:
	Player(vec2 pos);
	
	void addToWorld() override;
	void update() override;
	void draw() override;
	void onCollision(Entity* hit) override;
	void onCollisionEnd(Entity* hit) override;

	void shrinkAtEndOfLevel();

private:
	vec2 init_pos;
	float init_size;

	float size;
	float sizeGoal = 1;
	bool shrinking = false;
	float shrinkLifetime = 0;

	float speed = 5;

	float dashTime = 0.12f;
	float dashCooldownTime = 0.2f;

	float dashTimer = 0.f;
	float dashSpeed = 40;

	int health = 3;

	std::unordered_set<Entity*> touching;
};