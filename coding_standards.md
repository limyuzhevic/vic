# NLM Coding Standards

## Overview
This document defines the coding standards and best practices for the NLM (Neural Learning Machine) project. All new code and modifications to existing code should adhere to these standards.

## File Organization

### Directory Structure
- `src/` - Source code
  - `core/` - Core utilities
  - `brain/` - Neural components
  - `dynamics/` - Neural dynamics
  - `plasticity/` - Plasticity rules
  - `development/` - Developmental system
  - `neuromodulation/` - Neuromodulators
  - `memory/` - Memory systems
  - `prediction/` - Prediction systems
  - `cognition/` - Cognitive mechanisms
  - `sensory/` - Sensory processing
  - `motor/` - Motor system
  - `environment/` - Environment interface
  - `experiments/` - Experiment framework
  - `visualization/` - Visualization
- `tests/` - Test suite
- `python/` - Python bindings

### File Naming Conventions
- `.cpp` files: Implementation files
- `.hpp` files: Header files
- Class names: PascalCase (e.g., `NeuralRegion`, `SpikeSystem`)
- Function names: snake_case (e.g., `initialize_brain`, `get_region_count`)
- Variable names: snake_case (e.g., `simulation_timestep`, `neuron_count`)
- Constant names: UPPER_SNAKE_CASE (e.g., `PI_VALUE`, `MAX_SYNAPSES`)

## Code Style

### Indentation
- Use 4 spaces per indentation level (no tabs)
- Maintain consistent indentation throughout files
- Use hanging indentation for continuation lines

### Line Length
- Maximum line length: 100 characters
- Break long lines at logical points
- Use backslash for multi-line function calls when necessary

### Braces
- Opening brace on same line as control statement
- Closing brace on its own line
- Align opening brace for multi-line blocks

### Spacing
- One space before parentheses in function calls
- One space after keywords (if, for, while, etc.)
- No spaces around array subscript operator []
- One space around binary operators (+, -, *, /, ==, !=, etc.)
- No spaces around increment/decrement operators (++, --)

## Error Handling

### Error Codes
- Use consistent error return values:
  - `true` for success
  - `false` for failure
  - Use specific error codes when needed

### Error Messages
- Include context in error messages
- Use descriptive error messages that help with debugging
- Include file and line number in error messages when possible

### Exception Safety
- Ensure proper cleanup in exception scenarios
- Use smart pointers to manage resources
- Implement RAII principles

## API Design

### Header Files
- Include necessary forward declarations
- Use `#pragma once` instead of include guards
- Document public API with comments
- Use inline function definitions for simple implementations

### Class Design
- Use RAII (Resource Acquisition Is Initialization)
- Provide copy constructors and assignment operators when needed
- Make move constructors and assignment operators `noexcept` when possible
- Use const-correctness

### Naming Conventions
- Private members: snake_case with leading underscore (e.g., `_config`, `_regions`)
- Protected members: camelCase (e.g., `Config`, `Logger`)
- Public members: PascalCase (e.g., `NeuronId`, `SimulationStep`)

## Comments

### Documentation Comments
- Use Doxygen-style comments for API documentation
- Document class purpose, parameters, return values, and exceptions
- Include copyright and license information in files

### Inline Comments
- Use comments to explain complex logic
- Avoid commenting obvious code
- Update comments when code changes

### Comment Formatting
- Start comments with `//` for line comments
- Use `/* */` for multi-line comments
- Place comments before the code they document

## Performance Considerations

### Memory Management
- Use smart pointers instead of raw pointers
- Minimize dynamic memory allocation in hot paths
- Consider object pooling for frequently created/destroyed objects
- Use const references for temporary objects

### CPU Optimization
- Minimize function call overhead in tight loops
- Use local variables when possible
- Consider using references instead of pointers
- Profile critical sections before optimizing

### Thread Safety
- Ensure thread-safe access to shared resources
- Use mutexes or atomic operations for shared data
- Avoid data races in concurrent code

## Testing

### Unit Tests
- Write comprehensive unit tests for all functions
- Test edge cases and error conditions
- Use consistent test naming conventions

### Integration Tests
- Test system-wide interactions
- Verify that all subsystems work together correctly

### Performance Tests
- Measure performance impact of changes
- Ensure performance does not degrade

## Build System

### CMake Configuration
- Use consistent CMake policies
- Specify required C++ standard (C++20)
- Use compiler warnings and treat them as errors
- Include proper dependency management

### Cross-Platform Compatibility
- Avoid platform-specific code when possible
- Use conditional compilation for platform-specific features
- Ensure portable file operations

## Code Review

### Pull Request Guidelines
- Ensure code follows all standards
- Run existing tests to verify changes
- Add unit tests for new functionality
- Update documentation when necessary
- Ensure no warnings are introduced

### Review Checklist
- [ ] Code follows naming conventions
- [ ] Error handling is consistent
- [ ] Comments are clear and up-to-date
- [ ] Tests pass
- [ ] Performance is not degraded
- [ ] Code is formatted correctly

## Enforcement

### Tools
- Use clang-format for code formatting
- Use clang-tidy for code analysis
- Use cppcheck for static analysis
- Use unit test frameworks for testing

### CI/CD
- Integrate linting tools into CI/CD pipeline
- Require all tests to pass before merging
- Automatically format code on commit
- Fail builds on code quality issues

## Migration Guide

### Existing Code
- Gradually apply standards to existing codebase
- Prioritize critical files (core functionality)
- Address most critical issues first
- Create coding standards compliance plan

### New Code
- Apply all standards from the start
- Follow guidelines in this document
- Review changes against standards
- Update this document as needed

## Conclusion

These coding standards ensure code quality, maintainability, and consistency across the NLM project. By following these standards, developers can write high-quality, efficient, and maintainable code that is easy to understand and modify.