# glad/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(glad)

# Create a library from the glad source file
add_library(glad STATIC src/glad.c)

# Specify the include directories for glad
target_include_directories(glad PUBLIC include)
