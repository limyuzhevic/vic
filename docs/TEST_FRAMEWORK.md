# NLM Phase 2 Test Framework Documentation

## Overview

This document describes the improved test framework for NLM (Neural Learning Machine) Phase 2. The framework provides a modular, extensible approach to testing neural network functionality with comprehensive error handling, configurable parameters, and detailed reporting.

## What Was Improved

### 1. Code Organization and Structure
- **Separated concerns**: Test logic is now in dedicated files (`BasicConnectivityTest.hpp`, `PlasticityExperimentTest.hpp`, `StdpVerificationTest.hpp`)
- **Test utilities**: Created `TestUtils.hpp` and `TestUtils.cpp` with common testing functionality
- **Framework architecture**: Introduced `TestBase`, `SimpleTest`, and `TestRunner` classes for structured testing

### 2. Configurable Parameters
- **TestConfig structure**: Centralized all magic numbers into configurable parameters
- **Command-line interface**: Added comprehensive command-line options for test configuration
- **Parameter validation**: Built-in validation for all configuration parameters

### 3. Error Handling
- **Enhanced error handling**: Added `safeExecute` template function for exception-safe execution
- **Comprehensive validation**: Parameter validation with detailed error messages
- **Graceful degradation**: Tests continue even if some validation checks fail

### 4. Logging and Reporting
- **Improved logging**: Better formatted output with consistent structure
- **Progress reporting**: Detailed progress tracking during long-running tests
- **Comprehensive results**: Summary reports showing passed/failed tests with execution times

### 5. Extensibility
- **Test base classes**: Easy to add new test types by inheriting from `TestBase` or `SimpleTest`
- **Plugin architecture**: Test registration system for future test discovery
- **Configuration flexibility**: Tests can be easily parameterized and customized

## Usage Examples

### Running Tests with Default Configuration

```bash
./nlm_main
```

### Running with Custom Parameters

```bash
./nlm_main --verbose --plasticity-steps=2000 --stdp-trials=100 --config=my_config.cfg
```

### Running Specific Test Only

```bash
./nlm_main --single-test="Basic Connectivity"
```

### Getting Help

```bash
./nlm_main --help
```

## Test Categories

### Basic Connectivity Test
Tests that neural signals can propagate through the network:
- Injects current into sensory neurons
- Runs simulation for configurable steps
- Verifies spike generation

### Plasticity Experiment Test
Tests that synaptic plasticity (learning) occurs:
- Enables Hebbian and STDP plasticity
- Applies repeated input patterns
- Records and analyzes brain activity

### STDP Verification Test
Tests pre-before-post spike activity (LTP):
- Enables only STDP plasticity
- Creates correlated pre->post activity
- Verifies synaptic weight changes

## Configuration Parameters

### Connectivity Test Parameters
- `connectivityNeuronCount`: Number of neurons to inject current into (default: 10)
- `connectivityCurrent`: Current injection strength (default: 50.0f)
- `connectivitySteps`: Number of simulation steps (default: 50)

### Plasticity Experiment Parameters
- `plasticitySteps`: Number of learning steps (default: 1000)
- `plasticityNeuronCount`: Number of neurons for input pattern (default: 20)
- `plasticityCurrent`: Current for input pattern (default: 30.0f)

### STDP Verification Parameters
- `stdpSynapseCount`: Number of synapses to test (default: 5)
- `stdpNeuronCount`: Number of neurons for STDP test (default: 2)
- `stdpCurrent`: Current injection strength (default: 60.0f)
- `stdpTrials`: Number of STDP trial iterations (default: 50)

### Statistical Parameters
- `weightChangeThreshold`: Minimum weight change to consider significant (default: 0.01f)
- `learningThreshold`: Minimum mean weight change to detect learning (default: 0.001f)
- `mostActiveNeuronCount`: Number of top neurons to track (default: 10)

## File Structure

```
/src/
  /test/                    # Test framework files
    TestUtils.hpp           # Core test utilities
    TestUtils.cpp           # Test utility implementations
    BasicConnectivityTest.hpp # Basic connectivity test
    PlasticityExperimentTest.hpp # Plasticity learning test
    StdpVerificationTest.hpp   # STDP verification test
  main.cpp                 # Updated test runner with new framework
```

## Benefits of the Improved Framework

1. **Maintainability**: Code is easier to understand and modify
2. **Testability**: Tests are modular and can be run independently
3. **Configurability**: All parameters are easily configurable
4. **Error Resilience**: Comprehensive error handling prevents crashes
5. **Extensibility**: Easy to add new test types and features
6. **Documentation**: Self-documenting code with clear comments
7. **Performance**: Better organized code for easier optimization

## Backward Compatibility

The improved framework maintains backward compatibility:
- All original functionality is preserved
- Default parameters match original behavior
- Command-line interface is enhanced but compatible
- Existing tests produce the same results

## Future Enhancements

The framework is designed to support future improvements:
- Test result persistence to files
- Parallel test execution
- Automated test discovery
- Integration with continuous integration systems
- Performance benchmarking
- Stress testing capabilities

## Technical Details

### Dependencies
- Standard C++20 features
- No external dependencies beyond the NLM framework
- Compatible with existing build system

### Performance Considerations
- Tests measure and report execution time
- Memory usage is monitored
- Configurable step counts for performance tuning
- Progress logging prevents excessive I/O

### Code Quality
- Follows NLM coding standards
- Comprehensive error checking
- Type safety with modern C++ practices
- Memory management with smart pointers

## Testing the Improved Framework

To verify the improvements work correctly:

1. **Compile the code**: Run the build system
2. **Run basic tests**: Execute `./nlm_main` to run all tests
3. **Test configuration**: Use command-line options to verify parameter parsing
4. **Check error handling**: Test with invalid parameters to verify graceful handling
5. **Validate results**: Compare output with original implementation

The improved test framework provides a solid foundation for future NLM development while maintaining all existing functionality.