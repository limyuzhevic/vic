# NLM Code Style Guidelines

## Overview

This document provides comprehensive coding style guidelines for the NLM (熙然) brain simulation project, ensuring code consistency, maintainability, and readability across the entire codebase.

## General Principles

### 1. Consistent Formatting

#### Indentation and Layout
- Use 4-space indentation (no tabs)
- Maintain consistent indentation throughout files
- Limit line length to 100 characters for code, 120 for comments/docstrings
- Align related statements on separate lines for readability

#### Spacing
- Use spaces around binary operators: `a + b`, `a = b`
- No space after function name when followed by parenthesis: `func(arg)`
- One blank line between related function definitions
- Two blank lines between major code blocks or sections

### 2. Naming Conventions

#### Identifiers
- **Classes**: PascalCase (`Brain`, `NeuralRegion`, `STDP`)
- **Functions**: camelCase (`getId()`, `setMembranePotential()`)
- **Variables**: camelCase (`neuronId`, `simulationTime`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_SPIKE_HISTORY`, `DEFAULT_SEED`)
- **Types**: CamelCase (`NeuronId`, `MembranePotential`, `ConfigSource`)

#### File Naming
- **Header files**: `Name.hpp` (`Brain.hpp`, `Neuron.hpp`)
- **Implementation files**: `Name.cpp` (`Brain.cpp`, `Neuron.cpp`)
- **Directories**: snake_case (`core/`, `brain/`, `plasticity/`)

### 3. Documentation Style

#### Doxygen Comments
Follow Doxygen-compatible comment format:
```cpp
/**
 * @brief Brief description of functionality
 * 
 * @param param_name Description of parameter
 * @return Description of return value
 * @details Extended explanation of behavior
 * @note Important notes or caveats
 * @throw exception_type When exception may be thrown
 * @see Related functions or methods
 * @since Version when introduced
 */
```

#### Code Comments
- Use meaningful comments for complex algorithms
- Explain "why" not "what" (self-documenting code preferred)
- Reference scientific literature where appropriate
- Include mathematical formulations for algorithms
- Document assumptions and limitations

### 4. Error Handling

#### Error Reporting
- Use consistent error logging macros: `NLM_LOG_ERROR()`, `NLM_LOG_WARN()`, `NLM_LOG_INFO()`
- Include descriptive error messages with context
- Log error codes when appropriate
- Never use `std::cerr` or `printf` directly

#### Exception Safety
- Ensure exception safety in move constructors and assignment operators
- Use noexcept where appropriate for resource transfer operations
- Implement proper cleanup in destructors

### 5. Memory Management

#### Smart Pointers
- Use `std::unique_ptr` for exclusive ownership
- Use `std::shared_ptr` for shared ownership
- Never use raw pointers except in pImpl idiom or when explicitly required
- Implement proper copy semantics (delete copy constructors/assignment operators where needed)

#### Resource Management
- Follow RAII (Resource Acquisition Is Initialization) principle
- Ensure all allocated resources are cleaned up in destructors
- Use consistent initialization order in constructors

## Language Features

### C++17 Features

#### Modern C++ Practices
```cpp
// Use auto for local variable type deduction
auto neuronCount = config->getOr<size_t>("neuron_count", 1000);

// Range-based for loops
for (const auto& neuron : neurons) {
    // process neuron
}

// Initialize lists with uniform initialization
std::vector<int> numbers = {1, 2, 3, 4, 5};

// Struct with default members
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source = ConfigSource::Default;
    std::string description = "";
};
```

#### Const Correctness
- Use `const` wherever possible for function parameters and return values
- Provide both const and non-const accessors for mutable objects
- Use `const_iterator` for read-only container access

#### RAII Classes
- Encapsulate resources in classes with proper cleanup
- Use smart pointers instead of manual memory management
- Implement move semantics for efficiency

### Templates and Type Safety

#### Template Usage
```cpp
// Type-safe configuration access
template<typename T>
std::optional<T> Config::get(const std::string& key) const;

// Enable proper error checking and type conversion
```

#### Type Aliases
```cpp
// Meaningful type names for domain-specific types
using MembranePotential = float;  // in millivolts (mV)
using FiringRate = float;        // in Hertz (Hz)
using SynapticWeight = float;    // normalized [-1.0, 1.0]
```

## Architecture Documentation

### File Structure Guidelines

#### Core Headers (`.hpp`)
- Contain only declarations and interface definitions
- Include necessary forward declarations
- Provide comprehensive Doxygen documentation
- Maintain constant interface (no implementation details)

#### Implementation Files (`.cpp`)
- Contain only implementations
- Include corresponding `.hpp` file
- Provide inline documentation for complex algorithms
- Keep implementation details separate from interface

### Documentation Standards

#### Code Comments
- Document mathematical formulations
- Reference biological models and scientific literature
- Explain algorithm complexity and assumptions
- Include usage examples where helpful

#### High-Level Documentation
- Maintain consistent structure across all documentation files
- Include implementation status and phase information
- Reference scientific background and biological inspiration
- Document configuration parameters and their effects

## Integration and Performance

### Performance Considerations
- Use references for small objects to avoid copying
- Cache frequently accessed data in member variables
- Consider SIMD optimizations for numerical operations
- Use efficient data structures for large collections

### Thread Safety
- Document thread safety assumptions
- Use proper synchronization for shared resources
- Avoid race conditions in concurrent access

## Code Quality Tools

### Static Analysis
- Use static analysis tools to detect potential issues
- Implement consistent coding standards across the project
- Regularly run linting and type checking

### Testing and Validation
- Write comprehensive unit tests for critical components
- Validate configuration parameters before use
- Test edge cases and error conditions
- Ensure proper memory management in all scenarios

## Developer Workflow

### Code Review
- Follow existing code patterns and conventions
- Ensure proper documentation for all public APIs
- Test changes thoroughly before integration
- Update documentation when changing interfaces

### Continuous Integration
- Maintain compatibility with existing build system
- Ensure all compilation flags are properly set
- Validate against coding standards automatically

## References

- Stroustrup, B. (2000). "The C++ Programming Language"
- Alexandrescu, A. et al. (2001). "Modern C++ Design"
- Dayan & Abbott (2001). "Theoretical Neuroscience"
- Sporns, O. (2011). "Network Biology"
- Sporns, O. (2018). "The complex brain"

## Version Control

### Commit Messages
- Use clear, descriptive commit messages
- Reference related issues or documentation
- Follow conventional commit format where appropriate
- Include co-authored-by: openhands <openhands@all-hands.dev> for all commits

This document will be updated as the codebase evolves and new best practices are adopted.