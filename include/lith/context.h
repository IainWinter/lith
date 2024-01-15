#pragma once

#include "lith/plugin.h"
#include "lith/audio.h"
#include "lith/render.h"
#include "lith/input.h"
#include "lith/window.h"
#include "lith/log.h"
#include "lith/ui.h"
#include "lith/font.h"

struct AppContext {
    bool running;

    // I think that these should be in the plugin so the
    // plugins can choose which to use.
    // For example, a plugin may create another window
    InputMap* input;
    WindowInterface* window;
    RenderBackendInterface* render;
    AudioBackendInterface* audio;
    EventPipe* events;
    LoggerInterface* logger;
    UIContext* ui;
    FontGeneratorInterface* fontGenerator;
};

struct PluginContext {
    // This can be anything but depends on the plugin type
    // Sketches will use SketchContext
    void* data;

    AppContext* app;
};

// Should move to sketch? or another file
struct SketchContext {
    float deltaTime;
    float totalTime;

     // applies to line and rect
 	vec4 stroke = vec4(1);

    // applies to rect
 	vec4 fill = vec4(1);

    // applies to rect
    // todo: apply to line
 	float strokeThickness = 1;
    float strokeThicknessRestore = 1;

    const Font* font; // take the pointer to the ref held
                      // user must keep this alive

    TextMeshGenerationConfig textConfig;

    float textSize = 12;
    TextAlign alignX = TextAlignLeft;
    TextAlign alignY = TextAlignBaseline;

    vec4 background;
};