#include "SDLWindow.h"
#include "SDL.h"
#include "gl/glad.h"

#include "lith/log.h"
#include "lith/clock.h"

void gl_errorMessageCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam
) {
	if (type == GL_DEBUG_TYPE_ERROR) {
		print("GL: {}", message);
	}
}

void* SDLWindow::s_opengl = nullptr;

void initSDL() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
}

void SDLWindow::create() {
	if (!s_opengl) {
		// Set profile, this is from glad gen
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

		// Pick versions
#ifdef __APPLE__
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __linux__
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif _WIN32
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
#endif

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}

	print("set gl attributes");

	m_window = SDL_CreateWindow("lith", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		1280, 
		720, 
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
	);

	print("made window");

	if (!s_opengl) {
		// Init OpenGL using SDL
		s_opengl = SDL_GL_CreateContext(m_window);
		SDL_GL_MakeCurrent(m_window, s_opengl);
		SDL_GL_SetSwapInterval(1);

		gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

		print("created opengl context");
	}

	uintptr_t ptr = (uintptr_t)SDL_GL_GetProcAddress;

	print("SDL_GL_GetProcAddress = {}", ptr);

	int samples;
	glGetIntegerv(GL_SAMPLES, &samples);

	print("got samples {}", samples);

	int b, s;
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &b);
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &s);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_errorMessageCallback, 0);

	print("set error message callback");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	print("window done");
}

void SDLWindow::free() {
	SDL_GL_DeleteContext(s_opengl);
	SDL_DestroyWindow(m_window);
}

void SDLWindow::swapBuffers() {
	SDL_GL_SwapWindow(m_window);
}

void SDLWindow::makeCurrent() {
	SDL_GL_MakeCurrent(m_window, s_opengl);
}

void SDLWindow::pollEvents(EventPipe* events) {
	auto [windowWidth, windowHeight] = getSize();

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: {
				lithEvent e = {};
				e.type = lithExit;
				e.exit.code = 0;

				events->in.push_back(e);
				break;
			}

			case SDL_WINDOWEVENT: {
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						lithEvent e = {};
						e.type = lithWindowResize;
						e.windowResize.width = event.window.data1;
						e.windowResize.height = event.window.data2;

						events->in.push_back(e);
						break;
				}

				break;
			}

			case SDL_MOUSEMOTION: {
				int x = event.motion.x;
				int y = windowHeight - event.motion.y;

				float vel_x = (float) event.motion.xrel;   // I think the deltatime needs to be incorporated here
				float vel_y = (float)-event.motion.yrel;

				lithEvent e = {};
				e.type = lithMouse;
				e.mouse.mousecode = MOUSE_VEL_POS;
				e.mouse.pixel_x = x;
				e.mouse.pixel_y = y;
				e.mouse.screen_x = x / (float)windowWidth;
				e.mouse.screen_y = y / (float)windowHeight;
				e.mouse.vel_x = vel_x;
				e.mouse.vel_y = vel_y;
				e.mouse.button_left = bool(event.motion.state & SDL_BUTTON_LMASK);
				e.mouse.button_middle = bool(event.motion.state & SDL_BUTTON_MMASK);
				e.mouse.button_right = bool(event.motion.state & SDL_BUTTON_RMASK);
				e.mouse.button_x1 = bool(event.motion.state & SDL_BUTTON_X1MASK);
				e.mouse.button_x2 = bool(event.motion.state & SDL_BUTTON_X2MASK);
				e.mouse.button_repeat = event.button.clicks;
				e.mouse.is_wheel = false;

				events->in.push_back(e);
				break;
			}

			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN: {
				int x = event.button.x;
				int y = windowHeight - event.button.y;

				MouseInput mouseInput;

				switch (event.button.button) {
					case SDL_BUTTON_LEFT:   mouseInput = MOUSE_LEFT; break;
					case SDL_BUTTON_MIDDLE: mouseInput = MOUSE_MIDDLE; break;
					case SDL_BUTTON_RIGHT:  mouseInput = MOUSE_RIGHT; break;
					case SDL_BUTTON_X1:     mouseInput = MOUSE_X1; break;
					case SDL_BUTTON_X2:     mouseInput = MOUSE_X2; break;
					default: throw nullptr;
				}

				lithEvent e = {};
				e.type = lithMouse;
				e.mouse.mousecode = mouseInput;
				e.mouse.pixel_x = x;
				e.mouse.pixel_y = y;
				e.mouse.screen_x = x / (float)windowWidth;
				e.mouse.screen_y = y / (float)windowHeight;
				e.mouse.vel_x = 0;
				e.mouse.vel_y = 0;
				e.mouse.button_left = bool(event.button.state & SDL_BUTTON_LMASK);
				e.mouse.button_middle = bool(event.button.state & SDL_BUTTON_MMASK);
				e.mouse.button_right = bool(event.button.state & SDL_BUTTON_RMASK);
				e.mouse.button_x1 = bool(event.button.state & SDL_BUTTON_X1MASK);
				e.mouse.button_x2 = bool(event.button.state & SDL_BUTTON_X2MASK);
				e.mouse.button_repeat = event.button.clicks;
				e.mouse.is_wheel = false;

				events->in.push_back(e);
				break;
			}

			case SDL_MOUSEWHEEL: {
				float flip = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1.f : -1.f;

				lithEvent e = {};
				e.type = lithMouse;
				e.mouse.mousecode = MOUSE_VEL_WHEEL;
				e.mouse.vel_x = event.wheel.preciseX * flip;
				e.mouse.vel_y = event.wheel.preciseY * flip;
				e.mouse.is_wheel = true;

				events->in.push_back(e);
				break;
			}

			case SDL_KEYUP:
			case SDL_KEYDOWN: {
				lithEvent e = {};
				e.type = lithKey;

				e.key.keycode = (KeyboardInput)event.key.keysym.scancode;
				e.key.key = (char)event.key.keysym.sym;
				e.key.state = (bool)event.key.state;
				e.key.repeat = (int)event.key.repeat;
				e.key.key_shift = bool(event.key.keysym.mod & KMOD_SHIFT);
				e.key.key_ctrl = bool(event.key.keysym.mod & KMOD_CTRL);
				e.key.key_alt = bool(event.key.keysym.mod & KMOD_ALT);

				events->in.push_back(e);
				break;
			}
		}
	}
}

void SDLWindow::setTitle(const char* name) {
	SDL_SetWindowTitle(m_window, name);
}

void SDLWindow::setWindowSize(int width, int height) {
	SDL_SetWindowSize(m_window, width, height);
}

void SDLWindow::setVerticalSync(bool enabled) {
	SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

void SDLWindow::setMouseTrapped(bool trapped) {
	SDL_SetRelativeMouseMode(trapped ? SDL_TRUE : SDL_FALSE);
}

std::pair<int, int> SDLWindow::getSize() const {
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);

	return { width, height };
}

int SDLWindow::getPixelDesity() const {
	int drawWidth, drawHeight;
	SDL_GL_GetDrawableSize(m_window, &drawWidth, &drawHeight);

	auto [windowWidth, windowHeight] = getSize();

	// I am assuming that the width and height will have the same ratio
	// DrawableSize will be 2x larger on retina displays

	return drawWidth / windowWidth;
}

void* SDLWindow::getGraphicsAPILoaderFunction() const {
	return (void*)SDL_GL_GetProcAddress;
}