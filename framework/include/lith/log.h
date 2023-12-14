#pragma once

// I wish this did not have to be here
//#include "lith/math.h"

// #include <initializer_list>
// #include <string>

#include "fmt/core.h"

class LoggerInterface {
public:
	virtual void log(const char* str) = 0;
};

void registerLoggerInterface(LoggerInterface* logger);

void lithLog(const char* str);

template<typename... _args>
void lithLog(const char* format, const _args&... args) {
	std::string str = fmt::format(fmt::runtime(format), args...) + '\n';
	lithLog(str.c_str());
}

template<typename... _args>
void lithLogNoNewline(const char* format, const _args&... args) {

}