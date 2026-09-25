# Phase 6 Integration Tests - Quick Start Guide

This guide provides quick instructions for building and running the Phase 6 integration tests for the NLM brain simulation.

## Overview

The Phase 6 integration tests validate all Phase 6 integration improvements for the NLM brain simulation. The tests verify that all brain systems work together as a coherent artificial brain.

## Required Components

To build and run the Phase 6 tests, you need:

1. NLM source code (this repository)
2. CMake 3.16 or higher
3. C++20 compatible compiler
4. Build tools (make, ninja, etc.)

## Building Phase 6 Tests

### Standard Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
# Choose your preferred build type:
# - Release (recommended)
# - Debug
# - RelWithDebInfo

cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
# Use -jN for parallel build
make -j4
```

### Building Individual Test Components

The Phase 6 tests are organized into separate executables:

```bash
# Build all Phase 6 tests
make -j4

# Build individual test (example)
make test_brain_integration
make test_memory_integration
make test_neuromodulation_integration
make test_main
```

## Running Phase 6 Tests

### Run All Phase 6 Tests

```bash
# Run the main test suite (recommended)
./test_main

# Or run all tests via CTest
ctest
```

### Run Individual Tests

```bash
# Run specific test executable
./test_brain_integration
./test_memory_integration
./test_prediction_integration
./test_neuromodulation_integration
./test_cognition_integration
./test_development_integration
./test_persistence_integration
./test_stress_integration
```

### Run Tests with CTest

```bash
# List all tests
ctest -N

# Run tests with verbose output
ctest --output-on-failure

# Run specific test
ctest -R test_brain_integration
```

## Test Categories

The Phase 6 test suite includes the following categories:

### 1. Brain Integration Tests
- **File**: `test_brain_integration.cpp`
- **Tests**: Brain integration, memory data exchange
- **Purpose**: Verify all systems work together in Brain::step()

### 2. Memory System Tests
- **File**: `test_memory_integration.cpp`
- **Tests**: Working memory, episodic memory, consolidation, replay
- **Purpose**: Test memory systems functionality

### 3. Neuromodulation Tests
- **File**: `test_neuromodulation_integration.cpp`
- **Tests**: Adenosine, dopamine, acetylcholine, NE, 5-HT
- **Purpose**: Test neuromodulator effects and integration

### 4. Cognitive System Tests
- **File**: `test_cognition_integration.cpp`
- **Tests**: Neural planner, concept formation, attention
- **Purpose**: Test cognitive system integration

### 5. Development and Rest Tests
- **File**: `test_development_integration.cpp`
- **Tests**: Rest cycles, memory consolidation, developmental stages
- **Purpose**: Test development and rest functionality

### 6. Persistence Tests
- **File**: `test_persistence_integration.cpp`
- **Tests**: Save/load, checkpoint system
- **Purpose**: Test persistence and checkpoint functionality

### 7. Stress Tests
- **File**: `test_stress_integration.cpp`
- **Tests**: Cross-system stress, error conditions, recovery
- **Purpose**: Test system stability under stress

## Test Output

Each test provides detailed output including:

- Test progress indicators
- System state information
- Performance metrics
- Error messages for failures
- Final test summary

Example output:
```
=== Running All Phase 6 Integration Tests ===

=== BRAIN INTEGRATION TESTS ===
Running test_basic_brain_integration... PASSED (1.234s)
Running test_memory_data_exchange... PASSED (2.567s)

=== NEUROMODULATION INTEGRATION TESTS ===
Running test_adenosine_rest_cycle... PASSED (0.987s)
Running test_dopamine_effects... PASSED (1.456s)
Running test_acetylcholine_effects... PASSED (1.234s)

... (more tests) ...

=== OVERALL TEST RESULTS ===
Total tests: 7
Tests passed: 7
Tests failed: 0
Success rate: 100.00%
Total time: 12.345 seconds

