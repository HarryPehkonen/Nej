# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
Nej is a C++17 command-line tool for removing emojis from text files. It processes UTF-8 encoded files and can work in-place with backup creation or output to stdout.

## Build Commands

### Initial Setup
```bash
mkdir build
cd build
cmake ..
```

### Build Project
```bash
cmake --build .
```

### Build with Specific Type
```bash
# For debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# For release build  
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Clean Build
```bash
cmake --build . --target clean
```

## Testing Commands

### Run All Tests (Unit + Integration)
```bash
cd build
ctest
```

### Run Unit Tests Only
```bash
cd build
./nej_tests
```

### Run Integration Tests
```bash
cd tests/integration
./test_line_by_line.sh
```

### Run Specific Test Filter
```bash
cd build
./nej_tests --gtest_filter=RemoveEmojisTest.HandlesEmptyString
```

## Code Quality Commands

### Format Code
```bash
clang-format -i src/*.cpp src/*.h tests/*.cpp
```

### Lint Code
```bash
clang-tidy src/*.cpp tests/*.cpp -- -I src
```

## Architecture

### Core Components
- **main.cpp**: CLI interface using CLI11 library, file I/O, and in-place editing logic
- **core.cpp/core.h**: Core emoji removal logic and UTF-8 processing
- **emoji_data.h**: Static data containing Unicode emoji sequences as vectors of code points

### Key Design Patterns
- UTF-8 manual decoding with `decode_utf8_char()` function
- Longest-match algorithm for emoji sequence detection
- Atomic file operations for in-place editing (temp file → backup → rename)
- Binary file detection to skip non-text files

### Dependencies
- **CLI11**: Command-line argument parsing (fetched via CMake)
- **Google Test**: Unit testing framework
- **std::filesystem**: File operations (C++17)

### Important Files
- `src/emoji_data.h`: Contains `EMOJI_SEQUENCES` set - large auto-generated file with Unicode emoji data
- `tests/integration/test_line_by_line.sh`: Bash script for end-to-end testing
- `.clang-format` and `.clang-tidy`: Code style and linting configuration

### Build Artifacts
- `build/nej`: Main executable
- `build/nej_tests`: Unit test executable  
- `build-debug/`: Debug build directory (may exist)
- `build/compile_commands.json`: Generated for clang-tidy

## Development Notes
- Uses C++17 features including structured bindings and std::filesystem
- Manual UTF-8 decoding handles malformed sequences gracefully
- In-place editing uses atomic operations to prevent data loss
- Binary file detection prevents processing non-text files
- Integration tests verify end-to-end behavior including backup file creation