#pragma once

#include "entity.h"

class DeathDome : public Entity {
public:
	DeathDome(vec2 pos);

	void addToWorld() override;
	void update() override ;
	void draw() override ;
	void onCollision(Entity* other) override ;
	void setColliderRadius(float r) ;

private:
	vec2 init_pos;
	float init_radius;
	float init_alpha;
	float init_lifetime;

	float lifetime = 1.f;
	float radius = 0;
	float alpha = 0;
};
