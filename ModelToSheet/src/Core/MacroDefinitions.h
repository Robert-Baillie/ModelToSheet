#pragma once

/* Event Definitions */
// BIT - for Event Categories and Simplification of Bitwise definitions
#define BIT(x) (1 << x) // Left Shift 1 by number in bit i.e: 1 << 0 = 0001, 1 << 1 = 0010.

// Event Function Binding.
// bind: creates a function object that binds arguments to a callable object, returnng a new callable object
// &x: callable object being bound, x is a member function, we send the pointer
#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1) 


// Static class - at runtime we want to be able to check what event these are. 
// Overwrite (virtual) - we just have a pointer to an event, we want to be able to find it from just Event&.
// Helps us define events without rewriting the code everytime.

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
	
// Category - So we do not need to rewrite category every time
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

/* Log Macros */
// Define error (...) passing in undetermined variables. When we do we want to call the core logger __VA_ARGS__ passing in the (...) from before
#define TRACE_LOG(...) do { \
    ::Log::Get().GetLogger()->trace(__VA_ARGS__); \
    ::Log::Get().Push(Log::Level::Trace, __VA_ARGS__); \
} while(0)

#define INFO_LOG(...) do { \
    ::Log::Get().GetLogger()->info(__VA_ARGS__); \
    ::Log::Get().Push(Log::Level::Info, __VA_ARGS__); \
} while(0)

#define WARN_LOG(...) do { \
    ::Log::Get().GetLogger()->warn(__VA_ARGS__); \
    ::Log::Get().Push(Log::Level::Warn, __VA_ARGS__); \
} while(0)

#define ERROR_LOG(...) do { \
    ::Log::Get().GetLogger()->error(__VA_ARGS__); \
    ::Log::Get().Push(Log::Level::Error, __VA_ARGS__); \
} while(0)


/* Assertations */
// checks a condition, if it fails it prints it and adds a break.
// Want this to be removed in release builds (debug only)
#ifdef ENABLE_ASSERTS
	#define ASSERT(x, ...) { if(!(x)) { ERROR_LOG("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define ASSERT(x, ...)
#endif



/* Animations */
#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 100
#define ASSIMP_IMPORTER_FLAGS aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_GlobalScale |  aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder
