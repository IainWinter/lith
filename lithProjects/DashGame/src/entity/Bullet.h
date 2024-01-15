#pragma once

#include "entity.h"

class Bullet : public Entity {
public:
	Bullet(vec2 pos, vec2 vel);

	void addToWorld() override;
	void update() override;
	void draw() override;
	void onCollision(Entity* hit) override;

private:
	vec2 init_pos;
	vec2 init_vel;
	float init_size;

	float lifetime = 3;
};