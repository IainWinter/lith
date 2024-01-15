#include "entity/Wall.h"

#include "global.h"

#include "lith/sketchapi.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"

Wall::Wall(vec2 pos, vec2 size) {
	init_pos = pos;
	init_size = size;
}

vec2 Wall::size() const {
	return init_size;
}

void Wall::addToWorld() {
	b2BodyDef def;
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.userData.pointer = (uintptr_t)this;

	b2PolygonShape shape;
	shape.SetAsBox(init_size.x/2, init_size.y/2);

	b2Filter filter;
	filter.categoryBits = 0b1;

	b2FixtureDef fix;
	fix.shape = &shape;

	body = scene->addBody(def);
	body->CreateFixture(&fix);
}

void Wall::update() {
}

void Wall::draw() {
	noStroke();
	fill(128);
	rect(position() - init_size / 2.f, init_size);
}

bool Wall::handleMouseAndDrawGizmo(DebugMouseCtx* ctx) {
	// draw two grabs at the top and right to expand the size

	vec2 size = vec2(0.2f);

	vec2 top    = position() + vec2(0,                 init_size.y / 2.f) - size / 2.f;
	vec2 right  = position() + vec2(init_size.x / 2.f, 0)                 - size / 2.f;

	fill(100, 255, 100);
	rect(top, vec2(size));
	rect(right, vec2(size));

	if (!ctx->clicked) {
		ctx->gizmo = 0;
	}

	if (ctx->clickedOnce) {
		if (rectHovered(ctx->mouse, top, size)) {
			ctx->gizmoState.y = init_size.y;
			ctx->gizmoMousePos.y = ctx->mouse.y;
			ctx->gizmo = 1;
		}
		
		else if (rectHovered(ctx->mouse, right, size)) {
			ctx->gizmoState.x = init_size.x;
			ctx->gizmoMousePos.x = ctx->mouse.x;
			ctx->gizmo = 2;
		}
	}


	if (ctx->gizmo != 0) {
		b2PolygonShape* shape = (b2PolygonShape*)body->GetFixtureList()->GetShape();

		switch (ctx->gizmo) {
			case 1: {
				init_size.y = max(0.5f, roundToHalf(ctx->gizmoState.y + (ctx->mouse.y - ctx->gizmoMousePos.y) * 2.f));
				break;
			}

			case 2: {
				init_size.x = max(0.5f, roundToHalf(ctx->gizmoState.x + (ctx->mouse.x - ctx->gizmoMousePos.x) * 2.f));
				break;
			}
		}

		shape->SetAsBox(init_size.x / 2, init_size.y / 2);
	}

	return ctx->gizmo != 0; 
}