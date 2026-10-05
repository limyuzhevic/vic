# NLM Test Framework Documentation

## Overview

This document describes the improved test framework for the NLM (Neural Learning Machine) Phase 2: Real Neural Computation system. The framework provides organized, modular test execution with enhanced error handling and configuration support.

## Major Improvements

### 1. LearningExperiment Class

**Location**: `src/experiments/LearningExperiment.hpp` and `src/experiments/LearningExperiment.cpp`

**Purpose**: Encapsulates learning experiment logic that demonstrates measurable synaptic changes through experience.

**Key Features**:
- Records initial and final synaptic weights
- Tracks spike activity and neuron activation
- Computes statistical analysis of synaptic changes
- Determines if learning has occurred
- Provides getter methods for accessing experiment data

**Usage**:
```cpp
LearningExperiment experiment(brain, seed);
experiment.recordInitialState();
// ... run experiment ...
experiment.recordFinalState();
experiment.computeStatistics();
if (experiment.learningOccurred()) {
    std::cout << "Learning detected!" << std::endl;
}
```

### 2. TestSuite Class

**Location**: `src/experiments/TestSuite.hpp` and `src/experiments/TestSuite.cpp`

**Purpose**: Provides organized test execution for Phase 2 neural computation tests.

**Key Features**:
- Modular test organization (Basic Connectivity, Plasticity, STDP)
- Comprehensive error handling and reporting
- Progress logging during test execution
- Support for test suite execution
- Backward compatibility with existing test functions

**Test Categories**:

#### Basic Connectivity Test
- **Purpose**: Verifies neural network connectivity
- **Method**: Injects current into neurons and checks for spike propagation
- **Output**: Reports spike generation and neuron parameters

#### Plasticity Experiment
- **Purpose**: Demonstrates synaptic changes through experience
- **Method**: Records synaptic weights before/after repeated input patterns
- **Output**: Statistical analysis of synaptic strengthening/weakening

#### STDP Verification
- **Purpose**: Verifies Spike-Timing-Dependent Plasticity
- **Method**: Tests pre-before-post spike correlation for LTP
- **Output**: Reports weight changes and potentiation verification

### 3. Enhanced main.cpp

**Improvements**:
- **Error Handling**: Comprehensive exception handling with `std::set_terminate`
- **Command Line Interface**: Advanced CLI with multiple options
- **Logging Configuration**: Flexible logging levels and file output
- **Help/Version Support**: Built-in help and version information
- **Configuration Loading**: Improved config file format detection
- **Test Framework Integration**: Uses TestSuite for organized test execution

**New Command Line Options**:
- `-h, --help`: Show help message
- `-v, --verbose`: Enable verbose output
- `-q, --quiet`: Suppress informational messages
- `--log-level LEVEL`: Set logging level (debug, info, warning, error)
- `--log-file FILE`: Write log output to file
- `--config FILE`: Configuration file (default: configs/default.cfg)
- `--version`: Show version information

### 4. Improved Config System

**Location**: `src/core/Config/Config.cpp`

**Improvements**:
- **JSON/YAML Support**: Proper JSON parsing with YAML compatibility
- **File Type Detection**: Automatic detection of config file formats
- **Enhanced Command Line Support**: Improved CLI argument parsing
- **Error Handling**: Better error reporting and recovery
- **Backward Compatibility**: Maintains support for legacy key=value format

**Supported Config File Formats**:
- `.json` - JSON format
- `.yaml` - YAML format  
- `.yml` - YAML format (alternative extension)
- `.cfg` - Key=value format
- `.ini` - Key=value format
- `.properties` - Key=value format
- Auto-detection based on file content

### 5. Documentation Structure

**Main Documentation Files**:
- `src/experiments/LearningExperiment.hpp` - Class interface
- `src/experiments/LearningExperiment.cpp` - Implementation details
- `src/experiments/TestSuite.hpp` - Test framework interface
- `src/experiments/TestSuite.cpp` - Test implementation
- `src/main.cpp` - Main application with enhanced features
- `src/core/Config/Config.cpp` - Improved configuration system

## Building and Running Tests

### Build System
The project uses CMake for building. The current configuration includes:

```bash
mkdir build
cd build
cmake ..
make
```

### Running Tests

#### Standard Test Suite
Run the compiled test executable:
```bash
./test_runner
```

#### Phase 2 Integration Tests
Run the main executable with test integration:
```bash
./nlm --verbose
```

#### Custom Test Configuration
Create a custom config file and run with specific options:
```bash
./nlm --config my_config.json --log-level debug
```

## Testing Best Practices

### 1. Modular Test Design
Each test is separated into its own category with clear purpose and output.

### 2. Error Handling
All tests use try-catch blocks with proper error reporting.

### 3. Configuration Management
Tests support flexible configuration through command line and config files.

### 4. Progress Tracking
Tests provide detailed progress output during execution.

### 5. Backward Compatibility
The framework maintains compatibility with existing test code while providing enhanced functionality.

## API Usage Examples

