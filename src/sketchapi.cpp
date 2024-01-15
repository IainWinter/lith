#include "lith/sketchapi.h"
#include "gl/glad.h"

static SketchContext* sketch;
static AppContext* app;

static int s_keyCodeOnceLast = 0;
static bool s_mousePressedOnceLast = false;
static int s_loop = true;
static float s_loopTime = 0;
static float s_loopTimeAcc = 0;

float mouseX = 0;
float mouseY = 0;
float pmouseX = 0;
float pmouseY = 0;
bool mousePressed = false;
bool mousePressedOnce = false;
int keyCode = 0;
int keyCodeOnce = 0;
bool keyDown[512] = {};

float width = 0;
float height = 0;

float deltaTime = 0;
float totalTime = 0;

void size(int width, int height) {
	lithEvent event = {};
	event.type = lithWindowResize;
	event.windowResize = { width, height };

	app->events->out.push_back(event);

	float heightf = (float)height;

	CameraLens lens = lens_Orthographic(heightf, width / heightf, -10.f, 10.f);
	lens.position = vec3(lens.ScreenSize() / 2.f, 0.f);
	app->render->setCamera(lens);
}

void title(const char* title) {
	lithEvent event = {};
	event.type = lithWindowTitle;
	event.windowTitle = { title };

	app->events->out.push_back(event);
}

void vsync(bool enabled) {
	lithEvent event = {};
	event.type = lithWindowVSync;
	event.windowVSync = { enabled ? 1 : 0 };

	app->events->out.push_back(event);
}

void exit() {
	lithEvent event = {};
	event.type = lithExit;
	event.exit = { 0 };

	app->events->out.push_back(event);
}

void noLoop() {
	s_loop = false;
}

void loop() {
	s_loop = true;
}

void fps(int framelimit) {
	s_loopTime = 1.f / framelimit;
}

float millis() {
	return totalTime / 1000.f;
}

void camera(const CameraLens& lens) {
	app->render->setCamera(lens);
}

void background(int rgb) {
	background(rgb, rgb, rgb);
}

void background(int r, int g, int b) {
	sketch->background = vec4(r, g, b, 255) / 255.f;
}

void strokeWeight(float weight) {
	sketch->strokeThickness = weight;
}

void noStroke() {
	sketch->strokeThickness = 0.f;
	stroke(0, 0, 0, 0);
}

void stroke(float rgb, float a) {
	stroke(rgb, rgb, rgb, a);
}

void stroke(float r, float g, float b, float a) {
	sketch->stroke = vec4(r, g, b, a) / 255.f;
}

void noFill() {
	fill(0, 0, 0, 0);
}

void fill(float rgb, float a) {
	fill(rgb, rgb, rgb, a);
}

void fill(float r, float g, float b, float a) {
	sketch->fill = vec4(r, g, b, a) / 255.f;
}

void textFont(const Font& font) {
	sketch->font = &font;
}

void textSize(float size) {
	sketch->textSize = size;
}

void textAlign(TextAlign alignX, TextAlign alignY) {
	sketch->textConfig.alignX = alignX;
	sketch->textConfig.alignY = alignY;
}

void line(float x1, float y1, float x2, float y2) {
	line(vec3(x1, y1, 0), vec3(x2, y2, 0));
}

void line(float x1, float y1, float z1, float x2, float y2, float z2) {
	line(vec3(x1, y1, z1), vec3(x2, y2, z2));
}

void line(vec2 start, vec2 end) {
	line(vec3(start, 0.f), vec3(end, 0.f));
}

void line(vec3 start, vec3 end) {
	app->render->line(start, end, sketch->stroke);
}

void rect(float x, float y, float width, float height) {
	rect(vec2(x, y), vec2(width, height));
}

void rect(vec2 base, vec2 size) {
	app->render->rect(base, size, 0.f, sketch->fill, sketch->stroke, sketch->strokeThickness);
}

