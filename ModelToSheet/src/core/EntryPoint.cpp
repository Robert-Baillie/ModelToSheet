#include "pch.h"



// Entry point for Application based on Platform
// Windows
#ifdef PLATFORM_WINDOWS
	
int main(int argc, char** argv)
{
	// Initialising Logging Library
	Log::Init();
	TRACE_LOG("Initialised Logging.");

	// App to contain itself so create a pointer to it
	
	return 0;
}



	

#endif
