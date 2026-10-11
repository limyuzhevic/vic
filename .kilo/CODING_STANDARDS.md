# NLM Coding Standards and Best Practices

## 1. Overview

This document defines the coding standards and best practices for the Neural Learning Machine (NLM) project. The standards aim to improve code quality, maintainability, and consistency across the codebase.

## 2. Error Handling Standards

### 2.1 General Principles
- **Always validate inputs**: Check function parameters for null pointers, invalid ranges, and inconsistent values
- **Use exceptions for exceptional conditions**: throw `std::invalid_argument`, `std::runtime_error`, etc.
- **Return error codes when appropriate**: bool return types indicate success/failure
- **Provide meaningful error messages**: Include context and values that caused the error
- **Clean up resources on error**: Use RAII and ensure proper cleanup in exception handlers

### 2.2 Error Handling Patterns

#### 2.2.1 File I/O Operations
```cpp
// Good pattern
std::ifstream file(filepath);
if (!file.is_open()) {
    std::cerr << "Error: Cannot open file " << filepath << ": " 
              << std::strerror(errno) << "\n";
    return false;
}

// Always close file (RAII handles automatic cleanup)
// For custom cleanup, use try-catch
```

#### 2.2.2 Resource Management
```cpp
// Good pattern - use RAII
{
    std::lock_guard<std::mutex> lock(mutex_);
    // Critical section
}
// Automatic unlock when leaving scope

// Avoid raw pointers without proper ownership tracking
```

#### 2.2.3 Memory Management
```cpp
// Good pattern - use smart pointers
std::unique_ptr<Data> data = std::make_unique<Data>();
// Automatic cleanup

// Or use factory functions with clear ownership
auto data = createData();
```

### 2.3 Exception Handling
```cpp
try {
    // Operation that might throw
} catch (const std::invalid_argument& e) {
    std::cerr << "Invalid argument: " << e.what() << "\n";
    return false;
} catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << "\n";
    return false;
} catch (...) {
    std::cerr << "Unknown error occurred\n";
    return false;
}
```

## 3. Documentation Standards

### 3.1 File Headers
Each source file must begin with:
1. Brief description of file purpose
2. Key components and their relationships
3. Dependencies and external interfaces
4. Author/date information
5. Brief usage examples when appropriate

### 3.2 Function Documentation
Use Doxygen-style comments:
```cpp
/**
 * @brief [Short description of function]
 * 
 * @param [param_name] [Description of parameter]
 * @param [param_name] [Description of parameter]
 * @return [Description of return value]
 * @throw [Exception type] [When and why exception is thrown]
 * 
 * @note [Additional notes, important considerations]
 * @see [Related functions]
 */
ReturnType functionName(ParamType param1, ParamType param2);
```

### 3.3 Code Comments
- Use comments to explain *why* not *what* (code should be self-documenting)
- Add comments for complex algorithms, edge cases, and non-obvious logic
- Use TODO/FIXME comments for incomplete work:
  - `TODO: Implement JSON parser support`
  - `FIXME: Race condition in multi-threaded access`

## 4. Code Style Standards

### 4.1 Naming Conventions
```cpp
// Variables and members
private:
    std::string filePath_;      // snake_case for private members
    int maxIterations_;         // snake_case for private members
    
public:
    std::string getFilePath() const;  // camelCase for getters
    void setFilePath(const std::string& path); // camelCase for setters

// Functions and methods
void processData();        // camelCase for functions
void SaveToFile();         // ERROR: Should be saveToFile()

// Constants
static constexpr int MAX_SIZE = 100;
static constexpr double PI = 3.14159;
```

### 4.2 Code Organization
```cpp
// File structure (example)
namespace nlm {

struct ClassName::Impl {
    // Private implementation details
    Type member1_;
    AnotherType member2_;
};

// Public interface
ClassName() : pImpl(std::make_unique<Impl>()) {}
~ClassName() = default;

// Disable copying, enable moving
ClassName(const ClassName&) = delete;
ClassName& operator=(const ClassName&) = delete;
ClassName(ClassName&&) noexcept;
ClassName& operator=(ClassName&&) noexcept;

// Public methods
void publicMethod() {
    // Implementation
}

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
```

### 4.3 Braces and Whitespace
```cpp
// Function definitions
ReturnType ClassName::methodName(Param param) {
    // Implementation
}

// Control structures
if (condition) {
    // True branch
} else if (otherCondition) {
    // Other branch
} else {
    // False branch
}

// Loops
for (int i = 0; i < limit; ++i) {
    // Loop body
}

while (condition) {
    // Loop body
}
```

### 4.4 Maximum Line Length
- **80 characters** for code lines
- **100 characters** for comments and documentation
- Break long lines appropriately, especially for parameter lists

## 5. Performance Standards

### 5.1 Memory Management
- **Use smart pointers**: `std::unique_ptr` for exclusive ownership, `std::shared_ptr` for shared ownership
- **Pre-allocate containers** when size is known
- **Use move semantics** when transferring ownership
- **Check for memory allocation failures** and handle gracefully

