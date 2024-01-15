#include "lith/mesh.h"
#include "lith/shader.h"
#include "lith/ui.h"
#include "lith/target.h"
#include "lith/quad.h"
#include "lith/input.h"
#include "lith/sketchapi.h"

#include "global.h"

#include "entity/Player.h"
#include "entity/Bullet.h"
#include "entity/DeathDome.h"
#include "entity/Enemy.h"
#include "entity/Wall.h"
#include "entity/Door.h"

#include "content/levels.h"

struct GameContext {
	Scene* scene;
	CameraLens lens;
	DebugMouseCtx ctx;
	bool pause = false;

	BulletRenderer bulletRenderer;

	int currentLevelId = 1;

	std::unordered_map<Scene*, TextureInterface*> thumbnailTextures;

	GameContext() {
		lens = lens_Orthographic(20, 1, -10.f, 10.f);
	}

	~GameContext() {
		bulletRenderer.free();
	}

	void setup() {
		registerBulletRenderer(&bulletRenderer);
		bulletRenderer.create();

		// preload everything
		addAllScenes();

		// for level designer, should allow to rerun
		renderAllThumbnails();
	}

	void setLevel(int id) {
		scene = getScene(id);
		currentLevelId = id;
	}

	void update() {
		if (once("Pause")) {
			pause = !pause;
		}

		if (once("Reset")) {
			scene = reloadScene(currentLevelId);
		}

		if (scene->id != scene->requestId) {
			scene->switchLevelInSecond -= deltaTime;
			if (scene->switchLevelInSecond < 0.f) {
				setLevel(scene->requestId);
			}
		}

		if (!pause && !ctx.selected) {
			scene->update();
			scene->afterUpdate();
		}
	}

	void draw() {
		useScreenTarget();

		// maintain a 16/9 aspect ratio at all times
		lens.aspect = 16.f / 9.f;
		lens.width = lens.height * lens.aspect;
		camera(lens);

		scene->draw();
		
		bulletRenderer.draw(lens.GetViewMatrix(), lens.GetProjectionMatrix());
		bulletRenderer.clear();

		if (scene->gameOver) {
			textAlign(TextAlignCenter, TextAlignCenter);
			textSize(2);
			fill(22, 22, 22, 200);
			rect(-lens.width + lens.position.x, -lens.height + lens.position.y, lens.width * 2, lens.height * 2);
			text("Game Over", 0, 0);
		}
	}

	void debug() {
		ctx.mouse = vec2(mouseX, mouseY);
		ctx.clicked = mousePressed;
		ctx.clickedOnce = mousePressedOnce;
		ctx.lens = lens;

		if (button("Force Debug Draw")) {
			debugDrawGrid();
			scene->drawFixtures();

			// set these so entity handlers dont fire
			ctx.clicked = false;
			ctx.clickedOnce = false;

			for (Entity* entity : scene->entities) {
				entity->handleMouseAndDrawGizmo(&ctx);
			}
		}

		if (ctx.selected) {
			debugDrawGrid();
			scene->drawFixtures();
			
			bool dragHandled = ctx.selected->handleMouseAndDrawGizmo(&ctx);

			if (!dragHandled) {
				// deselect / select other
				if (ctx.clickedOnce) {
					auto [entity, delta] = scene->getEntityAt(ctx.mouse);
					ctx.selected = entity;
					ctx.delta = delta;
				}

				// move object
				else if (ctx.clicked) {
					b2Body* body = ctx.selected->getBody();
					vec2 dragPos = roundToHalf(ctx.mouse - ctx.delta);
					body->SetTransform(b2Vec2(dragPos.x, dragPos.y), body->GetAngle());

					// when dragging draw some rects reflected over the axes

					vec2 pos = ctx.selected->position();

					fill(200, 200, 200, 100);
					
					rect( pos.x - .5, -pos.y - .5, 1, 1);
					rect(-pos.x - .5,  pos.y - .5, 1, 1);
					rect(-pos.x - .5, -pos.y - .5, 1, 1);
				}
			}

			if (once("Delete")) {
				ctx.selected->remove();
				ctx.selected = nullptr;
			}
		}

		else if (ctx.clickedOnce) {
			auto [entity, delta] = scene->getEntityAt(ctx.mouse);
			ctx.selected = entity;
			ctx.delta = delta;
		}

		if (once("Save Level")) {
			std::string text = createTextFromLevel(scene);
			print("{}", text);
		}

		if (once("Create Player")) {
			scene->add(new Player(vec2(0, 0)));
		}

		if (once("Create Wall")) {
			scene->add(new Wall(vec2(0, 0), vec2(1, 1)));
		}

		if (once("Create Spinner")) {
			scene->add(new SpinnerEnemy(vec2(0, 0), {}));
		}

		if (once("Create Burst")) {
			scene->add(new BurstEnemy(vec2(0, 0), {}));
		}

		if (once("Create Door")) {
			scene->add(new Door(vec2(0, 0), {}));
		}

		scene->afterUpdate();
	}

	void debugDrawGrid() {
		int w = ceil(lens.width);
		int h = ceil(lens.height);

		for (int x = -w; x < w; x++) {
			stroke(x == 0 ? 200 : 100, 100, 100);
			line(x, -h, x, h);
		}

		for (int y = -h; y < h; y++) {
			stroke(100, y == 0 ? 200 : 100, 100);
			line(-w, y, w, y);
		}
	}

	void renderAllThumbnails() {
		const int thumbnailHeight = 128;
		const int thumbnailWidth = thumbnailHeight * 16 / 9.f;

		Target thumbnailTarget = TargetBuilder()
			.size(thumbnailWidth, thumbnailHeight)
			.attach(TargetAttachmentDepth, TextureFormatDepth)
			.attach(TargetAttachmentColor0, nullptr)
			.build();

		for (auto& [id, scene] : getAllScenes()) {
			Texture* thumbnail = new Texture();
			thumbnail->source(TextureFormatRGBA, thumbnailWidth, thumbnailHeight);
			thumbnail->upload();

			thumbnailTextures[scene] = thumbnail;

			thumbnailTarget.swapAttachment(TargetAttachmentColor0, thumbnail);
			thumbnailTarget.upload();
			thumbnailTarget.use();

			scene->draw();
			bulletRenderer.draw(lens.GetViewMatrix(), lens.GetProjectionMatrix());
			bulletRenderer.clear();
		}
	}
};

GameContext game;

void setup() {
	size(1280, 720);
	background(5);

	createAxis("Player Movement")
		.Map(Key_Up,    vec2( 0 , 1))
		.Map(Key_Down,  vec2( 0, -1))
		.Map(Key_Left,  vec2(-1,  0))
		.Map(Key_Right, vec2( 1,  0))
		.LimitToUnit();

	createAxis("Player Dash").MapButton(Key_Space);
	createAxis("Reset").MapButton(Key_R);
	createAxis("Save Level").MapButton(Key_Q);
	createAxis("Pause").MapButton(Key_P);
	createAxis("Clear").MapButton(Key_C);
	createAxis("Create Player").MapButton(Key_1);
	createAxis("Create Wall").MapButton(Key_2);
	createAxis("Create Spinner").MapButton(Key_3);
	createAxis("Create Burst").MapButton(Key_4);
	createAxis("Create Door").MapButton(Key_5);
	createAxis("Delete").MapButton(Key_Delete);
	createAxis("Force Debug Draw").MapButton(Key_D);

	game.setup();
	game.setLevel(0);
}

void draw() {
	game.update();
	game.draw();
	game.debug();
}