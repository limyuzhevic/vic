CMakeLists.txt for NLM Tests
============================

This document describes the test infrastructure for NLM (熙然) and how to build and run tests.

## Overview

NLM includes a comprehensive test framework that provides:
- Structured neural network testing
- Configurable test scenarios
- Detailed result reporting
- Extensible test architecture

## Test Framework Architecture

### Core Components

1. **TestFramework Base Class** (`src/tests/TestFramework.hpp`)
   - Provides common neural network testing functionality
   - Handles brain initialization and cleanup
   - Manages test configuration and validation
   - Executes standardized test sequences

2. **Test Implementations** (`src/tests/`)
   - Integration tests for all neural subsystems
   - Performance and stress tests
   - Edge case and error condition tests

3. **Test Configuration**
   - Parameterized test scenarios
   - Command-line configuration options
   - Validation and error handling

## Building Tests

### Standard Build

```bash
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j4
```

### Running Tests

```bash
# Run all tests
./nlm_tests

# Run specific test categories
./nlm_tests --test-category basic
./nlm_tests --test-category plasticity
./nlm_tests --test-category stdp

# Run with custom configuration
./nlm_tests --neurons 2000 --steps 5000 --seed 12345
```

## Test Categories

### 1. Basic Connectivity Tests
- **Purpose**: Verify basic neural network functionality
- **Tests**: Single neuron activation, signal propagation
- **Metrics**: Spike generation, firing rates
- **Configuration**: Neuron count, connection probability

### 2. Plasticity Learning Tests
- **Purpose**: Validate synaptic plasticity mechanisms
- **Tests**: STDP, Hebbian learning, structural plasticity
- **Metrics**: Weight changes, learning curves
- **Configuration**: Simulation steps, input patterns

### 3. STDP Verification Tests
- **Purpose**: Confirm spike-timing-dependent plasticity
- **Tests**: Pre-before-post spike ordering
- **Metrics**: Synaptic weight changes, LTP/LTD
- **Configuration**: Trial count, timing parameters

### 4. Integration Tests
- **Purpose**: Test complete brain system integration
- **Tests**: Memory systems, neuromodulation, cognition
- **Metrics**: System coherence, emergent behaviors
- **Configuration**: Multi-system parameters

## Configuration Options

### Command Line Arguments

```bash
--neurons <count>           Number of neurons (default: 500)
--seed <value>             Random seed (default: 42)
--steps <count>            Plasticity experiment steps (default: 1000)
--connection-prob <float>   Connection probability (default: 0.15)
--timestep <float>         Simulation timestep (default: 0.001)
--test-category <cat>      Specific test category (basic, plasticity, stdp, integration)
--help                     Show help information
```

### Configuration File Format

```ini
[test_framework]
neurons = 500
seed = 42
plasticity_steps = 1000
connection_probability = 0.15
simulation_timestep = 0.001
stdp_trials = 50

[plasticity]
input_injection_level = 30.0
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

[connectivity]
current_injection_level = 50.0
```

## Test Results and Reporting

### Output Format

The test framework produces detailed reports including:

1. **Summary Statistics**
   - Test name and status (PASS/FAIL)
   - Execution time
   - Weight changes
   - Spike activity statistics

2. **Detailed Metrics**
   - Synaptic changes (strengthened/weakened/unchanged)
   - Memory system performance
   - Neuromodulation levels

3. **Validation Results**
   - Configuration validation
   - System initialization status
   - Performance benchmarks

### Exit Codes

- `0`: All tests passed
- `1`: One or more tests failed
- `2`: Configuration error
- `3`: System initialization failure

## Extending the Test Framework

### Adding New Test Categories

```cpp
class CustomTestFramework : public TestFramework {
public:
    bool runCustomTest() override {
        // Implement custom test logic
        // Return true if test passes
    }
};
```

### Adding Test Parameters

```cpp
customConfig->set("custom_parameter", value, ConfigSource::User);
```

### Custom Test Scenarios

```cpp
// Example: Custom plasticity test
void runCustomPlasticityTest() {
    createBrain(true);
    // Custom test logic here
    // Enable specific plasticity rules
    // Apply unique input patterns
    // Validate specific outcomes
}
```

## Performance Considerations

### Memory Usage
- Brain configurations can be saved/loaded for repeated testing
- Checkpoint system supports state persistence
- Efficient memory management for large networks

### Execution Time
- Configurable test duration for performance benchmarking
- Progress reporting for long-running tests
- Parallel test execution support

## Troubleshooting

### Common Issues

**1. "Not enough synapses for test"**
- Solution: Increase neuron count or connection probability
- Example: `--neurons 2000 --connection-prob 0.2`

**2. "No learning detected"**
- Solution: Increase plasticity experiment steps or current injection levels
- Example: `--steps 5000 --neurons 1000`

**3. Configuration errors**
- Solution: Validate configuration file format
- Use `--help` for command line options

**4. Memory allocation failures**
- Solution: Reduce network size or increase system resources
- Use checkpoint system to test state persistence

### Debugging

```bash
# Run with verbose logging
./nlm_tests --verbose

# Capture test output for analysis
./nlm_tests > test_output.txt 2>&1

# Run with specific test category
./nlm_tests --test-category integration --verbose
```

## Integration with CI/CD

### GitHub Actions Example

```yaml
name: Neural Network Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Configure CMake
      run: cmake -B build -DBUILD_TESTS=ON
    - name: Build tests
      run: cmake --build build --target all
    - name: Run tests
      run: ./build/nlm_tests
    - name: Upload test results
      uses: actions/upload-artifact@v3
      with:
        name: test-results
        path: test_output.txt
```

## Future Enhancements

### Planned Features

1. **Parallel Test Execution**
   - Run multiple test scenarios concurrently
   - Utilize multi-core processors effectively

2. **Adaptive Test Generation**
   - Dynamic test parameters based on system performance
   - Self-modifying test scenarios

3. **Extended Validation**
   - Machine learning-based anomaly detection
   - Predictive test failure analysis

4. **Performance Benchmarking**
   - Automated performance regression testing
   - Resource usage profiling

5. **Visualization Support**
   - Real-time test result visualization
   - Interactive test parameter adjustment

## License

This test framework is part of the NLM project and is licensed under the MIT License.

See LICENSE file for complete license terms.
