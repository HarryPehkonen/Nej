# Technical Details: Nej (No EmoJis)

## 1. Core Logic

### 1.1. Emoji Detection and Removal
- The initial approach for emoji detection will be based on specific Unicode code points. This list can be expanded or refined as development progresses.
- **Initial Emoji Unicode Code Points (Hexadecimal):**
    - `U+2728` (✨)
    - `U+1F41B` (🐛)
    - `U+1F4DD` (📝)
    - `U+267B` (♻️)
    - `U+2B06` (⬆️)
    - `U+2B07` (⬇️)
    - `U+2705` (✅)
    - `U+1F680` (🚀)
    - `U+1F6A7` (🚧)
    - `U+1F525` (🔥)
    - `U+1F3A8` (🎨)
    - `U+26A1` (⚡️)
    - `U+1F512` (🔒)
    - `U+1F484` (💄)
    - `U+1F389` (🎉)
    - `U+1F527` (🔧)
    - `U+1F4A1` (💡)
    - `U+1F4E6` (📦)
    - `U+1F4C4` (📄)
    - `U+1F4A5` (💥)
    - `U+1F500` (🔀)
    - `U+23EA` (⏪️)
    - `U+1F9EA` (🧪)
    - `U+1F921` (🤡)
    - `U+1F648` (🙈)
- Removal will involve iterating through the input text and filtering out characters identified as emojis based on their Unicode code points.

### 1.2. File I/O and In-Place Editing
- **Reading:** Files will be read into memory, assuming UTF-8 encoding.
- **Writing (Standard Output):** If no in-place flag is provided, the processed content will be written to `stdout`.
- **Writing (In-place with backup):**
    - The original file will be renamed to a backup name (e.g., `file.txt` to `file.txt.bak`).
    - The processed content will then be written to the original filename (`file.txt`).
    - Backup filename generation will involve appending a numeric suffix (e.g., `.bak1`, `.bak2`) if the base backup name already exists.

### 1.3. Binary File Detection
- A simple, non-guaranteed method for binary file detection will be implemented. This will involve:
    - Checking for the presence of null bytes (`\0`) within the first 4KB (4096 bytes) of the file content.
- If a file is suspected to be binary, a warning will be issued, and the file will be skipped.

## 2. Command-Line Interface (CLI) Parsing

- **Library Choice:** `CLI11` will be used for command-line argument parsing due to its modern C++ design and ease of integration.
- **Arguments:** The CLI will parse:
    - Input file paths (supporting multiple files and wildcards).
    - The in-place editing flag (`-i<suffix>`) to determine the backup extension and enable in-place mode.

## 3. C++ Standard and Libraries

- **C++ Standard:** C++17 will be used.
- **Standard Library:** Heavy reliance on the C++ Standard Library for string manipulation, file operations, and basic data structures.
- **External Libraries (Limited):** Only essential external libraries will be introduced, primarily `CLI11` for CLI parsing.

## 4. Portability Considerations

- The code will be written with cross-platform compatibility in mind, avoiding platform-specific APIs where possible and using standard C++ features.
- File path manipulation will use `std::filesystem` (C++17) for consistent behavior across operating systems.

## 5. Error Handling

- Robust error handling will be implemented for file operations (e.g., file not found, permission issues, write errors).
- Clear error messages will be provided to the user.