#pragma once

#include <utility>
#include "lith/event.h"

class WindowInterface {
public:
	virtual void create() = 0;
	virtual void free() = 0;

	virtual void swapBuffers() = 0;
	virtual void makeCurrent() = 0;

	virtual void pollEvents(EventPipe* events) = 0;

	virtual void setTitle(const char* name) = 0;
	virtual void setWindowSize(int width, int height) = 0;
	virtual void setVerticalSync(bool enabled) = 0;
	virtual void setMouseTrapped(bool trapped) = 0;

	virtual std::pair<int, int> getSize() const = 0;
	virtual int getPixelDesity() const = 0;
	virtual void* getGraphicsAPILoaderFunction() const = 0;
};