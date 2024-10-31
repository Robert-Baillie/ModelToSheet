#include "pch.h"

#include "Application.h"

// Entry point for Application based on Platform
// Windows
#ifdef PLATFORM_WINDOWS
	
int main(int argc, char** argv)
{
	// Initialising Logging Library
	Log::Init();
	TRACE_LOG("Initialised Logging.");

	// App to contain itself so create a pointer to it
	std::unique_ptr<Application> app = std::make_unique<Application>();

	// Run App.
	app->Run();


	return 0;
}



	

#endif
