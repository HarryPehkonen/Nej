cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Nej VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Enable compile commands database (required for clang-tidy)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source directory
add_subdirectory(src)

# Add test directory
enable_testing()
add_subdirectory(tests)

# Configure clang-tidy and clang-format for the project
# This assumes clang-tidy and clang-format are in the system PATH
# For clang-tidy, a compile_commands.json will be generated in the build directory
# which clang-tidy can use.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    message(STATUS "Generating compile_commands.json for clang-tidy")
endif()

# Fetch dependencies
option(FETCHCONTENT_QUIET "Quiet FetchContent" OFF)
include(FetchContent)

# Fetch UTF8-CPP (UTF-8 encoding/decoding library)
FetchContent_Declare(
  utf8cpp
  GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
  GIT_TAG        v4.0.5 # Use a specific tag for stability
)

FetchContent_MakeAvailable(utf8cpp)
