#pragma once

#include "lith/window.h"

// Some free functions which call SDL with settings which are the runtime needs
void initSDL();

struct SDL_Window;

// This window uses SDL and OpenGL for drawing
class SDLWindow : public WindowInterface {
public:
	void create() override;
	void free() override;

	void swapBuffers() override;
	void makeCurrent() override;

	void pollEvents(EventPipe* events) override;

	void setTitle(const char* name) override;
	void setWindowSize(int width, int height) override;
	void setVerticalSync(bool enabled) override;
	void setMouseTrapped(bool trapped) override;

	std::pair<int, int> getSize() const override;
	int getPixelDesity() const override;
	void* getGraphicsAPILoaderFunction() const override;

private:
	SDL_Window* m_window;

	// For now, just make this static, could be put into its own interface?
	static void* s_opengl;
};

//class RenderingContext {
//public:
//	void create(WindowInterface* window);
//	void free();
//
//private:
//	void* m_opengl;
//};