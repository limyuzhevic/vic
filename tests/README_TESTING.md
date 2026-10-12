# NLM Testing Framework

## Overview

The NLM (熙然) project is an experimental artificial brain project with Phase 6 representing the final integration of all systems into a coherent brain loop. This document describes the comprehensive testing framework for the project.

## Testing Architecture

The NLM testing framework is organized into three main categories:

### 1. Core Neural Component Tests (Phase 2)
- **Location**: `tests/` directory
- **Focus**: Basic neural computation and plasticity
- **Scope**: LIF neuron dynamics, synaptic transmission, STDP plasticity
- **Purpose**: Test the fundamental building blocks of the neural system

### 2. Integration Tests (Phase 6)
- **Location**: `tests/test_integration.cpp`
- **Focus**: Complete system integration
- **Scope**: Memory systems, neuromodulation, prediction, cognition, development, plasticity
- **Purpose**: Verify that all systems work together as a coherent brain

### 3. Performance and Stress Tests
- **Location**: `tests/test_integration.cpp`
- **Focus**: System performance and robustness
- **Scope**: Initialization performance, step performance, memory usage, large scale networks
- **Purpose**: Ensure the system performs well under various conditions

## Test Components

### Core Neural Component Tests
The existing test suite includes:

- `test_types.cpp`: Type system tests
- `test_config.cpp`: Configuration system tests
- `test_random.cpp`: Random number generation tests
- `test_clock.cpp`: Simulation clock tests
- `test_neuron.cpp`: Neuron dynamics tests
- `test_synapse.cpp`: Synaptic transmission tests
- `test_stdp.cpp`: STDP plasticity tests
- `test_brain.cpp`: Brain integration tests
- `test_main.cpp`: Main test runner

### Integration Tests
The new integration test framework includes:

- `test_integration.hpp`: Integration test framework and utilities
- `test_integration.cpp`: Implementation of integration tests
- `test_example.cpp`: Example tests demonstrating framework usage
- `test_documentation.hpp`: Comprehensive documentation for the testing framework

### Test Utilities
The testing framework provides:

- `BrainTestFixture`: Test fixture for brain-related tests
- `TestConfig`: Configuration for tests
- `TestRunner`: Comprehensive test runner class
- `test_utils`: Utility functions for tests

## Running Tests

### Prerequisites

1. Build the NLM project:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. Ensure the `nlm` executable is built and available.

### Running the Complete Test Suite

```bash
cd tests
./nlm_test
```

Or if using CMake:

```bash
cd build
make test
```

### Running Individual Test Components

You can run specific test components by modifying the `test_main.cpp` file:

```cpp
// To run only integration tests:
try {
    test_integration::TestRunner::getInstance().runAllTests();
    std::cout << "  PASSED" << std::endl;
} catch (const std::exception& e) {
    std::cout << "  FAILED: " << e.what() << std::endl;
    allPassed = false;
}
```

## Test Results

The test suite reports results in the following format:

```
=== NLM Test Suite ===
Phase 2: Real Neural Computation Tests

Testing Core Neural Components:
  - LIF Neuron Dynamics
  - Synaptic Transmission
  - STDP Plasticity

Testing Integration Components:
  - Memory Systems
  - Neuromodulation
  - Prediction Systems
  - Cognitive Mechanisms
  - Development
  - Spike Processing
  - Complete Brain Loop

Running Core Neural Component tests...
  PASSED

Running Integration tests...
  PASSED

=== All Tests PASSED ===
```

## Test Coverage

### Core Neural Components
- **Types**: Type system, constants, and enums
- **Config**: Configuration management and validation
- **Random**: Random number generation and seeding
- **Clock**: Simulation timing and synchronization
- **Neuron**: LIF neuron dynamics and spike generation
- **Synapse**: Synaptic transmission and plasticity
- **STDP**: Spike-Timing-Dependent Plasticity
- **Brain**: Brain integration and coordination

### Phase 6 Integration
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Prediction Systems**: Neural prediction and error computation
- **Cognitive Mechanisms**: Planning, attention, concept formation
- **Development**: Developmental stages and plasticity modulation
- **Plasticity Systems**: STDP, Hebbian, structural plasticity
- **Spike Processing**: Event-driven spike propagation
- **Complete Brain Loop**: Full integration and coordination

### Performance and Stress
- **Initialization Performance**: Brain setup and configuration
- **Step Performance**: Simulation step execution
- **Memory Usage**: Memory consumption and management
- **Spike System Performance**: Spike processing and delivery
- **Plasticity Performance**: Plasticity rule application
- **Large Scale Networks**: Network scalability
- **High Spike Activity**: High-frequency neural activity
- **Memory Growth**: Memory system growth and consolidation
- **Plasticity Rate**: Plasticity dynamics and learning

