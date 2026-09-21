# NLM - Neural Learning Machine Testing Framework

This framework provides comprehensive tests for the NLM system, including unit tests for core components,
integration tests for the complete system, and performance benchmarks.

## Test Structure

```
NLM Tests
├── Unit Tests
│   ├── test_brain.cpp           - Brain core functionality
│   ├── test_neuron.cpp          - Neuron dynamics and LIF integration
│   ├── test_synapse.cpp         - Synaptic plasticity and transmission
│   ├── test_stdp.cpp           - Spike-Timing Dependent Plasticity
│   ├── test_config.cpp          - Configuration management
│   └── test_clock.cpp           - Simulation timing
├── Integration Tests
│   ├── test_main.cpp            - Complete system simulation
│   └── test_experiment.cpp     - Experiment framework
├── Performance Tests
│   ├── test_benchmark.cpp       - Performance benchmarks
│   └── test_scaling.cpp         - System scaling
└── Python Tests
    ├── test_python_bindings.py  - Python interface testing
    └── test_python_integration.py - Python-C++ integration
```

## Building Tests

### From Source

```bash
# Configure with tests enabled
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Run specific test suites
./nlm_test_brain
./nlm_test_neuron
./nlm_test_synapse
./nlm_test_main
```

### Using Python

```bash
# Install test dependencies
pip install pytest numpy

# Install pynlm in development mode
pip install -e .

# Run Python tests
pytest tests/

# Run all test suites
pytest tests/ --verbose --tb=short
```

## Running Tests

### Unit Tests (C++)

```bash
# Run all C++ tests
make test

# Or run individual test executables
./nlm_test_brain
./nlm_test_neuron
./nlm_test_synapse
./nlm_test_stdp
./nlm_test_config
./nlm_test_clock
./nlm_test_main
```

### Integration Tests

```bash
# Phase 2 integration test (runs the main simulation)
./nlm

# Phase 3 experiment test
./nlm_phase3_demo

# Phase 4 experiment test
./nlm_phase4_demo
```

### Python Tests

```bash
# Basic Python functionality test
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); print('Import successful')"

# Run full Python test suite
pytest tests/test_python_bindings.py -v
pytest tests/test_python_integration.py -v
```

## Test Categories

### 1. Core Functionality Tests

Tests the fundamental components of NLM:

- **Brain Initialization**: Testing brain setup, region creation, neuron population management
- **Neuron Dynamics**: Testing LIF neuron integration, spike generation, refractory periods
- **Synaptic Transmission**: Testing synaptic delays, weight management, connection establishment
- **Plasticity Rules**: Testing STDP, Hebbian learning, reward-modulated learning

### 2. Integration Tests

Tests the complete system working together:

- **Sensorimotor Loop**: Testing perception-action cycle in complete agent
- **Memory Integration**: Testing working memory, episodic memory, associative memory
- **Neuromodulation**: Testing dopamine, curiosity, novelty detection
- **Development**: Testing developmental stages and structural changes

### 3. Performance Tests

Tests system performance and scalability:

- **Benchmark Testing**: Testing computational performance at various scales
- **Memory Usage**: Testing memory efficiency and cleanup
- **Scaling Analysis**: Testing how system performance scales with neuron count

### 4. Python Interface Tests

Tests the Python bindings and API:

- **Binding Tests**: Testing that all C++ classes are accessible from Python
- **Data Type Tests**: Testing that Python data structures are properly converted
- **Error Handling Tests**: Testing that errors are properly propagated

## Performance Monitoring

The test framework includes built-in performance monitoring:

```bash
# Run performance benchmarks
./nlm_test_benchmark

# Generate performance reports
./nlm_test_benchmark --report

# Memory leak detection
./nlm_test_memory
```

## Continuous Integration

The test framework is designed to integrate with CI/CD pipelines:

```yaml
# Example GitHub Actions workflow
- name: Build and Test
  run: |
    mkdir build && cd build
    cmake .. -DBUILD_TESTS=ON
    make -j$(nproc)
    ctest --output-on-failure
    pytest tests/
```

## Custom Test Framework

The test framework provides utilities for:

1. **Test Case Management**: Organizing related tests into logical groups
2. **Performance Benchmarking**: Measuring execution time and memory usage
3. **Result Reporting**: Generating human-readable test reports
4. **Logging**: Detailed logging for debugging test failures

```cpp
// Example custom test
TEST(BrainTest, InitializationWithConfig) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    Brain brain(config);
    ASSERT_TRUE(brain.initialize());
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 1000);
    EXPECT_EQ(brain.getRegionCount(), 2);
}
```

## Test Coverage

The current test suite covers:

- **Code Coverage**: ~85% of C++ code is tested
- **Branch Coverage**: Most decision points and error conditions
- **Integration Coverage**: Complete system integration tested
- **Performance Coverage**: Benchmarks for all major components

## Using Tests for Development

### During Development

```bash
# Run tests frequently during development
make test

# Run specific test categories
make test-brain
make test-neuron
make test-synapse
```

### Before Committing

```bash
# Run full test suite before committing
make test
pytest tests/

# Check for regressions
make test-regressions
```

### Troubleshooting

If tests fail:

1. **Memory Issues**: Check for memory leaks with `valgrind`
2. **Performance Issues**: Run benchmarks to identify bottlenecks
3. **Integration Issues**: Run integration tests to check system cohesion
4. **Python Issues**: Test Python bindings separately

```bash
# Debug memory issues
valgrind --leak-check=full ./nlm_test_*

# Profile performance
perf record ./nlm_test_*
perf report
```

## Test Extensions

### Adding New Tests

To add new tests:

1. **C++ Tests**: Add new test files to `tests/` directory
2. **Python Tests**: Add new test files to `tests/` directory
3. **Integration Tests**: Add new test scenarios to existing test executables

### Test Configuration

Modify `CMakeLists.txt` to:

- Add new test targets
- Configure test options
- Set test dependencies

```cmake
# Example test addition
add_test(NAME nlm_test_neuron
    COMMAND nlm_test_neuron
)

set_tests_properties(nlm_test_neuron PROPERTIES TIMEOUT 60)
```

This testing framework provides comprehensive coverage for the NLM system, from low-level neuron dynamics to high-level system integration, ensuring reliable and performant operation of the neural simulation framework.