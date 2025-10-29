#pragma once
#include <string>

#define LOG(message) { Log(message); return; }
#define LOGB(message) { Log(message); return 0; }

void Log(const char* message);
void Log(const std::string& message);