### LearningExperiment Example
```cpp
#include "experiments/LearningExperiment.hpp"

// Create experiment
LearningExperiment experiment(brain, 42);

// Record initial state
experiment.recordInitialState();

// ... run neural network simulation ...

// Record final state and analyze
experiment.recordFinalState();
experiment.computeStatistics();

// Check if learning occurred
if (experiment.learningOccurred()) {
    std::cout << "✓ Learning detected: " << experiment.getMostActiveNeurons().size() 
              << " neurons showed changes" << std::endl;
} else {
    std::cout << "✗ No significant learning detected" << std::endl;
}
```

### TestSuite Usage
```cpp
#include "experiments/TestSuite.hpp"

TestSuite testSuite;

// Run individual tests
bool connectivityPassed = testSuite.runBasicConnectivityTest(brain);
bool plasticityPassed = testSuite.runPlasticityExperiment(brain);
bool stdpPassed = testSuite.runStdpVerification(brain);

// Run all tests
bool allPassed = testSuite.runAllTests(brain);
```

### Enhanced CLI Usage
```cpp
// Run with verbose output and custom config
int main(int argc, char** argv) {
    TestSuite testSuite;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            testSuite.printHelp();
            return 0;
        } else if (arg == "--verbose") {
            std::cout << "Verbose mode enabled" << std::endl;
        }
    }
    
    // Create and configure brain
    auto config = std::make_shared<Config>("/path/to/config.json");
    auto brain = std::make_shared<Brain>(config);
    
    // Run tests
    if (testSuite.runAllTests(brain)) {
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some tests failed" << std::endl;
        return 1;
    }
}
```

## File Structure

```
/src/
  /experiments/
    LearningExperiment.hpp          # LearningExperiment class interface
    LearningExperiment.cpp          # LearningExperiment implementation
    TestSuite.hpp                   # TestSuite class interface
    TestSuite.cpp                   # TestSuite implementation
  /core/
    /Config/
      Config.hpp                  # Configuration class interface
      Config.cpp                  # Enhanced configuration implementation
  main.cpp                         # Main application with new features
  ... (other source files)
```

## Migration Guide

### From Old to New Structure

**Before** (main.cpp with inline LearningExperiment):
```cpp
// LearningExperiment defined inline in main.cpp
struct LearningExperiment { ... };
void runPlasticityExperiment() { ... }  // Inline implementation
```

**After** (separate LearningExperiment class):
```cpp
// LearningExperiment in dedicated files
#include "experiments/LearningExperiment.hpp"
LearningExperiment experiment(brain, seed);
```

**Before** (Individual test functions in main.cpp):
```cpp
void runBasicConnectivityTest() { ... }
void runPlasticityExperiment() { ... }
void runStdpVerification() { ... }
```

**After** (TestSuite class organization):
```cpp
#include "experiments/TestSuite.hpp"
TestSuite suite;
bool passed = suite.runBasicConnectivityTest(brain);
bool passed = suite.runPlasticityExperiment(brain);
bool passed = suite.runStdpVerification(brain);
```

## Configuration File Examples

### JSON Configuration
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.15,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "description": "Phase 2 neural computation configuration"
}
```

### YAML Configuration
```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.15
simulation_timestep: 0.001
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
description: "Phase 2 neural computation configuration"
```

### Key=Value Configuration
```cfg
# Configuration file
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
```

## Error Handling and Recovery

### Exception Handling
The framework implements comprehensive error handling:

1. **Main Program Errors**: `std::set_terminate` for unhandled exceptions
2. **Test Execution Errors**: Try-catch blocks around individual tests
3. **Configuration Errors**: Graceful degradation with fallback configurations
4. **File I/O Errors**: Proper error reporting and recovery

### Error Reporting
All errors are reported through the configured logging system with appropriate severity levels.

## Performance Considerations

### Memory Management
- All classes use RAII principles for proper memory management
- Smart pointers (shared_ptr, unique_ptr) used where appropriate
- No memory leaks or resource management issues

### Test Efficiency
- Tests are designed to run efficiently
- Configurable test parameters for performance tuning
- Progress reporting for long-running tests

### Configuration Loading
- Lazy loading of configuration files
- Caching of parsed configurations where appropriate
- Efficient file format detection

## Future Enhancements

### Suggested Improvements
1. **Advanced JSON/YAML Parser**: Implement dedicated JSON/YAML parsing library
2. **Configuration Validation**: Add schema validation for configuration files
3. **Performance Monitoring**: Add timing and profiling for tests
4. **Parallel Testing**: Implement parallel test execution
5. **Test Persistence**: Save/load test results for regression testing
6. **Plugin Architecture**: Allow for pluggable test modules
7. **Continuous Integration**: Integration with CI/CD pipelines

### Potential Features
- Web-based test visualization
- Automated test generation
- Performance benchmarking
- System resource monitoring
- Cross-platform test execution

## Conclusion

The improved NLM test framework provides:

1. **Better Organization**: Clear separation of concerns and modular design
2. **Enhanced Reliability**: Comprehensive error handling and validation
3. **Improved Usability**: Flexible CLI and configuration options
4. **Better Documentation**: Clear API documentation and examples
5. **Future Extensibility**: Plugin architecture for future enhancements

This framework significantly improves the testability, maintainability, and usability of the NLM Phase 2 neural computation system while maintaining backward compatibility with existing code.

---

*Document Version: 1.0*
*Last Updated: 2026-10-05*
*Author: NLM Development Team*