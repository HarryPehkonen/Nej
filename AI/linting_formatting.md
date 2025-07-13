# Linting and Formatting: Nej (No EmoJis)

## 1. Code Formatting with clang-format

- **`clang-format`** will be used to automatically format the C++ source code, ensuring consistent style across the project.
- The version `clang-format-19` is available on the system and will be referred to as `clang-format`.

### 1.1. Configuration

- A `.clang-format` file will be placed at the root of the project to define the formatting rules.
- The specific style (e.g., `LLVM`, `Google`, `Microsoft`) will be chosen and configured within this file. A common approach is to start with a predefined style and then customize it as needed.

Example `.clang-format` (at project root):

```yaml
# Example .clang-format configuration
# This will be refined based on project needs.
BasedOnStyle: Google
IndentWidth: 4
UseTab: Never
BreakBeforeBraces: Attach
AllowShortIfStatementsOnASingleLine: false
ColumnLimit: 100
```

### 1.2. Running clang-format

- **Formatting a single file:**
    ```bash
    clang-format -i <file_path>
    ```
    (The `-i` flag applies the changes directly to the file.)

- **Formatting multiple files (e.g., all `.cpp` and `.h` files):**
    ```bash
    find . -regex '.*\.\(cpp\|hpp\|c\|h\)' -exec clang-format -style=file -i {} \;
    ```

- **Checking formatting without applying changes (for CI/pre-commit hooks):**
    ```bash
    clang-format --Werror --dry-run <file_path>
    ```
    (This will return a non-zero exit code if the file is not formatted correctly.)

## 2. Static Analysis with clang-tidy

- **`clang-tidy`** will be used for static analysis to identify potential bugs, style violations, and enforce best practices.
- The version `clang-tidy-19` is available on the system and will be referred to as `clang-tidy`.

### 2.1. Configuration

- A `.clang-tidy` file will be placed at the root of the project to specify which checks to enable/disable and their configurations.
- `clang-tidy` can also read configuration from the `CMakeLists.txt` file or command-line arguments.

Example `.clang-tidy` (at project root):

```yaml
# Example .clang-tidy configuration
# This will be refined based on project needs.
Checks: 'clang-diagnostic-*,clang-analyzer-*,modernize-*,readability-*,performance-*,bugprone-*,cppcoreguidelines-*
'
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
```

### 2.2. Running clang-tidy

- **Integrating with CMake:** The recommended way to run `clang-tidy` is through CMake's integration, which ensures it uses the correct compile commands.

    1.  **Generate compile commands database:**
        ```bash
        cd build
        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
        ```
        This will create a `compile_commands.json` file in the `build` directory.

    2.  **Run clang-tidy:**
        ```bash
        run-clang-tidy -p build
        ```
        (You might need to install `run-clang-tidy` separately, often part of the `clang-tools-extra` package.)

- **Running on a single file (less common for projects with CMake):**
    ```bash
    clang-tidy <file_path> -- -std=c++17 <other_compiler_flags>
    ```
    (The `--` separates `clang-tidy` options from compiler options.)

## 3. Integration into Development Workflow

- **Pre-commit Hooks:** Consider setting up Git pre-commit hooks to automatically run `clang-format` and `clang-tidy` before commits, ensuring that all committed code adheres to the defined style and quality standards.
- **Continuous Integration (CI):** Integrate `clang-format` and `clang-tidy` checks into the CI pipeline to enforce code quality for all pull requests and merges.