#include "entity/Bullet.h"
#include "entity/Wall.h"

#include "lith/sketchapi.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"

#include "global.h"

Bullet::Bullet(vec2 pos, vec2 vel) {
	init_pos = pos;
	init_vel = vel;
	init_size = .2f;
}

void Bullet::addToWorld() {
	b2BodyDef def = {};
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.type = b2_dynamicBody;
	def.bullet = true;
	def.linearVelocity = b2Vec2(init_vel.x, init_vel.y);
	def.userData.pointer = (uintptr_t)this;

	b2CircleShape shape = {};
	shape.m_radius = init_size;

	b2Filter filter = {};
	filter.groupIndex = -1; // enemy group

	b2FixtureDef fix = {};
	fix.shape = &shape;
	fix.filter = filter;

	body = scene->addBody(def);
	body->CreateFixture(&fix);
}

void Bullet::update() {
	lifetime -= deltaTime;
	if (lifetime <= 0) {
		remove();
	}
}

void Bullet::draw() {
	g_bulletRenderer->addBullet(position(), init_size, vec4(66, 239, 255, 255) / 255.f, lifetime);
}

void Bullet::onCollision(Entity* hit) {
	if (dynamic_cast<Wall*>(hit)) {
		remove();
	}
}