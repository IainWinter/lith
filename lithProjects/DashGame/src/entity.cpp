#include "entity.h"
#include "lith/sketchapi.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_contact.h"

void SceneCollisionCallback::BeginContact(b2Contact* contact) {
	Entity* a = (Entity*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	Entity* b = (Entity*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

	a->onCollision(b);
	b->onCollision(a);
}

void SceneCollisionCallback::EndContact(b2Contact* contact) {
	Entity* a = (Entity*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	Entity* b = (Entity*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

	a->onCollisionEnd(b);
	b->onCollisionEnd(a);
}

Entity::Entity() {
	body = nullptr;
	scene = nullptr;
}

void Entity::removeFromWorld() {
	if (body) {
		scene->removeBody(body);
	}
}

b2Body* Entity::getBody() {
	return body;
}

vec2 Entity::position() const {
	return vec2(body->GetPosition().x, body->GetPosition().y);
}

void Entity::setVelocity(vec2 velocity) {
	body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
}

void Entity::remove() {
	scene->remove(this);
}

void Entity::setScene(Scene* scene) {
	this->scene = scene;
}

Scene::Scene() {
	collisionCallback = new SceneCollisionCallback();

	world = new b2World(b2Vec2(0.f, 0.f));
	world->SetContactListener(collisionCallback);

	gameOver = false;
	id = 0;
	requestId = 0;
	enemyCount = 0;
}

Scene::~Scene() {
	delete world;
	delete collisionCallback;
}

void Scene::add(Entity* entity) {
	addAfterUpdate.push_back(entity);
}

void Scene::remove(Entity* entity) {
	removeAfterUpdate.insert(entity);
}

b2Body* Scene::addBody(const b2BodyDef& def) {
	return world->CreateBody(&def);
}

void Scene::removeBody(b2Body* body) {
	world->DestroyBody(body);
}

std::pair<Entity*, vec2> Scene::getEntityAt(vec2 pos) const {
	for (Entity* entity : entities) {
		if (!entity->getBody()) {
			continue;
		}

		bool clicked = entity->getBody()->GetFixtureList()->TestPoint(b2Vec2(pos.x, pos.y));
		if (clicked) {
			b2Vec2 center = entity->getBody()->GetWorldCenter();
			vec2 delta = pos - vec2(center.x, center.y);

			return { entity, delta };
		}
	}

	return { nullptr, vec2(0.f) };
}

void Scene::update() {
	for (Entity* entity : entities) {
		entity->update();
	}

	world->Step(deltaTime, 8, 3);
}

void Scene::afterUpdate() {
	for (Entity* entity : removeAfterUpdate) {
		// clean up entity
		entity->removeFromWorld();
		delete entity;

		// pop erase from entities list
		auto itr = std::find(entities.begin(), entities.end(), entity);
		*itr = entities.back();
		entities.pop_back();
	}
	removeAfterUpdate.clear();

	for (Entity* entity : addAfterUpdate) {
		entity->setScene(this);
		entity->addToWorld();
		entities.push_back(entity);
	}
	addAfterUpdate.clear();
}

void Scene::draw() {
	for (Entity* entity : entities) {
		entity->draw();
	}
}

void Scene::drawFixtures() {
	stroke(100, 255, 100);
	for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
		vec2 pos = vec2(body->GetPosition().x, body->GetPosition().y);

		for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
			b2Shape* shape = body->GetFixtureList()->GetShape();

			switch (shape->GetType()) {
				case b2Shape::e_circle: {
					b2CircleShape* circle = (b2CircleShape*)shape;

					int segments = 20;
					float deltaA = w2PI / segments;
					for (int i = 0; i < segments; i++) {
						int j = (i + 1) % segments;

						vec2 a = on_unit(i * deltaA) * circle->m_radius + pos;
						vec2 b = on_unit(j * deltaA) * circle->m_radius + pos;

						line(a, b);
					}
					break;
				}

				case b2Shape::e_polygon: {
					b2PolygonShape* polygon = (b2PolygonShape*)shape;
					
					for (int i = 0; i < polygon->m_count; i++) {
						int j = (i + 1) % polygon->m_count;

						vec2 a = vec2(polygon->m_vertices[i].x, polygon->m_vertices[i].y) + pos;
						vec2 b = vec2(polygon->m_vertices[j].x, polygon->m_vertices[j].y) + pos;

						line(a.x, a.y, b.x, b.y);
					}
					break;
				}
			}
		}
	}
}