# Project Requirements: Nej (No EmoJis)

## 1. Functional Requirements

### 1.1. Emoji Removal
- The tool shall remove emojis from text files.
- The definition of an "emoji" will be flexible and can be updated during development (e.g., based on Unicode ranges or a dedicated library).

### 1.2. Input/Output
- **In-place editing with backup:**
    - The tool shall support an in-place editing mode specified by a flag (e.g., `-i`).
    - When in-place editing, the original file shall be renamed with a specified suffix (e.g., `-i.bak` renames `file.txt` to `file.txt.bak`).
    - If a backup file with the exact name already exists (e.g., `file.txt.bak`), the tool shall increment a digit suffix (e.g., `file.txt.bak1`, `file.txt.bak2`, etc.) until a unique backup filename is found.
- **Standard Output:**
    - If no in-place editing flag is provided, the tool shall print the processed content to standard output (stdout).

### 1.3. File Handling
- **Non-existent files:** If a specified input file does not exist, the tool shall report an error and skip that file.
- **Binary files:** The tool shall attempt to detect binary (non-text) files. If detected (without guarantees of perfect detection), the tool shall issue a warning and skip processing that file.
- **Encoding:** The tool shall assume all input files are UTF-8 encoded.

### 1.4. Command-Line Interface (CLI)
- The tool shall accept file paths as command-line arguments, supporting shell wildcards (e.g., `nej -i.bak *.txt`).
- The tool shall support execution via `find . -iname '*.cpp' -exec nej -i.bak {} \;`.
- The in-place editing flag shall be `-i<suffix>`, where `<suffix>` is the desired backup file extension (e.g., `-i.bak`, `-iblarg`).

## 2. Non-Functional Requirements

### 2.1. Performance
- The tool should be reasonably efficient for processing large text files.

### 2.2. Portability
- The tool shall be designed to be easily portable to Windows and macOS, in addition to Linux.

### 2.3. Maintainability
- The codebase shall be clean, well-structured, and easy to understand and modify.

### 2.4. External Dependencies
- The number of external C++ libraries shall be limited to minimize complexity and build requirements.

### 2.5. Error Handling
- The tool shall provide clear and informative error messages for issues such as non-existent files or unprocessable binary files.