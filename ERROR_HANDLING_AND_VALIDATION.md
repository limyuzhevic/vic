# NLM Error Handling and Validation

This document provides comprehensive documentation for the error handling and validation modules in the NLM framework.

## Overview

This module provides comprehensive error handling and validation utilities for the NLM (Neural Learning Machine) framework, including:

1. **Exception Classes** - Custom exception types for different error categories
2. **Validation Functions** - Input validation for configuration, simulation parameters
3. **Error Handling Utilities** - Error reporting, logging, and recovery mechanisms
4. **Configuration Validation** - Validate configuration files and parameter values
5. **Simulation Validation** - Validate simulation parameters and state
6. **Performance Validation** - Validate performance metrics and resource usage
7. **Memory Validation** - Validate memory usage and allocation
8. **Error Recovery** - Error recovery and fallback mechanisms

## Key Features

### Exception Hierarchy
- Base `NLMException` class
- Specific exception types (`ConfigurationException`, `SimulationException`, etc.)
- Error codes and severity levels

### Validation Systems
- Configuration validation
- Simulation validation  
- Memory validation
- Performance validation
- Parallel processing validation

### Error Handling
- Comprehensive error reporting
- Error recovery mechanisms
- Logging and monitoring
- User-friendly error messages

### Integration Support
- Easy integration with existing NLM code
- Macros for common error handling patterns
- Support for different configuration file formats

## Files

This module includes the following files:

- `error_handling.md` - Detailed documentation for error handling
- `validation.md` - Detailed documentation for validation
- `exceptions.md` - Documentation for exception classes
- `error_reporting.md` - Documentation for error reporting utilities
- `performance_validation.md` - Documentation for performance validation
- `memory_validation.md` - Documentation for memory validation

## Usage Examples

### Basic Error Handling

```cpp
#include "error_handling/error_handling.h"

int main() {
    try {
        // Your simulation code here
        brain->step(step);
    } catch (const nlm::NLMException& e) {
        nlm::ErrorReporter::logError(
            nlm::ErrorReporter::createErrorReport(
                e.getErrorCode(),
                nlm::ErrorReporter::ErrorSeverity::Error,
                e.what(),
                "main",
                std::vector<std::string>{"step = " + std::to_string(step)}
            )
        );
        return 1;
    }
    
    return 0;
}
```

### Configuration Validation

```cpp
#include "validation/configuration_validator.h"

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Load configuration
    if (!config->loadFromFile("config.json")) {
        throw nlm::ConfigurationException("Failed to load configuration file");
    }
    
    // Validate configuration
    auto validation_result = nlm::ConfigurationValidator::validateConfiguration(*config);
    
    if (!validation_result.valid) {
        std::cerr << "Configuration validation failed:" << std::endl;
        for (const auto& error : validation_result.errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return 1;
    }
    
    // Continue with simulation
    return 0;
}
```

### Performance Monitoring

```cpp
#include "performance/performance_monitor.h"

int main() {
    // Start performance monitoring
    nlm::PerformanceMonitor monitor;
    monitor.startMonitoring();
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Stop performance monitoring
    monitor.stopMonitoring();
    
    // Get performance metrics
    auto stats = monitor.getCurrentPerformanceStats();
    nlm::PerformanceValidator::printPerformanceStats(stats);
    
    return 0;
}
```

## Best Practices

### Error Handling
1. **Use Custom Exceptions**: Always use custom exception types for better error handling
2. **Provide Detailed Error Messages**: Include context information in error messages
3. **Implement Recovery Strategies**: Provide recovery mechanisms where possible
4. **Log Errors**: Always log errors for debugging and monitoring
5. **Validate Input**: Validate all inputs before processing

### Validation
1. **Early Validation**: Validate inputs early in the execution flow
2. **Comprehensive Validation**: Validate all aspects of the system
3. **Performance Consideration**: Balance validation thoroughness with performance
4. **Clear Error Messages**: Provide clear, actionable error messages
5. **Recovery Options**: Include recovery options for validation failures

### Integration
1. **Use Macros**: Use provided macros for common error handling patterns
2. **Consistent API**: Use consistent API across the codebase
3. **Configuration-Driven**: Support configuration-driven behavior
4. **Extensible Design**: Design for easy extension and customization
5. **Documentation**: Provide comprehensive documentation for all components

## Files

This module includes the following source files:

### Core Modules
- `error_handling_base.cpp` - Base error handling functionality
- `exception_classes.cpp` - Exception class implementations
- `error_reporting.cpp` - Error reporting utilities
- `error_recovery.cpp` - Error recovery mechanisms

