#pragma once

#include "lith/sketchapi.h"
#include "lith/cr.h"

// This function needs to be compiled in the sketch DLL, so
// keep the definition in the header!!!

CR_EXPORT int cr_main(cr_plugin* plugin, cr_op operation) {
    PluginContext* context = (PluginContext*)plugin->userdata;
    SketchContext* sketch = (SketchContext*)context->data;

    switch (operation) {
        case CR_LOAD: 
            // This HAS to be done first because it connects the logger
            __setContext(sketch, context->app);
            setup();

            lithLog("Sketch loaded");
            break;

        case CR_STEP: 
            if (__nextFrame()) {
                draw();
                __endFrame();
            }

            break;

        case CR_UNLOAD:
            lithLog("Sketch unloaded");
            break;

        case CR_CLOSE:
            lithLog("Sketch closed");
            break;
    }

    return 0;
}