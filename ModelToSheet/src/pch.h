#pragma once


// Common Standards we will need
#include <string>
#include <bitset>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <filesystem>

// Windows Definitions
#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#endif



// Standards
#include "Core/MacroDefinitions.h"
#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"

// Input
#include "Core/Input.h"

// Logging
#include "Core/Log.h"


// Events
#include "Events/Event.h"
#include "Events/EventDispatcher.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/ApplicationEvents.h"

// Renderer
#include "Renderer/Renderer.h"

// Resources
#include "Core/ResourceManager.h"

// Layer and Stack
#include "Layers/Layer.h"
#include "Layers/LayerStack.h"
#include "Layers/UILayer.h"
#include "Layers/ViewportLayer.h"
#include "Layers/DockspaceLayer.h"


// Helping

#include "Core/Helpers.h"