### 5.2 Computational Efficiency
- **Use const references** for function parameters when passing large objects
- **Reserve memory** for containers when size is known in advance
- **Minimize redundant calculations** in loops
- **Use appropriate data structures**: vector for dynamic arrays, map for associative containers

### 5.3 Thread Safety
```cpp
// Always protect shared state with mutexes
{
    std::lock_guard<std::mutex> lock(mutex_);
    // Access shared state
}

// Reentrant methods should be const
Type getValue() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return value_;
}
```

## 6. Testing and Validation Standards

### 6.1 Unit Testing
- **Test edge cases**: Empty inputs, boundary values, invalid inputs
- **Test error conditions**: File not found, invalid parameters, out of memory
- **Test concurrent access** when applicable
- **Test resource cleanup**: Ensure no memory leaks or resource leaks

### 6.2 Performance Testing
- **Profile critical sections**: Identify bottlenecks
- **Test with realistic data sizes**: Don't just test with small inputs
- **Benchmark key operations**: Compare performance improvements

## 7. Build and Deployment Standards

### 7.1 Build Configuration
- **Use consistent build system**: CMake with modern features
- **Enable compiler warnings**: Treat warnings as errors
- **Configure build types**: Debug, Release, RelWithDebInfo, MinSizeRel
- **Package dependencies**: Document all external dependencies

### 7.2 Code Quality Tools
- **Run static analysis**: Check for code quality issues
- **Perform linting**: Enforce coding standards
- **Check for security vulnerabilities**: Use security scanning tools

## 8. File Organization Standards

### 8.1 Directory Structure
```
src/
├── core/              # Core framework components
│   ├── Types/         # Type definitions
│   ├── Config/        # Configuration management
│   ├── Logger/        # Logging system
│   └── Random/        # Random number generation
│
├── brain/             # Neural system components
│   ├── Neuron.cpp      # Individual neuron implementation
│   ├── Synapse.cpp     # Synapse connections
│   └── Brain.cpp       # Whole brain simulation
│
├── environment/       # Simulation environment
├── experiments/       # Experiment definitions
├── tests/             # Unit tests
└── visualization/     # Visualization system
```

### 8.2 Header vs. Implementation
- **Header files (.hpp)**: Declarations, template definitions, inline functions
- **Implementation files (.cpp)**: Function definitions, static implementations
- **Never include implementation in headers** (except for inline helper functions)

## 9. Anti-Patterns to Avoid

### 9.1 Memory Management Anti-Patterns
- **Raw pointers without ownership tracking** ❌
- **Memory leaks** ❌
- **Double-free errors** ❌
- **Use-after-free bugs** ❌

### 9.2 Error Handling Anti-Patterns
- **Silent failures** ❌
- **Generic exception catching** ❌
- **Missing error checking** ❌
- **Inconsistent error handling** ❌

### 9.3 Code Quality Anti-Patterns
- **Code duplication** ❌
- **Complex conditional logic** ❌
- **Missing documentation** ❌
- **Hard-coded values** ❌

## 10. Tools and Resources

### 10.1 Code Quality Tools
- **clang-tidy**: C++ linting and code analysis
- **cppcheck**: Static analysis tool
- **valgrind**: Memory debugging
- **AddressSanitizer**: Memory error detection

### 10.2 Documentation Tools
- **Doxygen**: API documentation generation
- **Sphinx**: Technical documentation
- **Markdown**: Project documentation

## 11. Continuous Integration

### 11.1 Build Pipeline
1. **Compile with all warnings enabled**
2. **Run static analysis tools**
3. **Execute unit tests**
4. **Run performance benchmarks**
5. **Generate documentation**
6. **Create build artifacts**

### 11.2 Testing Strategy
- **Unit tests**: Test individual components
- **Integration tests**: Test component interactions
- **Performance tests**: Verify performance requirements
- **Regression tests**: Ensure no regressions

## 12. Migration Guidelines

### 12.1 Upgrading to New Standards
1. **Audit existing codebase**: Identify violations
2. **Prioritize fixes**: Start with critical files
3. **Gradual migration**: Apply standards incrementally
4. **Automate checks**: Use CI to enforce standards

### 12.2 Legacy Code Handling
- **Document legacy patterns**: Explain why they exist
- **Isolate legacy code**: Minimize impact on new code
- **Gradually refactor**: Replace legacy patterns with standards

## 13. Conclusion

Following these coding standards will improve:
- **Code readability**: Easier to understand and maintain
- **Code quality**: Fewer bugs and issues
- **Developer productivity**: Faster development and debugging
- **Team collaboration**: Consistent code across the team
- **Long-term maintenance**: Sustainable codebase

Adhering to these standards is essential for maintaining a high-quality, maintainable neural simulation framework capable of supporting advanced research in artificial intelligence and neuroscience.

---
*Last Updated: 2026-10-11*
*NLM Project Team*