=== ALL TESTS PASSED ===
```

## Test Configuration

The tests use a configurable test infrastructure with predefined scenarios:

### Available Test Scenarios

The `TestConfigManager` class provides the following predefined scenarios:

1. **basic_integration** - Basic integration test
   - Max steps: 1000
   - Neurons: 500
   - Regions: 1
   - Replay: false
   - Development: false

2. **memory_intensive** - Memory-intensive test
   - Max steps: 2000
   - Neurons: 1000
   - Regions: 2
   - Replay: true
   - Development: true

3. **neuromodulation_heavy** - Neuromodulation-heavy test
   - Max steps: 1500
   - Neurons: 750
   - Regions: 1
   - Development: true

4. **cognition_heavy** - Cognitive-heavy test
   - Max steps: 2500
   - Neurons: 1200
   - Regions: 2
   - Replay: true
   - Development: true

5. **stress_test** - Stress test
   - Max steps: 5000
   - Neurons: 2000
   - Regions: 3
   - Replay: true
   - Development: true

6. **persistence_test** - Persistence test
   - Max steps: 1000
   - Neurons: 300
   - Checkpoint: true
   - Replay: false
   - Development: false

### Customizing Tests

To customize tests:

1. Modify `Phase6TestConfig` in `brain_test_common.hpp`
2. Add custom scenarios in `test_config.hpp`
3. Modify test parameters in individual test files

Example customization:

```cpp
// In brain_test_common.hpp
struct Phase6TestConfig {
    uint64_t maxSteps;           // Default: 1000
    size_t neuronCount;          // Default: 500
    size_t regionCount;          // Default: 1
    float connectionProbability; // Default: 0.1f
    bool enableCheckpointing;    // Default: false
    bool enableReplay;           // Default: false
    bool enableDevelopment;      // Default: false
    std::string checkpointPath; // Default: "/tmp/nlm_test_checkpoint.bin"
};
```

## Test Implementation Priority

Tests are organized by implementation priority:

1. **Priority 1 (Critical)**:
   - Brain integration tests
   - Memory integration tests
   - Neuromodulation integration tests

2. **Priority 2 (Important)**:
   - Prediction integration tests
   - Cognition integration tests
   - Development integration tests

3. **Priority 3 (Comprehensive)**:
   - Persistence integration tests
   - Stress integration tests
   - Cross-system integration tests

## Troubleshooting

### Common Issues

1. **CMake not found**
   ```bash
   # Install CMake
   sudo apt-get install cmake  # Ubuntu/Debian
   sudo yum install cmake     # CentOS/RHEL
   # Or download from https://cmake.org/
   ```

2. **Compiler issues**
   ```bash
   # Check compiler version
   g++ --version
   clang++ --version
   ```
   Ensure you have a C++20 compatible compiler.

3. **Build failures**
   ```bash
   # Try clean build
   rm -rf build
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

4. **Missing headers**
   ```bash
   # Check include paths
   ls src/
   find . -name "*.hpp" -o -name "*.h"
   ```

### Getting Help

If you encounter issues:

1. Check the output for error messages
2. Verify CMake version and compiler
3. Ensure all required files are present
4. Check build directory for logs
5. Refer to NLM documentation for more details

## Additional Information

### Test Framework

The test framework provides:
- Common utilities (`brain_test_common.hpp`)
- Configuration management (`test_config.hpp`)
- Performance measurement
- Statistics collection
- Detailed test reporting

### Integration with Existing Tests

The Phase 6 tests are integrated with the existing test suite:
- Located in `tests/phase6/` directory
- Listed in `tests/CMakeLists.txt`
- Can be run alongside existing Phase 2 tests

### Test Coverage

The tests provide comprehensive coverage of:
- Brain::step() integration
- Memory system interactions
- Neuromodulator effects
- Cognitive system integration
- Development and rest functionality
- Persistence and checkpointing
- Stress and error handling

## Conclusion

The Phase 6 integration tests provide comprehensive validation of all Phase 6 improvements. The tests ensure that:

1. All brain systems are properly integrated
2. Memory systems correctly exchange data
3. Prediction and neuromodulation systems affect neural computation
4. Cognitive systems properly integrate with other components
5. Development and rest cycles work correctly
6. Persistence mechanisms properly restore system states
7. Systems handle stress and error conditions gracefully

The test suite follows best practices and provides detailed reporting to help diagnose integration issues. It's ready for use in validating the NLM Phase 6 integration improvements.

## License

This test suite is part of the NLM project and follows the MIT License.
