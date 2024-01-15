#pragma once

#include "lith/math.h"
#include "lith/lens.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"

#include <vector>
#include <unordered_set>

class Scene;
class Entity;

struct DebugMouseCtx {
	vec2 mouse;
	bool clicked;
	bool clickedOnce;

	vec2 delta;
	Entity* selected;

	// for entities to store some info about what is being dragged
	int gizmo;
	vec2 gizmoMousePos;
	vec2 gizmoState;

	CameraLens lens;
};

class Entity {
public:
	Entity();
	virtual ~Entity() = default;

	virtual void update() = 0;
	virtual void draw() = 0;

	virtual void addToWorld() = 0;
	virtual void removeFromWorld();

	virtual void onCollision(Entity* other) {}
	virtual void onCollisionEnd(Entity* other) {}

	// Return true if the mouse drag was handled
	virtual bool handleMouseAndDrawGizmo(DebugMouseCtx* ctx) { return false; }

	b2Body* getBody();

	vec2 position() const;
	
	void setVelocity(vec2 velocity);
	void remove();
	void setScene(Scene* scene);

protected:
	b2Body* body;
	Scene* scene;
};

struct SceneCollisionCallback : b2ContactListener {
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

private:
	std::vector<std::pair<Entity*, Entity*>> collisions;
};

class Scene {
public:
	Scene();
	~Scene();

	void add(Entity* entity);
	void remove(Entity* entity);
	
	b2Body* addBody(const b2BodyDef& def);
	void removeBody(b2Body* body);

	std::pair<Entity*, vec2> getEntityAt(vec2 pos) const;

	void update();
	void afterUpdate();
	void draw();

	void drawFixtures();
	void debug(DebugMouseCtx* ctx);

public:
	std::vector<Entity*> entities;
	b2World* world;
	bool gameOver;

	int id;
	int requestId; // ask the game context to switch to this level is != to id

	int enemyCount;

	float switchLevelInSecond;

private:
	std::vector<Entity*> addAfterUpdate;
	std::unordered_set<Entity*> removeAfterUpdate;
	SceneCollisionCallback* collisionCallback;
};