#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace nlohmann {};

using namespace std;
using namespace spdlog;
using namespace nlohmann;

std::shared_ptr<spdlog::logger> getConsole(std::string name);
void setDebugConsole();
