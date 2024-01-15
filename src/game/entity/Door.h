#pragma once

#include "entity.h"

class Door : public Entity {
public:
	Door(vec2 pos, int id);

	void addToWorld() override;
	void update() override;
	void draw() override;
	void onCollision(Entity* other) override;
	bool handleMouseAndDrawGizmo(DebugMouseCtx* ctx) override;

public:
	int id;

private:
	vec2 init_pos;
	float init_radius;
	vec4 init_color;

	vec4 color;
	float colorSwitchLifetime;

	bool goingThroughDoor = false;
};