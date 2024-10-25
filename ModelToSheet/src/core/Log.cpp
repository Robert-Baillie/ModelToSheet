#include "pch.h"

#include "Log.h"


// Define the static logger outside of the class
std::shared_ptr<spdlog::logger> Log::s_AppLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");

	s_AppLogger = spdlog::stdout_color_mt("ModelToSheet");
	s_AppLogger->set_level(spdlog::level::trace);
}
