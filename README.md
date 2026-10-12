# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Testing

See the [Testing Section](#testing) for comprehensive information about the testing framework, how to build tests, and how to run them.

## Testing

### Overview
NLM provides a comprehensive testing framework to ensure code quality, reliability, and maintainability. The testing infrastructure includes unit tests, integration tests, performance tests, and reporting capabilities.

### Test Structure

The test infrastructure is organized into several categories:

#### Unit Tests
Unit tests verify individual components in isolation:

- **ConfigBuilder Tests** (`tests/unit/TestConfigBuilder.hpp/cpp`): Tests configuration builder functionality
- **Brain Core Tests** (`tests/unit/TestBrainCore.hpp/cpp`): Tests neural components (neurons, synapses, brain regions)
- **Memory System Tests** (`tests/unit/TestMemorySystems.hpp/cpp`): Tests working memory and episodic memory systems
- **Neuromodulation Tests** (`tests/unit/TestNeuromodulation.hpp/cpp`): Tests reward, prediction error, curiosity, and novelty systems
- **Cognition Tests** (`tests/unit/TestCognition.hpp/cpp`): Tests concept formation and neural planning
- **Plasticity Tests** (`tests/unit/TestPlasticity.hpp/cpp`): Tests STDP and Hebbian plasticity rules

#### Integration Tests
Integration tests verify system components working together:

- **Phase 2 Integration Tests** (`tests/integration/TestPhase2Integration.hpp/cpp`): Tests LIF dynamics and STDP integration
- **Phase 6 Integration Tests** (`tests/integration/TestPhase6Integration.hpp/cpp`): Tests complete system integration
- **End-to-End Tests** (`tests/integration/TestEndToEnd.hpp/cpp`): Tests complete system behavior

#### Performance Tests
Performance tests measure system efficiency and scalability:

- **Performance Tests** (`tests/performance/TestPerformance.hpp/cpp`): Performance benchmarks and profiling

#### Test Framework and Utilities
Framework and utility components that support testing:

- **Test Framework** (`tests/Framework/TestFramework.hpp/cpp`): Core testing framework with assertions and test organization
- **Test Helpers** (`tests/Helpers/TestHelpers.hpp/cpp`): Common test utilities and data generators
- **Test Assertions** (`tests/utils/TestAssertions.hpp`): Custom assertion macros
- **Test Comparators** (`tests/utils/TestComparators.hpp`): Numerical and data comparison utilities
- **Test Data Providers** (`tests/utils/TestDataProviders.hpp`): Test data generators
- **Test Benchmarks** (`tests/utils/TestBenchmarks.hpp`): Performance benchmark utilities

#### Test Configuration
Configuration and test runner components:

- **Test Configuration** (`tests/TestConfig.hpp`): Test framework configuration
- **Test Runner** (`tests/TestRunner.hpp`): Test execution and reporting
- **Integration Runner** (`tests/IntegrationRunner.hpp`): Integration test execution

### Building and Running Tests

#### Build Instructions
To build the test infrastructure, run:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

This will build:
- `nlm_test`: Unit and integration test executable
- `nlm_integration_test`: Integration test runner executable

#### Running Tests
After building, run the tests with:

```bash
# Run unit and integration tests
./nlm_test

# Run integration tests only
./nlm_integration_test
```

#### Test Results
Tests produce detailed output including:
- Test names and categories
- Pass/fail status
- Execution time measurements
- Performance benchmarks
- Integration test results

### Test Framework Features

#### Core Testing Framework
- **Simple Test API**: Easy-to-use macros for test case creation
- **Assertion Macros**: Comprehensive assertion utilities (ASSERT_EQ, ASSERT_TRUE, ASSERT_DOUBLE_EQ, etc.)
- **Test Organization**: Tests organized by category (Unit, Integration, Performance, General)
- **Timing**: Automatic test timing and performance measurement
- **Exception Handling**: Graceful test failure handling

#### Test Utilities
- **Data Generators**: Random data generation for deterministic tests
- **Benchmarking**: Performance benchmarking utilities
- **Memory Monitoring**: Memory usage tracking
- **Numerical Comparison**: Floating-point and integer comparison utilities
- **Container Utilities**: Vector and matrix comparison utilities

#### Configuration Options
The test framework supports extensive configuration:

- **Framework Options**: Assertion behavior, output format, parallel execution
- **Test Organization**: Categories, patterns, nesting
- **Performance Monitoring**: Timing, memory, CPU profiling
- **Test Data**: Random generation, persistence
- **Reporting**: Console, file, HTML, XML, CSV, PDF reports
- **Coverage**: Code coverage measurement (when enabled)
- **Integration**: Integration test configuration
- **Execution**: Test reuse, caching, retry, parallelism

#### Reporting
The testing framework generates multiple types of reports:

- **Console Reports**: Detailed test output to terminal
- **JSON Reports**: Structured data for automation
- **XML Reports**: Standard test reporting format
- **CSV Reports**: Spreadsheet-compatible data
- **HTML Reports**: Interactive web-based reports
- **PDF Reports**: Formatted printable reports

### Test Design Principles

#### Backward Compatibility
- Existing demo tests continue to work without modification
- Gradual adoption path for new test components
- Optional integration with existing infrastructure

#### Gradual Adoption
- Test framework can be adopted incrementally
- Components can be enabled/disabled as needed
- Flexible test organization allows selective testing

#### Flexibility
- Multiple test approaches supported (unit, integration, performance)
- Configurable test execution options
- Extensible plugin architecture for new test types

#### Integration
- Works with existing demo and experiment infrastructure
- Compatible with existing codebase
- Supports integration testing of complex systems

#### Performance
- Minimal test overhead
- Configurable test execution options
- Efficient data generation and comparison

#### Maintainability
- Easy to add new tests
- Clear test organization
- Well-documented testing approach
- Consistent coding standards

#### Documentation
- Comprehensive documentation
- Examples and templates
- Configuration guides
- Performance guidelines

### Adding New Tests

To add new tests, follow these guidelines:

1. **Unit Tests**: Place test files in `tests/unit/` with naming convention `Test[Component].hpp/cpp`
2. **Integration Tests**: Place test files in `tests/integration/` with naming convention `Test[Scenario].hpp/cpp`
3. **Performance Tests**: Place test files in `tests/performance/` with naming convention `TestPerformance.hpp/cpp`
4. **Test Framework**: Add to `tests/Framework/` or `tests/Helpers/` as needed
5. **Test Utilities**: Add to `tests/utils/` as needed

Example test file structure:

```cpp
#include "tests/Framework/TestFramework.hpp"

class Test[ComponentName] : public TestCase {
public:
    Test[ComponentName]() : TestCase("[ComponentName]") {}
    
    void run() override {
        // Test implementation
        TestFramework::getInstance().assertTrue(condition, "Error message");
        // ... additional test assertions
    }
};

// Register the test case
static Test[ComponentName] ComponentNameTest;
TestFramework::getInstance().registerTest(&ComponentNameTest);
```

### Testing Best Practices

#### Test Organization
- Group related tests together
- Use descriptive test names
- Organize tests by component or functionality
- Include setup and teardown for shared resources

#### Test Implementation
- Write focused, single-purpose tests
- Use deterministic data generation
- Include edge case testing
- Verify error conditions
- Measure performance where appropriate

#### Test Maintenance
- Keep tests up-to-date with code changes
- Review test coverage regularly
- Update test data as needed
- Refactor tests for clarity

### Troubleshooting

#### Build Issues
If build fails due to missing dependencies:
1. Ensure all required CMake modules are present
2. Check for missing source files
3. Verify compiler support for C++20 features

#### Runtime Issues
If tests fail unexpectedly:
1. Check test configuration
2. Verify test data generation
3. Review assertion messages for clues
4. Check system resources (memory, CPU)

#### Performance Issues
If performance tests are slow:
1. Reduce number of iterations
2. Optimize test data generation
3. Check for memory leaks
4. Consider parallel execution

### Future Enhancements

The testing framework is designed to be extensible:

1. **Code Coverage**: Integration with coverage tools (GCov, LCOV)
2. **Static Analysis**: Integration with linting and code quality tools
3. **Continuous Integration**: Automated test execution in CI/CD pipelines
4. **Advanced Reporting**: Interactive dashboards and trend analysis
5. **Machine Learning**: AI-assisted test generation and optimization
6. **Parallel Testing**: Advanced parallel test execution
7. **Cloud Testing**: Distributed test execution in cloud environments

### License

The testing framework is part of the NLM project and is covered under the MIT License.

### Authors

NLM Test Framework Contributors

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
