#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


// Logging class - This is essentially a wrapper for spdlog include
class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_AppLogger; }


private:
	static std::shared_ptr<spdlog::logger> s_AppLogger;
};


