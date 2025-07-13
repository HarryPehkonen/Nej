# Coding Guidelines: Readability, Maintainability, and Extensibility

Beyond automated checks, adhering to certain coding practices significantly enhances the long-term health of a C++ project. These guidelines aim to foster a codebase that is easy to understand, modify, and extend.

## 1. Function and Method Design

-   **Single Responsibility Principle (SRP):** Each function or method should have one, and only one, reason to change. If a function's name requires conjunctions (e.g., "read and process"), it likely does too much. Break it down into smaller, more focused units.
-   **Keep it Small:** Aim for functions that are concise and fit comfortably on a single screen (e.g., 20-50 lines). Smaller functions are inherently easier to comprehend, test, debug, and reuse.

## 2. Naming Conventions

-   **Descriptive Names:** Use clear, unambiguous, and descriptive names for variables, functions, classes, and files. Names should convey the purpose and intent of the entity they represent.
-   **Consistency:** Adhere to a consistent naming convention throughout the codebase (e.g., `camelCase`, `PascalCase`, `snake_case`). `clang-format` can help enforce this.

## 3. Modularity and Encapsulation

-   **Logical Grouping:** Group related data and behavior into classes or structs. This improves organization and reduces complexity.
-   **Information Hiding:** Use access specifiers (`private`, `protected`) to hide implementation details. Expose only the necessary public interface. This reduces coupling and makes changes safer.
-   **Namespaces:** Utilize namespaces to prevent naming collisions and provide logical grouping for related code.

## 4. Loose Coupling and Minimal Dependencies

-   **Reduce Interdependencies:** Strive to minimize direct dependencies between different modules or classes. High coupling makes code harder to change and test in isolation.
-   **Explicit Data Flow:** Prefer passing data explicitly through function parameters rather than relying heavily on global variables or singletons, unless their use is clearly justified and limited.

## 5. Robust Error Handling

-   **Consistent Strategy:** Establish and follow a consistent error handling strategy. This might involve:
    -   **Exceptions:** For truly exceptional and unrecoverable conditions.
    -   **`std::optional` / `std::expected`:** For situations where a function might legitimately fail to produce a result, or produce an alternative result. Use these when they add clarity and reduce boilerplate, avoiding them if they introduce unnecessary noise.
    -   **Error Codes:** For simple, predictable failure modes where performance is critical.
-   **Informative Messages:** Provide clear and actionable error messages to aid in debugging and user understanding.

## 6. Memory Safety and Resource Management (RAII)

-   **Resource Acquisition Is Initialization (RAII):** This is a cornerstone of modern C++ for memory safety and resource management. Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) for dynamic memory, and RAII wrappers for other resources (file handles, locks, network connections).
-   **`const` Correctness:** Use `const` extensively for parameters that are not modified, for member functions that do not alter object state, and for variables whose values should not change. This improves readability, helps prevent accidental modifications, and enables compiler optimizations.

## 7. Code Documentation

-   **Focus on "Why":** Comments should explain the *reasoning* behind complex logic, design decisions, or non-obvious behavior, rather than simply restating *what* the code does (which should be evident from clear code).
-   **Public API Documentation:** Document the purpose, parameters, return values, and any preconditions/postconditions for public functions and classes.

## 8. Avoid Premature Optimization

-   Prioritize correctness, readability, and maintainability during initial development.
-   Optimize only when profiling identifies a specific performance bottleneck. Unnecessary optimization can often lead to more complex and less readable code.