#pragma once

#include "lith/input.h"

#include <vector>

enum EventType {
	lithWindowResize,
	lithWindowTitle,
	lithWindowVSync,
	lithExit,

	lithRecompilePlugin,

	lithKey,
	lithMouse,
	lithController,
	lithInput
};

struct EventWindowResize {
	int width;
	int height;
};

struct EventWindowTitle {
	const char* title;
};

struct EventWindowVSync {
	int interval;
};

struct EventExit {
	int code;
};

struct EventRecompilePlugin {
};

struct EventKey {
	KeyboardInput keycode;

	char key;

	bool state;
	int repeat;

	bool key_shift;
	bool key_ctrl;
	bool key_alt;
};

struct EventMouse {
	MouseInput mousecode;
	
	int pixel_x, pixel_y;
	float screen_x, screen_y;
	float vel_x, vel_y;

	bool button_left;
	bool button_middle;
	bool button_right;
	bool button_x1;
	bool button_x2;

	int button_repeat;

	// If this is true, this mouse event is from the mouse wheel being scrolled
	// vel_x and vel_y hold the direction of scrolling
	bool is_wheel;
};

struct EventController {
	ControllerInput input;

	float value;
	int controllerId;
};

struct EventInput {
	InputNameRef name;

	union {
		float state; // for 1d inputs (buttons)
		vec2 axis;   // for 2d inputs (axes)
	};

	bool enabled() const { 
		return state == 1.f; 
	}
};

struct lithEvent {
	EventType type;

	~lithEvent() noexcept {}

	union {
		EventWindowResize windowResize;
		EventWindowTitle windowTitle;
		EventWindowVSync windowVSync;
		EventExit exit;
		EventRecompilePlugin recompilePlugin;
		EventKey key;
		EventMouse mouse;
		EventController controller;
		EventInput input;
	};
};

using lithEventHandler = void(*)(const lithEvent&);

struct EventPipe {
	std::vector<lithEvent> out;
	std::vector<lithEvent> in;
};