void sprite(const TextureInterface& texture, float x, float y, float width, float height) {
	//sketch->render->sprite(vec2(x, y), vec2(width, height), vec2(0, 0), vec2(1, 1), textureHandle);
}
 
void text(const std::string& text, float x, float y) {
	app->render->text(vec2(x, y), sketch->textSize, sketch->textConfig, *sketch->font, text);
}

void playSound(Audio& audio) {
	audio.play();
}

void playMusic(Audio& audio) {
	audio.stream().play();
}

vec2 axis(const InputName& name) {
	return app->input->GetAxis(name);
}

float state(const InputName& name) {
	return app->input->GetAxis(name).x;
}

bool button(const InputName& name) {
	return app->input->GetButton(name);
}

bool once(const InputName& name) {
	return app->input->GetOnce(name);
}

void trapMouse() {
	app->window->setMouseTrapped(true);
}

void releaseMouse() {
	app->window->setMouseTrapped(false);
}

InputAxisBuilder createAxis(const InputName& name) {
	return app->input->CreateAxis(name);
}

AxisGroupBuilder createAxisGroup(const InputName& name) {
	return app->input->CreateGroupAxis(name);
}

void __setContext(SketchContext* ctx, AppContext* app) {
	sketch = ctx;
	::app = app;

	// quick hack to remove all axes
	*app->input = {};

	// quick hack to remove all ui
	*app->ui = {};

	registerLoggerInterface(app->logger);
	registerAudioBackendInterface(app->audio);
	registerUIContext(app->ui);
	registerFontGeneratorInterface(app->fontGenerator);
	gladLoadGLLoader((GLADloadproc)app->window->getGraphicsAPILoaderFunction());
}

bool __nextFrame() {
	// get events from runtime
	// process these outside of loop so they don't accumulate while paused

	for (const lithEvent& e : app->events->in) {
		switch (e.type) {
			case lithKey: {
				keyDown[e.key.keycode] = e.key.state;

				if (e.key.state) {
					keyCode = e.key.keycode;

					if (s_keyCodeOnceLast != keyCode) {
						s_keyCodeOnceLast = keyCode;
						keyCodeOnce = keyCode;
					}
				}

				else {
					s_keyCodeOnceLast = 0;
				}

				// check if plugin should reload
				// this shouldn't be here
				if (e.key.key == 'r' && e.key.key_alt) {
					lithEvent event = {};
					event.type = lithRecompilePlugin;
					app->events->out.push_back(event);
				}

				break;
			}

			case lithMouse: {
				vec2 worldMouse = app->render->getCamera().ScreenToWorld2D(vec2(e.mouse.screen_x, e.mouse.screen_y));

				mouseX = worldMouse.x;
				mouseY = worldMouse.y;

				//mouseX = e.mouse.pixel_x;
				//mouseY = e.mouse.pixel_y;

				mousePressed = e.mouse.button_left;

				if (s_mousePressedOnceLast != mousePressed) {
					s_mousePressedOnceLast = mousePressed;
					mousePressedOnce = mousePressed;
				}

				break;
			}

			case lithWindowResize: {
				width = (float)e.windowResize.width;
				height = (float)e.windowResize.height;
				app->render->setViewport(width, height);
				break;
			}

			default:
				break;
		}
	}
	// this should go in main loop
	app->events->in.clear();

	if (s_loop) {
		auto [viewportWidth, viewportHeight] = app->render->getViewportSize();

		// sync external state

		width = (float)viewportWidth;
		height = (float)viewportHeight;

		deltaTime = sketch->deltaTime;
		totalTime = sketch->totalTime;

		s_loopTimeAcc += deltaTime;

		if (s_loopTimeAcc >= s_loopTime) {
			s_loopTimeAcc = 0.f;

			return true;
		}
	}

	return false;
}

void __endFrame() {
	pmouseX = mouseX;
	pmouseY = mouseY;
	keyCode = 0;
	keyCodeOnce = 0;
	mousePressedOnce = false;
}