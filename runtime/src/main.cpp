//"args": ["${workspaceFolder}/Test/Test.lithproj"],

#include "gl/glad.h"

#include "lith/clock.h"
#include "lith/timer.h"
#include "lith/job.h"
#include "lith/ui.h"

#include "Project.h"

#include "SketchPlugin.h"
#include "SketchRenderBackend.h"
#include "SDLWindow.h"
#include "SDLMixerAudioBackend.h"
#include "printfLogger.h"
#include "msdfgenFontGenerator.h"

#include <cstring>

static SketchPlugin s_plugin;
static AppContext s_app;

static EventPipe s_events;
static SDLMixerAudioBackend s_audio;
static SketchRenderBackend s_render;
static InputMap s_input;
static SDLWindow s_window;
static JobExecutor s_job;
static printfLogger s_log;
static msdfgenFontGenerator s_fontGenerator;

static UIContext s_ui;

static std::atomic<bool> s_compiling = false;
static bool running = true;

void compileSketch(Job job) {
	lithLog("Building sketch...");
	s_compiling = true;

	// FILE* file = _popen(GetBuildProjectLibraryCommand(s_plugin.getProject()).c_str(), "r");

	// char buffer[1024];
	// while (fgets(buffer, sizeof(buffer), file) != nullptr) {
	// 	lithLogNoNewline(buffer);
	// }

	// _pclose(file);

	s_compiling = false;
	lithLog("Done");
}

void sketchPluginEventHandler(const lithEvent& event) {
	switch (event.type) {
		case lithExit: {
			running = false;
			break;
		}

		case lithWindowResize: {
			s_input.SetViewportBounds(vec2(0, 0), vec2(event.windowResize.width, event.windowResize.height));
			s_render.setViewport(event.windowResize.width, event.windowResize.height);
			s_window.setWindowSize(event.windowResize.width, event.windowResize.height);
			break;
		}
		
		case lithWindowVSync: {
			s_window.setVerticalSync(event.windowVSync.interval);
			break;
		}

		case lithWindowTitle: {
			s_window.setTitle(event.windowTitle.title);
			break;
		}

		case lithRecompilePlugin: {
			lithLog("Attempting to recompile");

			if (!s_compiling) {
				JobTree& tree = s_job.CreateTree();
				tree.Create(compileSketch);
				s_job.Run(tree);
			}

			break;
		}
		default:
			break;
	}
}

void sendInputEvent(InputCode code, float state) {
	s_input.SetState(code, state);

	for (const InputName& name : s_input.GetMapping(code)) {
		lithEvent event = {};
		event.type = lithInput;
		event.input.name = name;
		event.input.state = state;

		s_events.in.push_back(event);
	}
}

void inputEventHandler(const lithEvent& event) {
	switch (event.type) {
		case lithKey: {
			sendInputEvent(event.key.keycode, event.key.state ? 1.f : 0.f);
			break;
		}
		case lithMouse: {
			switch (event.mouse.mousecode) {
				case MOUSE_LEFT:
				case MOUSE_MIDDLE:
				case MOUSE_RIGHT:
				case MOUSE_X1:
				case MOUSE_X2: { 
					sendInputEvent(event.mouse.mousecode,
						   event.mouse.button_left
						|| event.mouse.button_middle
						|| event.mouse.button_right
						|| event.mouse.button_x1
						|| event.mouse.button_x2
					);

					break;
				}

				case MOUSE_VEL_POS: {
					sendInputEvent(MOUSE_POS_X, event.mouse.screen_x);
					sendInputEvent(MOUSE_POS_Y, event.mouse.screen_y);
					sendInputEvent(MOUSE_VEL_X, event.mouse.vel_x);
					sendInputEvent(MOUSE_VEL_Y, event.mouse.vel_y);

					break;
				}
            
				case MOUSE_VEL_WHEEL: {
					sendInputEvent(MOUSE_VEL_WHEEL_X, event.mouse.vel_x);
					sendInputEvent(MOUSE_VEL_WHEEL_Y, event.mouse.vel_y);

					break;
				}

				default:
					break;
			}
			break;
		}
		case lithController: {
			sendInputEvent(event.controller.input, event.controller.value);
			break;
		}
		default:
			break;
	}
}

