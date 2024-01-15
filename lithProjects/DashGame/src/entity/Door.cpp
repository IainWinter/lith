#include "entity/Door.h"
#include "entity/Player.h"

#include "lith/sketchapi.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"

#include "global.h"

Door::Door(vec2 pos, int id) {
	init_pos = pos;
	init_radius = 2.0f;
	init_color = vec4(1., .6, .6, .2);
	this->id = id;

	color = init_color;
	colorSwitchLifetime = 0.0f;
}

void Door::addToWorld() {
	b2BodyDef def;
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.type = b2_staticBody;
	def.userData.pointer = (uintptr_t)this;

	b2CircleShape shape;
	shape.m_radius = init_radius;

	b2FixtureDef fix;
	fix.shape = &shape;
	fix.isSensor = true;

	body = scene->addBody(def);
	body->CreateFixture(&fix);
}

void Door::update() {
	if (scene->enemyCount == 0) {
		color = lerp(init_color, vec4(.6, 1., .6, .3), clamp(colorSwitchLifetime, 0, 1));
		colorSwitchLifetime += deltaTime;
	}
}

void Door::draw() {
	g_bulletRenderer->addBullet(position(), init_radius, color, totalTime);
}

void Door::onCollision(Entity* other) {
	if (goingThroughDoor) {
		return;
	}
	
	if (scene->enemyCount > 0) {
		return;
	}

	if (Player* player = dynamic_cast<Player*>(other)) {
		scene->requestId = id;
		scene->switchLevelInSecond = 1.f;
		player->shrinkAtEndOfLevel();
		
		goingThroughDoor = true;
	}
}

bool Door::handleMouseAndDrawGizmo(DebugMouseCtx* ctx) {
	bool handled = false;

	float w = 10;
	float h = 5;
	float x =  ctx->lens.width / 2 - w;
	float y = -ctx->lens.height / 2;

	fill(0, 0, 0, 220);
	rect(x, y, w, h);

	x += .5;
	y += h - .5;

	noStroke();
	fill(200);
	textSize(.3);

	text("Door", x, y);

	handled |= sliderWithText("%1.2f Radius", &init_radius, 1, 8, x, y - (.5) * 1, 5);

	init_radius = roundToHalf(init_radius);

	return handled;
}
