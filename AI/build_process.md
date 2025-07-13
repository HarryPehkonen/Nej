# Build Process: Nej (No EmoJis)

## 1. Build System

- **CMake** will be used as the primary build system for Nej. CMake provides cross-platform compatibility and a robust way to manage project dependencies and compilation.

## 2. Prerequisites

- **C++ Compiler:** A C++17 compatible compiler (e.g., GCC 7+, Clang 5+, MSVC 19.14+) is required.
- **CMake:** Version 3.10 or higher is recommended.

## 3. Building the Project

### 3.1. Standard Build Steps

To build the project from the root directory, follow these steps:

1.  **Create a build directory (out-of-source build):**
    ```bash
    mkdir build
    cd build
    ```

2.  **Configure CMake:**
    This step generates the build system files (e.g., Makefiles for Unix-like systems, Visual Studio solutions for Windows).
    ```bash
    cmake ..
    ```
    *Note: If you want to specify a particular generator (e.g., for Visual Studio), you can do so with the `-G` flag (e.g., `cmake .. -G "Visual Studio 16 2019"`).*

3.  **Build the project:**
    This step compiles the source code and links the executable.
    ```bash
    cmake --build .
    ```
    *Alternatively, if using Makefiles, you can simply run `make` in the build directory.*

### 3.2. Build Options

- **Build Type:** You can specify the build type during the CMake configuration step. Common build types include:
    - `Debug`: Includes debugging symbols and disables optimizations.
    - `Release`: Enables optimizations and omits debugging symbols.
    - `RelWithDebInfo`: Enables optimizations but includes debugging symbols.
    - `MinSizeRel`: Optimizes for size and omits debugging symbols.

    Example:
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

## 4. Installing the Project (Optional)

After building, you can optionally install the executable to a system-wide or custom location.

1.  **Build the `install` target:**
    ```bash
    cmake --build . --target install
    ```
    *Note: On Unix-like systems, you might need `sudo` for system-wide installation (e.g., `sudo cmake --build . --target install`).*

- **Installation Prefix:** The default installation prefix can be changed during CMake configuration:
    ```bash
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
    ```

## 5. Cleaning the Build

To remove all compiled files and artifacts from the build directory:

```bash
cmake --build . --target clean
```
*Alternatively, you can simply remove the `build` directory: `rm -rf build` (on Unix-like systems).*