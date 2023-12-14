#include "printfLogger.h"

printfLogger::printfLogger()
	: isStale(false)
{}

void printfLogger::log(const char* str) {
	printf("%s", str);
	logs.push_back({ str, 3.f });
	isStale = true;

	if (logs.size() > 50) {
		logs.erase(logs.begin());
	}
}

const std::string& printfLogger::getLines() {
	if (isStale) {
		std::stringstream ss;
		for (const auto& [str, _] : logs) {
			ss << str;
		}

		string = ss.str();
		isStale = false;
	}

	return string;
}

void printfLogger::clear() {
	logs = {};
	isStale = true;
}

void printfLogger::removeOldLogs(float deltaTime) {
	for (int i = 0; i < logs.size(); i++) {
		float& life = logs[i].second;
		life -= deltaTime;

		if (life < 0.f) {
			logs.erase(logs.begin() + i);
			i--;

			isStale = true;
		}
	}
}
