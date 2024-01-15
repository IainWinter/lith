#pragma once

#include "entity.h"

struct SpinnerEnemyProps {
	float direction = 1;
	float angleOffset = wPI / 12.f;
	float rotationsPerSecond = 4;
	int shotsPerRotation = 12;
};

struct BurstEnemyProps {
	float angleOffset = 0;
	int numberOfShots = 24;
	float burstTime = 1.5f;
	float burstCooldown = 1.f;
};

class SpinnerEnemy : public Entity {
public:
	SpinnerEnemy(vec2 pos, SpinnerEnemyProps props);

	void addToWorld() override;
	void update() override;
	void draw() override;
	bool handleMouseAndDrawGizmo(DebugMouseCtx* ctx) override;
	
public:
	SpinnerEnemyProps props;

private:
	vec2 init_pos;
	float init_size = 1;
	float shootTimer = 0.f;
	int currentShot = 0;
};

class BurstEnemy : public Entity {
public:
	BurstEnemy(vec2 pos, BurstEnemyProps props) ;

	void addToWorld() override;
	void update() override;
	void draw() override;
	bool handleMouseAndDrawGizmo(DebugMouseCtx* ctx) override;
	
public:
	BurstEnemyProps props;

private:
	vec2 init_pos;
	float init_size = 1;
	float burstTimer = 0.f;
};