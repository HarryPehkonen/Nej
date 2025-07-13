# Nej - No EmoJis!

A robust, memory-safe C++17 command-line tool for removing emojis from UTF-8 text files with atomic file operations and comprehensive Unicode support.

## Overview

Nej processes text files to remove emoji sequences while preserving all other Unicode characters. It uses battle-tested libraries for UTF-8 processing and implements safe file operations to prevent data loss.

## Features

### ✅ **Strengths & Advantages**

#### **Memory Safety & Modern C++ Design**
- **RAII (Resource Acquisition Is Initialization)**: All file handles, temporary files, and memory allocations are automatically managed through C++ destructors
- **Exception-safe operations**: UTF-8 processing gracefully handles malformed sequences without crashes
- **No manual memory management**: Uses standard containers (`std::string`, `std::vector`) and smart resource management
- **Modern C++17**: Leverages structured bindings, `std::filesystem`, and other modern language features

#### **Robust UTF-8 & Unicode Handling**
- **Professional UTF-8 library**: Uses [UTF8-CPP](https://github.com/nemtrif/utfcpp) instead of manual parsing for proven correctness
- **Comprehensive emoji support**: Handles complex emoji sequences including multi-codepoint combinations (👨‍👩‍👧‍👦, 🏳️‍🌈)
- **Malformed sequence handling**: Invalid UTF-8 bytes are safely replaced with '?' characters rather than causing errors
- **Longest-match algorithm**: Correctly identifies and removes the longest possible emoji sequences

#### **Data Safety & Atomic Operations**
- **Safe backup creation**: Automatically increments backup filenames (`.bak`, `.bak1`, `.bak2`) to prevent overwriting existing backups
- **Atomic file operations**: Uses temporary files with unique names (PID + timestamp) to prevent corruption during processing
- **Cross-platform compatibility**: Handles filesystem differences between Unix and Windows systems
- **Race condition prevention**: Eliminates TOCTOU (Time-of-Check-to-Time-of-Use) vulnerabilities in temporary file creation

#### **Comprehensive Testing**
- **Unit tests**: 9 test cases covering edge cases, malformed input, and various emoji types
- **Integration tests**: End-to-end testing with real files and backup creation verification
- **Continuous validation**: Both Google Test framework and shell-based integration testing

#### **Professional CLI Interface**
- **CLI11 library**: Industry-standard command-line parsing with automatic help generation
- **Clear error messages**: Informative feedback for file access issues, binary file detection, etc.
- **Flexible operation modes**: Supports dry-run mode, in-place editing, and stdout output

### ⚠️ **Limitations & Considerations**

#### **Binary File Detection Limitations**
- **Simple heuristic**: Only checks for null bytes (`\x00`) in the first 4KB of files
- **False positives**: Text files containing null bytes (e.g., some log formats, certain encodings) are incorrectly classified as binary
- **False negatives**: Binary files without null bytes in the first 4KB (e.g., some image headers, certain binary formats) may be processed incorrectly
- **Limited detection scope**: Doesn't use file extensions, magic numbers, or content analysis beyond null byte detection

#### **Input/Output Limitations**
- **No stdin/stdout support**: Cannot be used in Unix pipes or with redirected input/output
- **File-only processing**: Requires actual files on disk; cannot process streams or in-memory content
- **Line-by-line processing**: Processes files line by line, which may not be suitable for files with very long lines

#### **Performance Considerations**
- **Memory usage**: Loads entire lines into memory, potentially problematic for extremely large single-line files
- **Temporary file overhead**: Creates temporary files for in-place operations, requiring additional disk space
- **UTF-8 validation overhead**: Full UTF-8 validation on every character may be slower than simpler approaches for ASCII-heavy files

#### **Feature Gaps**
- **No configuration**: Emoji definitions are compiled-in; cannot customize which Unicode sequences to remove
- **No pattern matching**: Cannot remove custom patterns or non-emoji Unicode ranges
- **No encoding conversion**: Only handles UTF-8; other encodings must be converted externally
- **No regex support**: Cannot use regular expressions for more complex text processing

## Installation

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 19.14+)
- CMake 3.10+
- Internet connection (for downloading dependencies)

### Building from Source
```bash
# Clone the repository
git clone <repository-url>
cd Nej

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Optional: Install system-wide
cmake --build . --target install
```

### Build Types
```bash
# Debug build with symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Optimized release build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

## Usage

### Basic Syntax
```bash
nej [OPTIONS] file1.txt [file2.txt ...]
```

### Command Line Options
- `-h, --help`: Display help message and exit
- `-i, --in-place EXT`: Edit files in-place with backup (specify backup extension)
- `--dry-run`: Report what would be changed without modifying files

### Examples

#### Basic Usage (Output to stdout)
```bash
# Remove emojis and print to stdout
nej document.txt

