#pragma once

#include "lith/log.h"
#include <sstream>
#include <vector>
#include <utility>

class printfLogger : public LoggerInterface {
public:
	printfLogger();

	void log(const char* str) override;

	const std::string& getLines();
	void clear();

	void removeOldLogs(float deltaTime);

private:
	bool isStale;
	std::string string;
	std::vector<std::pair<std::string, float>> logs;
};
