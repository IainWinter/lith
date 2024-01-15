#pragma once

#include "entity.h"

class Wall : public Entity {
public:
	Wall(vec2 pos, vec2 size);

	vec2 size() const;
	void addToWorld() override;
	void update() override;
	void draw() override;
	bool handleMouseAndDrawGizmo(DebugMouseCtx* ctx) override;

private:
	vec2 init_pos;
	vec2 init_size;
};