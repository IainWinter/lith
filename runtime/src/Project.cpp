#include "Project.h"
#include <filesystem>
#include <fstream>
#include <cstdio>

#include "fmt/core.h"
#include "json.hpp"

#include "lith/log.h"

static const char* templateBuildScript = R"(project('{}', 'c', 'cpp', default_options: ['cpp_std=c++20'])

sources = [
	'../src/sketch.cpp'
]

include = include_directories(['../src'])

deps = [
	dependency('lith'),
	dependency('glm')
]

shared_library('{}', sources, include_directories: include, dependencies: deps))";

static const char* templateSketch = R"(#include "lith/sketch.h"

void setup() {
	title("Welcome to lith");
	size(480, 480);
}

void draw() {
	background(23);
	stroke(245);
	line(width/2, height/2, mouseX, mouseY);
}
)";

static const char* templateProjectFile = R"({{
	"name": "{}"
}})";

// static const char* templatevsCodeCppProps = R"({{
//     "configurations": [
//         {{
//             "name": "{}",
//             "includePath": [
//                 "${{default}}",
//                 ".lith/subprojects/lith/include"
//             ]
//         }}
//     ]
// }}
// )";

using namespace std::filesystem;

void CreateProject(const std::string& projectFilePath) {
	path root = projectFilePath;
	path outerFolder = root.parent_path();
	std::string name = root.stem().string();

	if (exists(root)) {
		lithLog("Cannot create project. {} already exists", root.string());
		return;
	}

	// Create lith project file (for runtime)
	// do this first so if it errors on an invalid project name (path) it exists without
	// much damage
	create_directories(root);
	{
		std::ofstream lithProjectFile(root / (name + ".lithproj"));
		lithProjectFile << fmt::format(fmt::runtime(templateProjectFile), name);
	}

	// this should be provided, and is not needed if pkg-config exists
	//path frameworkPath = path("C:/dev/lith/framework");

	// Create .lith folder and its contents (build stuff)
	create_directories(root / ".lith");
	create_directories(root / ".lith" / "subprojects");
	//create_directory_symlink(frameworkPath, root / ".lith" / "subprojects" / "lith");
	{
		std::ofstream mesonBuildFile(root / ".lith" / "meson.build");
		mesonBuildFile << fmt::format(fmt::runtime(templateBuildScript), name, name);
	}

	// Create src folder and its contents (default sketch file)
	create_directories(root / "src");
	{
		std::ofstream sketchFile(root / "src" / "sketch.cpp");
		sketchFile << templateSketch;
	}
}

void RepairProject(const std::string& projectFilePath) {
	path root = projectFilePath;
	std::string name = root.stem().string();

	create_directories(root / ".lith");
	create_directories(root / ".lith" / "subprojects");
	{
		std::ofstream mesonBuildFile(root / ".lith" / "meson.build");
		mesonBuildFile << fmt::format(fmt::runtime(templateBuildScript), name, name);
	}
}

Project GetProject(const std::string& projectFilePath) {
	if (!exists(projectFilePath)) {
		Project failed;
		failed.failedToLoad = true;
		return failed;
	}

	path folder = path(projectFilePath).parent_path();
	std::string name;

	// read name form file
	// also basically works as a way to validate that this is a .lithproj
	{
		std::ifstream f(projectFilePath);
		if (!f.is_open()) {
			Project failed;
			failed.failedToLoad = true;
			return failed;
		}

		nlohmann::json projectFileData = nlohmann::json::parse(f);
		name = projectFileData["name"].get<std::string>();
	}

	Project project;
	project.name = name;
	project.folder = folder.string();
	project.outputFile = (folder / ".lith" / "build" / (name + ".dll")).string();

	return project;
}

void SetupProject(const Project& project) {
	current_path(path(project.folder) / ".lith");
	system("meson setup build");
}

void SetupProjectOnce(const Project& project) {
	if (!exists(path(project.folder) / ".lith" / "build")) {
		SetupProject(project);
	}
}

void CompileProject(const Project& project) {
	current_path(path(project.folder) / ".lith");
	system("meson compile -C build");
}
