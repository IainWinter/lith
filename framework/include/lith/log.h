#pragma once

#include "fmt/core.h"

class LoggerInterface {
public:
	virtual void log(const char* str) = 0;
};

void registerLoggerInterface(LoggerInterface* logger);

void print(const char* str);

template<typename... _args>
void print(const char* format, const _args&... args) {
	std::string str = fmt::format(fmt::runtime(format), args...);
	print(str.c_str());
}