int main(int argc, char *argv[]) {
	// Register interfaces
	registerLoggerInterface(&s_log);
	registerAudioBackendInterface(&s_audio);
	registerUIContext(&s_ui);
	registerFontGeneratorInterface(&s_fontGenerator);

	if (argc >= 3 && strcmp(argv[1], "create") == 0) {
		CreateProject(argv[2]);
		return 0;
	}

	if (argc >= 3 && strcmp(argv[1], "repair") == 0) {
		RepairProject(argv[2]);
		return 0;
	}

	// Force init the project
	if (argc >= 3 && strcmp(argv[1], "init") == 0) {
		Project project = GetProject(argv[2]);
		if (project.failedToLoad) {
			lithLog("Failed to load project. File not found: {}", argv[2]);
			return 1;
		}

		SetupProject(project);
		return 0;
	}

	if (argc == 1) {
		lithLog("To run a project, provide the .lithproj file as the second argument");
		return 0;
	}

	Project project = GetProject(argv[1]);

	if (project.failedToLoad) {
		lithLog("Failed to load project. File not found: {}", argv[1]);
		return 1;
	}

	lithLog("Init project");
	SetupProjectOnce(project); // init if this is the first time running a project

	lithLog("Compile project");
	CompileProject(project);

	lithLog("Built project");

	initSDL();
	lithLog("Init SDL2");

	s_window.create();
	lithLog("Open window");
	s_audio.create();
	lithLog("Open audio");

	auto [windowWidth, windowHeight] = s_window.getSize();

	CameraLens lens = lens_Orthographic(windowHeight, windowWidth / (float)windowHeight, -10, 10);
	lens.position = vec3(lens.ScreenSize()/2.f, 0);

	s_render.create();
	s_render.setPixelDensity(s_window.getPixelDesity());
	s_render.setViewport(windowWidth, windowHeight);
	s_render.setCamera(lens);

	s_app.running = true;
	s_app.input = &s_input;
	s_app.audio = &s_audio;
	s_app.render = &s_render;
	s_app.events = &s_events;
	s_app.window = &s_window;
	s_app.logger = &s_log;
	s_app.fontGenerator = &s_fontGenerator;
	s_app.ui = &s_ui;

	s_plugin = SketchPlugin(project);
	s_plugin.create(&s_app);

	Font defaultFont;
	defaultFont
		.source("C:/Windows/Fonts/seguisb.ttf")
		.scale(32)
		.generate()
		.upload();

	s_plugin.getContext()->font = &defaultFont;

	lithUpdateTime();

	while (running) {
		s_window.pollEvents(&s_events);
		s_input.UpdateStates(lithDeltaTime());

		for (lithEvent& e : s_events.in) {
			inputEventHandler(e);

			switch (e.type) {
				case lithExit:
					running = false;
					break;
				default:
					break;
			}
		}

		lithUpdateTime();

		glClearColor(.06, .06, .06, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_plugin.update();
		s_plugin.handleEventsOut(sketchPluginEventHandler);

		const CameraLens& c = s_render.getCamera();
		vec2 rootPosition = vec2(-c.height / 2 * c.aspect, c.height / 2) + vec2(c.position);
		float pt = s_render.getCamera().height / s_render.getViewportSize().second;

		s_render.text(rootPosition, 12 * pt, {TextAlignLeft, TextAlignTop}, defaultFont, s_log.getLines());

		s_render.draw();
		s_render.clear();

		s_window.swapBuffers();
		s_log.removeOldLogs(lithDeltaTime());
	}

	s_plugin.free();
	s_audio.free();

	return 0;
}