## Writing Tests

### Test Structure

The testing framework follows a modular structure:

1. **Test Fixtures**: `BrainTestFixture` manages test state
2. **Test Configuration**: `TestConfig` provides test parameters
3. **Test Runners**: `TestRunner` orchestrates test execution
4. **Test Utilities**: `test_utils` provides helper functions

### Basic Test Example

```cpp
#include "test_integration.hpp"

namespace test_example {
    void testBrainInitialization() {
        BrainTestFixture fixture;
        
        if (!fixture.setup()) {
            throw std::runtime_error("Setup failed");
        }
        
        auto brain = fixture.getBrain();
        assert(brain != nullptr);
        assert(brain->initialize());
        assert(brain->getTotalNeuronCount() > 0);
        
        fixture.teardown();
    }
    
    void runAll() {
        testBrainInitialization();
    }
}
```

### Integration Test Example

```cpp
namespace phase6_tests {
    void testMemoryIntegration(BrainTestFixture& fixture) {
        auto brain = fixture.getBrain();
        assert(brain->getWorkingMemory() != nullptr);
        assert(brain->getEpisodicMemory() != nullptr);
        assert(brain->getAssociativeMemory() != nullptr);
        
        // Test memory system functionality
        for (size_t i = 0; i < 10; ++i) {
            brain->step(i, 0.001f * i);
        }
    }
}
```

### Performance Test Example

```cpp
namespace performance_tests {
    void testStepPerformance(BrainTestFixture& fixture) {
        auto brain = fixture.getBrain();
        auto startTime = test_utils::getCurrentTime();
        
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step, 0.001f * step);
        }
        
        auto endTime = test_utils::getCurrentTime();
        double elapsed = endTime - startTime;
        
        std::cout << "Performance: " << elapsed << " seconds for 100 steps" << std::endl;
    }
}
```

## Test Development Guidelines

### 1. Test Isolation
- Each test should be independent
- Use test fixtures to manage test state
- Clean up resources in teardown

### 2. Test Assertions
- Use `assert()` for simple checks
- Use exceptions for test failures
- Include descriptive error messages

### 3. Test Environment
- Use test configuration for test parameters
- Clean up temporary files
- Use consistent timing

### 4. Test Documentation
- Document test purpose and expected behavior
- Include test preconditions and postconditions
- Provide examples of expected output

## Test Files

### Core Neural Tests
- `test_types.cpp`: Type system tests
- `test_config.cpp`: Configuration system tests
- `test_random.cpp`: Random number generation tests
- `test_clock.cpp`: Simulation clock tests
- `test_neuron.cpp`: Neuron dynamics tests
- `test_synapse.cpp`: Synaptic transmission tests
- `test_stdp.cpp`: STDP plasticity tests
- `test_brain.cpp`: Brain integration tests
- `test_main.cpp`: Main test runner

### Integration Tests
- `test_integration.hpp`: Integration test framework
- `test_integration.cpp`: Integration test implementation
- `test_example.cpp`: Example tests
- `test_documentation.hpp`: Testing documentation

## Test Dependencies

The testing framework depends on:

1. **C++ Standard Library**: Core functionality
2. **NLM Library**: Brain system implementation
3. **CMake**: Build system
4. **Compiler**: C++11 or later

## Building Tests

### Using CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
make test
```

### Using Makefile

```bash
cd tests
g++ -std=c++11 -I../src -I../src/core -I../src/brain -c test_*.cpp
./nlm_test
```

## Test Troubleshooting

### Common Issues

1. **Linkage Errors**: Ensure all required libraries are linked
2. **Compilation Errors**: Check for missing headers or incorrect includes
3. **Runtime Errors**: Verify test fixture setup and teardown
4. **Memory Issues**: Check for memory leaks and proper cleanup

### Debugging Tips

1. **Add Debug Output**: Use `std::cout` for debug information
2. **Use Assertions**: Use `assert()` for simple checks
3. **Exception Handling**: Catch and report exceptions
4. **Memory Validation**: Check for null pointers and valid state

## Future Enhancements

### 1. Unit Test Framework
- Add more comprehensive unit test framework
- Implement test parameterization
- Add test fixtures for common scenarios

### 2. Performance Testing
- Add detailed performance benchmarks
- Implement stress testing with configurable parameters
- Add memory leak detection

### 3. Integration Testing
- Add cross-component integration tests
- Test system interactions under various conditions
- Test error conditions and recovery

### 4. Continuous Integration
- Add CI/CD pipeline for test automation
- Implement automated testing on commits
- Add performance regression testing

## License

The NLM testing framework is part of the NLM project and is licensed under the MIT License.

## Contact

For questions or issues with the testing framework, please contact the NLM project team.

## Version

Version 1.0.0 (Phase 6 Implementation)