# Process multiple files
nej file1.txt file2.txt file3.txt
```

#### In-Place Editing with Backup
```bash
# Edit file in-place, create backup with .bak extension
nej -i.bak document.txt

# Custom backup extension
nej -i.original document.txt

# Multiple files with backup
nej -i.backup *.txt
```

#### Dry Run Mode
```bash
# See what would be changed without modifying files
nej --dry-run document.txt
# Output: File: "document.txt", Emojis removed: 5

# Dry run with multiple files
nej --dry-run *.txt
```

### Integration with Other Tools

#### Using with `find`
```bash
# Process all .txt files in directory tree
find . -name "*.txt" -exec nej -i.bak {} \;

# Process only files modified in last 7 days
find . -name "*.txt" -mtime -7 -exec nej -i.clean {} \;

# Dry run on all markdown files
find . -name "*.md" -exec nej --dry-run {} \;
```

#### Batch Processing
```bash
# Process all files in current directory
nej -i.backup *.txt

# Process with shell globbing
nej --dry-run **/*.md  # (with bash globstar enabled)
```

#### Version Control Integration
```bash
# Clean files before committing
find . -name "*.txt" -exec nej -i.pre-commit {} \;
git add .
git commit -m "Remove emojis from text files"
```

## File Processing Behavior

### Text File Processing
- **Line-by-line**: Files are processed one line at a time
- **UTF-8 encoding**: Input files must be valid UTF-8 (malformed sequences become '?')
- **Emoji replacement**: Each emoji/sequence is replaced with a single space character
- **Preserves formatting**: Whitespace, punctuation, and non-emoji Unicode characters are preserved

### Binary File Handling
- **Automatic detection**: Files containing null bytes in the first 4KB are skipped
- **Warning message**: Displays "Warning: Skipping binary file: filename"
- **Safe behavior**: Never attempts to process detected binary files

### Backup File Strategy
- **Incremental naming**: `.bak`, `.bak1`, `.bak2`, etc. to prevent overwrites
- **Same directory**: Backups are created in the same directory as the original file
- **Atomic operations**: Original file is only modified after successful processing

## Error Handling

### File Access Errors
```bash
$ nej nonexistent.txt
Error: File not found: nonexistent.txt

$ nej /protected/file.txt
Error: Could not open file for reading: /protected/file.txt
```

### Binary File Detection
```bash
$ nej binary_file.exe
Warning: Skipping binary file: binary_file.exe
```

### Backup Creation Issues
```bash
$ nej -i.bak readonly_dir/file.txt
Error: Could not create backup file for readonly_dir/file.txt: Permission denied
```

## Testing

### Running Tests
```bash
# All tests (unit + integration)
cd build && ctest

# Unit tests only
cd build && ./nej_tests

# Integration tests only
cd tests/integration && ./test_line_by_line.sh

# Specific test cases
cd build && ./nej_tests --gtest_filter=RemoveEmojisTest.HandlesEmptyString
```

### Code Quality
```bash
# Format code
clang-format -i src/*.cpp src/*.h tests/*.cpp

# Lint code
clang-tidy src/*.cpp tests/*.cpp -- -I src
```

## Technical Details

### Dependencies
- **CLI11**: Command-line argument parsing (fetched automatically)
- **UTF8-CPP**: UTF-8 encoding/decoding (fetched automatically)
- **Google Test**: Unit testing framework (for development)

### Architecture
- **Modular design**: Core logic separated from CLI interface
- **Header-only dependencies**: UTF8-CPP requires no separate compilation
- **Static linking**: All dependencies are statically linked for easy deployment

### Build System
- **CMake**: Cross-platform build system with automatic dependency management
- **FetchContent**: Automatic downloading and building of dependencies
- **Out-of-source builds**: Clean separation of source and build artifacts

## Contributing

### Development Setup
1. Install prerequisites (C++17 compiler, CMake)
2. Clone repository and build with debug symbols
3. Run tests to ensure functionality
4. Follow existing code style (see `.clang-format`)

### Code Style
- Modern C++17 with RAII principles
- Google-based formatting with 4-space indentation
- Comprehensive error handling and resource management
- Function signature style: `auto function() -> return_type`

## License

See `LICENSE` file for license information.

## Troubleshooting

### Common Issues

**"Invalid cross-device link" error**: 
- Temporary files are created in the same directory as the source file to avoid cross-filesystem issues

**"Binary file" warnings for text files**:
- Files containing null bytes are treated as binary; check file encoding or use a hex editor to identify null bytes

**Performance with large files**:
- Tool processes files line-by-line; very large single-line files may use significant memory

**UTF-8 encoding issues**:
- Ensure input files are valid UTF-8; other encodings should be converted first using tools like `iconv`