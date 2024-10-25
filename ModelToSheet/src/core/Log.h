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


// Core Log Macros
// Define error (...) passing in undetermined variables. When we do we want to call the core logger __VA_ARGS__ passing in the (...) from before
#define TRACE_LOG(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#define INFO_LOG(...)  ::Log::GetLogger()->info(__VA_ARGS__)
#define WARN_LOG(...)  ::Log::GetLogger()->warn(__VA_ARGS__)
#define ERROR_LOG(...) ::Log::GetLogger()->error(__VA_ARGS__)