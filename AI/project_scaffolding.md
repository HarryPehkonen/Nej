# Project Scaffolding: Nej (No EmoJis)

This document outlines the initial project structure and provides basic files to kickstart the development of the Nej tool.

## 1. Initial Directory Structure

The project should be initialized with the following directory structure:

```
Nej/
├── CMakeLists.txt
├── .clang-format
├── .clang-tidy
├── src/
│   └── main.cpp
├── tests/
│   └── CMakeLists.txt
│   └── test_main.cpp
└── AI/
    ├── build_process.md
    ├── coding_guidelines.md
    ├── linting_formatting.md
    ├── requirements.md
    ├── technical_details.md
    └── project_scaffolding.md
```

## 2. Root `CMakeLists.txt`

Create a `CMakeLists.txt` file in the root directory of the project with the following content. This file sets up the project, defines the C++ standard, and includes the source and test directories.

```cmake
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Nej VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Fetch CLI11 (Command Line Interface Library)
include(FetchContent)
FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.4.2 # Use a specific tag for stability
)
FetchContent_MakeAvailable(CLI11)

# Add CLI11 to the main executable
target_link_libraries(nej PRIVATE CLI11::CLI11)
```

## 3. `src/CMakeLists.txt`

Create a `CMakeLists.txt` file inside the `src/` directory with the following content. This defines the main executable.

```cmake
add_executable(nej main.cpp)
```

## 4. `src/main.cpp` (Placeholder)

Create a `main.cpp` file inside the `src/` directory with the following placeholder content. This provides a basic structure for the main application.

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <CLI/CLI.hpp> // Include CLI11 header

// Function to remove emojis (placeholder)
std::string removeEmojis(const std::string& text) {
    // TODO: Implement emoji removal logic here
    return text; // Return original text for now
}

int main(int argc, char** argv) {
    CLI::App app{"Nej - No EmoJis: A command-line tool for removing emojis from text files"};

    std::vector<std::string> input_files;
    app.add_option("files", input_files, "Input text files to process")
       ->required()
       ->check(CLI::ExistingFile);

    std::string backup_extension = ".bak";
    bool in_place = false;
    auto in_place_option = app.add_option("-i,--in-place", "Perform in-place editing with backup. Specify backup extension (e.g., -i.bak)")
                               ->option_text("EXT");

    in_place_option->callback([&](void) {
        in_place = true;
        if (in_place_option->count() > 0) {
            backup_extension = in_place_option->as<std::string>();
        }
    });

    CLI11_PARSE(app, argc, argv);

    if (in_place) {
        std::cout << "In-place editing enabled. Backup extension: " << backup_extension << std::endl;
    } else {
        std::cout << "Output will be printed to stdout." << std::endl;
    }

    for (const auto& file_path : input_files) {
        std::cout << "Processing file: " << file_path << std::endl;
        // TODO: Read file, remove emojis, and handle output/backup
    }

    return 0;
}
```

## 5. `tests/CMakeLists.txt`

Create a `CMakeLists.txt` file inside the `tests/` directory with the following content. This sets up Google Test.

```cmake
find_package(GTest CONFIG REQUIRED)

add_executable(nej_tests test_main.cpp)
target_link_libraries(nej_tests PRIVATE GTest::gtest_main)

# Link against the main project's source if needed for testing internal functions
# target_link_libraries(nej_tests PRIVATE nej)

include(GoogleTest)
gtest_discover_tests(nej_tests)
```

## 6. `tests/test_main.cpp` (Placeholder)

Create a `test_main.cpp` file inside the `tests/` directory with the following placeholder content. This is the entry point for Google Tests.

```cpp
#include "gtest/gtest.h"

// Placeholder for future tests
TEST(PlaceholderTest, TrueIsTrue) {
    ASSERT_TRUE(true);
}

// You might need to include headers from your main project here to test its functions
// #include "../src/your_module.h"

// Example of a test for a function that might be in src/
// TEST(EmojiRemovalTest, BasicEmojiRemoval) {
//     std::string text_with_emoji = "Hello 👋 World!";
//     std::string text_without_emoji = removeEmojis(text_with_emoji);
//     ASSERT_EQ(text_without_emoji, "Hello  World!");
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```