#include "entity/Player.h"
#include "entity/Enemy.h"
#include "entity/DeathDome.h"
#include "entity/Bullet.h"

#include "lith/sketchapi.h"
#include "box2d/b2_polygon_shape.h"

Player::Player(vec2 pos) {
	init_pos = pos;
	init_size = .8f;

	size = init_size;
}

void Player::addToWorld() {
	b2BodyDef def;
	def.position = b2Vec2(init_pos.x, init_pos.y);
	def.type = b2_dynamicBody;
	def.bullet = true;
	def.userData.pointer = (uintptr_t)this;

	b2PolygonShape shape;
	shape.SetAsBox(init_size / 2, init_size / 2);

	body = scene->addBody(def);
	body->CreateFixture(&shape, 1.f);
}

void Player::update() {
	if (button("Player Dash") && dashTimer <= -dashCooldownTime) {
		dashTimer = dashTime;
		print("DASH");
	}

	dashTimer -= deltaTime;

	vec2 velocity = (speed + dashSpeed * max(0.f, dashTimer / dashTime)) * axis("Player Movement");
	setVelocity(velocity);

	if (shrinking) {
		size = lerpf(init_size, sizeGoal, clamp(shrinkLifetime, 0.f, 1.f));
		shrinkLifetime += deltaTime * 1.5f;
	}

	auto itr = touching.begin();
	while (itr != touching.end()) {
		if (dashTimer <= 0) {
			++itr;
		}

		else {
			Entity* e = *itr;

			// spawn death dome
			vec2 pos = e->position();
			scene->add(new DeathDome(pos));
			scene->enemyCount -= 1;

			// remove
			e->remove();
			itr = touching.erase(itr);
		}
	}
}

void Player::draw() {
	float c = lerpf(0, 255, health / 3.f);

	noStroke();
	fill(255, c, c);
	rect(position() - size / 2.f, vec2(size));
}

void Player::onCollision(Entity* hit) {
	if (SpinnerEnemy* enemy = dynamic_cast<SpinnerEnemy*>(hit)) {
		touching.insert(enemy);
	}

	else if (Bullet* bullet = dynamic_cast<Bullet*>(hit)) {
		bullet->remove();
		health -= 1;

		if (health < 0) {
			remove();
			scene->gameOver = true;
		}
	}
}

void Player::onCollisionEnd(Entity* hit) {
	if (SpinnerEnemy* enemy = dynamic_cast<SpinnerEnemy*>(hit)) {
		touching.erase(enemy);
	}
}

void Player::shrinkAtEndOfLevel() {
	shrinking = true;
	sizeGoal = 0.f;
}