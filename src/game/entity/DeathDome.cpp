#include "entity/DeathDome.h"
#include "entity/Bullet.h"

#include "lith/sketchapi.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"

#include "global.h"

DeathDome::DeathDome(vec2 pos) {
	init_pos = pos;
	init_radius = 1.f;
	init_alpha = .8f;
	init_lifetime = 1.f;
}

void DeathDome::addToWorld() {
	b2BodyDef def;
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.type = b2_staticBody;
	def.userData.pointer = (uintptr_t)this;

	body = scene->addBody(def);

	setColliderRadius(init_radius);
}

void DeathDome::update() {
	lifetime -= deltaTime;
	if (lifetime < 0.f) {
		remove();
	}

	float ratio = 1 - lifetime / init_lifetime;
	
	radius = lerpf(init_radius, 3.f, ratio);
	alpha = lerpf(init_alpha, 0, ratio);

	setColliderRadius(radius);
}

void DeathDome::draw() {
	g_bulletRenderer->addBullet(position(), radius, vec4(1, 1, 1, alpha), lifetime);
}

void DeathDome::onCollision(Entity* other) {
	if (Bullet* bullet = dynamic_cast<Bullet*>(other)) {
		other->remove();
	}
}

void DeathDome::setColliderRadius(float r) {
	// is there really no way to just scale the circle?

	if (b2Fixture* fix = body->GetFixtureList()) {
		body->DestroyFixture(fix);
	}

	b2CircleShape shape;
	shape.m_radius = r;

	b2FixtureDef fix;
	fix.shape = &shape;
	fix.isSensor = true;
	body->CreateFixture(&fix);
}
