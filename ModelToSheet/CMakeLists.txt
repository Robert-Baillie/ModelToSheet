# Define the source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# ImGui has no CMakeList - add files directly
file(GLOB IMGUI_SOURCES "include/imgui/*.cpp" "include/imgui/*.h" 
                        "include/imgui/backends/imgui_impl_glfw.cpp"
                        "include/imgui/backends/imgui_impl_opengl3.cpp")


# Debugging: Print all files captured in SOURCE_FILES
message("Source files: ${SOURCE_FILES} ${IMGUI_SOURCES}")



# Define the platform options
if (WIN32)
	# Add source and imgui to this project's executable.
	add_executable (ModelToSheet WIN32 ${SOURCE_FILES} ${IMGUI_SOURCES})
	target_compile_definitions(ModelToSheet PRIVATE PLATFORM_WINDOWS PLATFORM_OPENGL)
else()
    add_executable(ModelToSheet ${SOURCE_FILES} ${IMGUI_SOURCES})
endif()


if(MSVC)
    target_link_options(ModelToSheet PRIVATE "/ENTRY:mainCRTStartup")
endif()


# Set Cpp Standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ModelToSheet PROPERTY CXX_STANDARD 20)
endif()


# Target include directories for Librarys, Includes and Other Project Headers
target_include_directories(ModelToSheet PRIVATE	src 
												include/spdlog/include 
												include/glfw/include
												include/glad/include
												include/imgui
												include/glm/glm
												include/stb
												include/assimp/include
)

# Add Glad Library
add_subdirectory(include/glad)

# Add GLFW library
add_subdirectory(include/glfw) 

# Add SPDLOG library
add_subdirectory(include/spdlog) 

# Add GLM library
add_subdirectory(include/glm) 


# Assimp options
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_INSTALL OFF)



# Add ASSIMP library
add_subdirectory(include/assimp) 


target_link_libraries(ModelToSheet PRIVATE glfw glad spdlog glm assimp) # Private - internal use.

# Precompiled header
target_precompile_headers(ModelToSheet PRIVATE "src/pch.h")

# Enable assertions in Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ModelToSheet PRIVATE ENABLE_ASSERTS)
endif()