### Validation Modules
- `configuration_validator.cpp` - Configuration validation
- `simulation_validator.cpp` - Simulation validation
- `memory_validator.cpp` - Memory validation
- `performance_validator.cpp` - Performance validation
- `parallel_validator.cpp` - Parallel processing validation

### Integration Modules
- `error_handling_macros.cpp` - Error handling macros
- `validation_macros.cpp` - Validation macros
- `integration.cpp` - Integration utilities

### Documentation
- `error_handling.md` - Main documentation
- `validation.md` - Validation documentation
- `exceptions.md` - Exception documentation
- `error_reporting.md` - Error reporting documentation
- `performance_validation.md` - Performance validation documentation
- `memory_validation.md` - Memory validation documentation
- `parallel_validation.md` - Parallel validation documentation

## Build System

The error handling and validation module is built as part of the NLM framework:

### CMake Configuration

```cmake
# Add error handling and validation module
add_subdirectory(src/error_handling)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/error_handling
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

# Link libraries
link_libraries(nlm_core)
```

### Build Targets

- `nlm_error_handling` - Core error handling library
- `nlm_validation` - Validation library
- `nlm_performance` - Performance monitoring library
- `nlm_memory` - Memory validation library

## Testing

The error handling and validation module includes comprehensive tests:

### Unit Tests
- Configuration validation tests
- Exception handling tests
- Error reporting tests
- Performance monitoring tests
- Memory validation tests

### Integration Tests
- End-to-end error handling tests
- Cross-module integration tests
- Performance validation tests
- Real-world scenario tests

### Test Framework

```bash
# Run all tests
make test

# Run specific test modules
make test_error_handling
make test_validation
make test_performance
make test_memory

# Run tests with specific options
ctest --output-on-failure
```

## Installation

### Dependencies
- C++20 compatible compiler
- Standard library
- CMake 3.16 or higher

### Installation Steps

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Install (optional)
sudo make install
```

### Python Bindings

The error handling and validation module also includes Python bindings:

```python
import pynlm

# Create advanced configuration with validation
config = pynlm.createAdvancedConfig()

# Validate configuration
validation_result = config.validate()
if not validation_result['valid']:
    print("Configuration validation failed:")
    for error in validation_result['errors']:
        print(f"  - {error}")
    exit(1)

# Create advanced brain
brain = pynlm.createAdvancedBrain(config)

# Initialize brain
if not brain.initialize():
    print("Failed to initialize brain")
    exit(1)

# Run simulation with error handling
for step in range(1000):
    try:
        brain.step(step)
    except Exception as e:
        print(f"Error at step {step}: {e}")
        break

print("Simulation completed successfully")
```

## Configuration

### Configuration File Example

```json
{
    "error_handling": {
        "log_level": "INFO",
        "log_file": "nlm_errors.log",
        "max_log_entries": 10000,
        "enable_error_reporting": true,
        "enable_error_recovery": true,
        "recovery_strategy": "fallback",
        "max_recovery_attempts": 3,
        "enable_performance_monitoring": true,
        "performance_threshold_ms": 1000
    },
    "validation": {
        "strict_mode": false,
        "auto_fix_enabled": true,
        "validation_rules": [
            "required_fields",
            "parameter_ranges",
            "type_checks",
            "conflict_detection"
        ]
    }
}
```

## License

This module is part of the NLM (Neural Learning Machine) framework and is licensed under the MIT License.

```
MIT License

Copyright (c) 2026 NLM Project Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furniciously furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Acknowledgements

This error handling and validation module was developed as part of the NLM Phase 6 (Final Integration) project, building upon the existing NLM framework with comprehensive error handling and validation capabilities.

The module provides a foundation for creating robust, fault-tolerant neural simulation applications with extensive error handling and validation support.

## References

1. [C++ Exception Handling](https://en.cppreference.com/w/cpp/error/exception)
2. [Error Handling Best Practices](https://en.wikipedia.org/wiki/Error_handling)
3. [Validation Techniques](https://en.wikipedia.org/wiki/Validation_(software_engineering))
4. [Performance Monitoring](https://en.wikipedia.org/wiki/Monitoring_(computer_systems))
5. [Error Recovery Strategies](https://en.wikipedia.org/wiki/Fault_tolerance)

## Contact

For questions, issues, or contributions:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Support: nlm-support@nlm-project.org

This documentation provides comprehensive guidance for implementing robust error handling and validation in NLM applications.