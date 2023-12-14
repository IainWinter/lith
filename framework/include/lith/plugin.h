#pragma once

#include "lith/event.h"

struct AppContext;

class PluginInterface {
public:
	virtual void create(AppContext* app) = 0;
	virtual void free() = 0;
	virtual void update() = 0;

	virtual void sendEventIn(const lithEvent& event) = 0;
	virtual void handleEventsOut(lithEventHandler handler) = 0;
};