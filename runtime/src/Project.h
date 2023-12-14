#pragma once

#include <string>

struct Project {
	std::string name;
	std::string folder;
	std::string outputFile;

	bool failedToLoad = false;
};

void CreateProject(const std::string& projectFilePath);

void RepairProject(const std::string& projectFilePath);

Project GetProject(const std::string& projectFilePath);

void SetupProject(const Project& project);
void SetupProjectOnce(const Project& project);
void CompileProject(const Project& project);