#pragma once

#include "lith/context.h"
#include "lith/log.h"

// the public api should match processing as close as possible

extern float mouseX;
extern float mouseY;

extern float pmouseX;
extern float pmouseY;

extern bool mousePressed;
extern bool mousePressedOnce;
extern int keyCode;
extern int keyCodeOnce;
extern bool keyDown[];

extern float width;
extern float height;

extern float deltaTime;
extern float totalTime;

extern void setup();
extern void draw();

void size(int width, int height);
void title(const char* title);
void vsync(bool enabled);
void exit();

void noLoop();
void loop();
void fps(int framelimit);

float millis();

void camera(const CameraLens& lens);

void background(int rgb);
void background(int r, int g, int b);

void strokeWeight(float weight);

void noStroke();
void stroke(float rgb, float a = 255);
void stroke(float r, float g, float b, float a = 255);

void noFill();
void fill(float rgb, float a = 255);
void fill(float r, float g, float b, float a = 255);

void textFont(const Font& font);
void textSize(float size);
void textAlign(TextAlign alignX, TextAlign alignY = TextAlignBaseline);

void line(float x1, float y1, float x2, float y2);
void line(float x1, float y1, float z1, float x2, float y2, float z2);
void line(vec2 start, vec2 end);
void line(vec3 start, vec3 end);

void rect(float x, float y, float width, float height);
void rect(vec2 base, vec2 size);

void sprite(const TextureInterface& texture, float x, float y, float width, float height);

void text(const std::string& text, float x, float y);

void playSound(Audio& audio);
void playMusic(Audio& audio);

vec2 axis(const InputName& name);
float state(const InputName& name);
bool button(const InputName& name);
bool once(const InputName& name);

void trapMouse();
void releaseMouse();

InputAxisBuilder createAxis(const InputName& name);
AxisGroupBuilder createAxisGroup(const InputName& name);

void __setContext(SketchContext* ctx, AppContext* app);
bool __nextFrame();
void __endFrame();