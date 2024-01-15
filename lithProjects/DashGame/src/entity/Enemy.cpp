#include "entity/Enemy.h"
#include "entity/Bullet.h"

#include "global.h"

#include "lith/sketchapi.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"

SpinnerEnemy::SpinnerEnemy(vec2 pos, SpinnerEnemyProps props) {
	init_pos = pos;
	this->props = props;
}

void SpinnerEnemy::addToWorld() {
	b2BodyDef def;
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.type = b2_dynamicBody;
	def.bullet = true;
	def.userData.pointer = (uintptr_t)this;

	b2PolygonShape shape;
	shape.SetAsBox(init_size / 2, init_size / 2);

	b2Filter filter = {};
	filter.groupIndex = -1; // enemy group

	b2FixtureDef fix = {};
	fix.shape = &shape;
	fix.filter = filter;

	body = scene->addBody(def);
	body->CreateFixture(&fix);
} 

void SpinnerEnemy::update() {
	shootTimer -= deltaTime;
	if (shootTimer <= 0) {
		shootTimer = 1.f / props.rotationsPerSecond;

		float shotAngle = (currentShot + props.angleOffset) * w2PI / (float)props.shotsPerRotation * props.direction;
		currentShot = (currentShot + 1) % props.shotsPerRotation;

		vec2 pos = position();
		vec2 vel = on_unit(shotAngle) * 5.f;

		scene->add(new Bullet(pos + normalize(vel), vel));
	}
}

void SpinnerEnemy::draw() {
	noStroke();
	fill(255, 100, 100);
	rect(position() - init_size / 2.f, vec2(init_size));
}

bool SpinnerEnemy::handleMouseAndDrawGizmo(DebugMouseCtx* ctx) {
	float angle = 0.f;

	stroke(200);

	int shots = props.shotsPerRotation;
	for (int i = 0; i < props.shotsPerRotation; i++) {
		float angle = (i + props.angleOffset) * w2PI / (float)props.shotsPerRotation * props.direction;
		vec2 dir = on_unit(angle);
		line(position(), position() + dir * 100.f);
	}

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
	textSize(.3f);

	text("Spinner Enemy", x, y);

	bool handled = false;

	handled |= sliderWithText   ("%1.2f Rotation Direction", &props.direction,          -1,   1, x,  y - (.5) * 1, 5);
	handled |= sliderWithText   ("%1.1f Shot offset",        &props.angleOffset,         0,   1, x,  y - (.5) * 2, 5);
	handled |= sliderWithText   ("%1.2f rotations/s",        &props.rotationsPerSecond,  0,  20, x,  y - (.5) * 3, 5);
	handled |= sliderWithTextInt("%d shots/rotations",       &props.shotsPerRotation,    1, 100, x,  y - (.5) * 4, 5);

	props.angleOffset = roundToHalf(props.angleOffset);

	return handled;
}



BurstEnemy::BurstEnemy(vec2 pos, BurstEnemyProps props) {
	init_pos = pos;
	this->props = props;
}

void BurstEnemy::addToWorld() {
	b2BodyDef def;
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.type = b2_dynamicBody;
	def.bullet = true;
	def.userData.pointer = (uintptr_t)this;

	b2PolygonShape shape;
	shape.SetAsBox(init_size / 2, init_size / 2);

	b2Filter filter = {};
	filter.groupIndex = -1; // enemy group

	b2FixtureDef fix = {};
	fix.shape = &shape;
	fix.filter = filter;

	body = scene->addBody(def);
	body->CreateFixture(&fix);
}

void BurstEnemy::update() {
	burstTimer -= deltaTime;

	if (burstTimer <= props.burstCooldown) {
		// animate something
	}

	if (burstTimer <= 0.f) {
		burstTimer = props.burstTime + props.burstCooldown;

		for (float angle = 0.f; angle < w2PI; angle += w2PI / props.numberOfShots) {
			vec2 pos = position();
			vec2 vel = on_unit(angle) * 5.f;

			scene->add(new Bullet(pos + normalize(vel), vel));

			vec2 dir = on_unit(angle + props.angleOffset);
			line(position(), position() + dir * 100.f);
		}
	}
}

void BurstEnemy::draw() {
	noStroke();
	fill(255, 100, 100);
	rect(position() - init_size / 2.f, vec2(init_size));
}

bool BurstEnemy::handleMouseAndDrawGizmo(DebugMouseCtx* ctx) {
	float angle = 0.f;

	stroke(200);

	for (float angle = 0.f; angle < w2PI; angle += w2PI / props.numberOfShots) {
		vec2 dir = on_unit(angle + props.angleOffset);
		line(position(), position() + dir * 100.f);
	}

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
	textSize(.3f);

	text("Burst Enemy", x, y);

	bool handled = false;

	handled |= sliderWithText   ("%1.1f Shot offset",     &props.angleOffset,    0,   1, x,  y - (.5) * 1, 5);
	handled |= sliderWithTextInt("%1.2f Shot count",      &props.numberOfShots,  0, 100, x,  y - (.5) * 2, 5);
	handled |= sliderWithText   ("%d Burst Windup",       &props.burstTime,      0,   5, x,  y - (.5) * 3, 5);
	handled |= sliderWithText   ("%1.2f Burst Cooldown",  &props.burstCooldown,  0,   5, x,  y - (.5) * 4, 5);

	props.angleOffset = roundToHalf(props.angleOffset);

	return handled;
}