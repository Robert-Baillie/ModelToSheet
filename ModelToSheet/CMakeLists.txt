# Define the source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Add source to this project's executable.
add_executable (ModelToSheet ${SOURCE_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ModelToSheet PROPERTY CXX_STANDARD 20)
endif()

# Define the platform options
if (WIN32)
	target_compile_definitions(ModelToSheet PRIVATE PLATFORM_WINDOWS)
endif()

# Target include directories for Librarys, Includes and Other Project Headers
target_include_directories(ModelToSheet PRIVATE include/spdlog/include src)


# Precompiled header
target_precompile_headers(ModelToSheet PRIVATE "src/pch.h")

# Enable assertions in Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ModelToSheet PRIVATE ENABLE_ASSERTS)
endif()
