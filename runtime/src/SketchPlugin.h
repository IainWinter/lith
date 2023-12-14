#pragma once

#include "lith/plugin.h"
#include "lith/context.h"

#include "Project.h"

#include <string>
#include <memory>

// forward declare
struct cr_plugin;

class SketchPlugin : public PluginInterface {
public:
	SketchPlugin();
	SketchPlugin(const Project& project);

	void create(AppContext* app) override;
	void free() override;
	void update() override;
	void sendEventIn(const lithEvent& event) override;
	void handleEventsOut(lithEventHandler handler) override;

	SketchContext* getContext();
	const Project& getProject() const;
	bool isLoaded() const;

private:
	Project m_project;

	SketchContext* m_sketch;
	PluginContext* m_context;
	cr_plugin* m_plugin;
};