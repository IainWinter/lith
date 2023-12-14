#include "SketchPlugin.h"
#include "lith/log.h"
#include "lith/clock.h"

// required in the host only and before including cr.h
#define CR_HOST
#include "lith/cr.h"

#include "SDL_video.h"

SketchPlugin::SketchPlugin()
	: m_sketch  (nullptr)
	, m_context (nullptr)
	, m_plugin  (nullptr)
{}

SketchPlugin::SketchPlugin(const Project& project)
	: m_project (project)
	, m_sketch  (nullptr)
	, m_context (nullptr)
	, m_plugin  (nullptr)
{}

void SketchPlugin::create(AppContext* app) {
	m_sketch = new SketchContext();
	m_context = new PluginContext();
	m_plugin = new cr_plugin();

	m_context->data = m_sketch;
	m_context->app = app;

	m_plugin->userdata = (void*)m_context;

	bool loaded = cr_plugin_open(*m_plugin, m_project.outputFile.c_str());

	if (!loaded) {
		lithLog("Failed to load project '{}'", m_project.name);
		// may want to delete the context
	}
}

void SketchPlugin::free() {
	if (!isLoaded()) {
		// prevent double free only because we need to deref m_plugin
		return;
	}

	cr_plugin_close(*m_plugin);

	delete m_sketch;
	delete m_context;
	delete m_plugin;
}

void SketchPlugin::update() {
	m_sketch->deltaTime = lithDeltaTime();
	m_sketch->totalTime = lithTotalTime();

	cr_plugin_update(*m_plugin);
}

void SketchPlugin::sendEventIn(const lithEvent& event) {
	m_context->app->events->in.push_back(event);
}

void SketchPlugin::handleEventsOut(lithEventHandler handler) {
	for (const lithEvent& event : m_context->app->events->out) {
		handler(event);
	}

	m_context->app->events->out.clear();
}

SketchContext* SketchPlugin::getContext() {
	return m_sketch;
}

const Project& SketchPlugin::getProject() const {
	return m_project;
}

bool SketchPlugin::isLoaded() const {
	return !!m_sketch;
}
