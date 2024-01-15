#include "lith/log.h"
#include <sstream>
#include <cstring>

static LoggerInterface* backend = nullptr;

void registerLoggerInterface(LoggerInterface* backend) {
	::backend = backend;
}

void print(const char* str) {
	backend->log(str);
}

// std::string lithToString(const char* x) {
// 	return std::string(x);
// }

// std::string lithToString(const std::string& x) {
// 	return x;
// }

// std::string lithToString(int x) {
// 	return std::to_string(x);
// }

// std::string lithToString(uint64_t x) {
// 	return std::to_string(x);
// }

// std::string lithToString(float x) {
// 	return std::to_string(x);
// }

// std::string lithToString(const vec2& x) {
// 	std::stringstream ss;
// 	ss << "(" << x.x << ", " << x.y << ")";
// 	return ss.str();
// }

// std::string lithToString(const vec3& x) {
// 	std::stringstream ss;
// 	ss << "(" << x.x << ", " << x.y << ", " << x.z << ")";
// 	return ss.str();
// }

// std::string lithToString(const vec4& x) {
// 	std::stringstream ss;
// 	ss << "(" << x.x << ", " << x.y << ", " << x.z << ", " << x.w << ")";
// 	return ss.str();
// }

// void printStringArgs(const char* format, std::initializer_list<std::string> arguments, bool newline) {
// 	auto argument = arguments.begin();
// 	const int len = (int)strlen(format);
	
// 	std::stringstream ss;
// 	ss << "[" << "lith" << "] ";

// 	for (int i = 0; i < len; i++) {
// 		char c = format[i];

// 		if (i != len - 1) {
// 			char c1 = format[i + 1];

// 			if (c == '{' && c1 == '}') {
// 				ss << *argument;
// 				argument += 1;
// 				i += 1;

// 				continue;
// 			}
// 		}
		
// 		ss << c;
// 	}

// 	if (newline) {
// 		ss << "\n";
// 	}

// 	std::string str = ss.str();
// 	backend->log(str.c_str());
// }