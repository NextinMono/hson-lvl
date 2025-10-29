#include "log.h"
#include <iostream>

void Log(const char* message) {
	std::cerr << message << std::endl;
}

void Log(const std::string& message) {
	Log(message.c_str());
}
