# Enhanced NLM Python API Test Suite

## Overview

This document provides guidance for working with the Enhanced NLM Python API test suite.

## Test Suite Structure

### Python Tests (`test_suite/python/`)

#### Core API Tests (`test_core_api.py`)
- **Purpose**: Tests for core Python API functionality
- **Coverage**: Basic operations, class creation, simulation control
- **Framework**: pytest
- **Focus**: Python-C++ integration, object creation and management

#### Integration Tests (`test_integration.py`)
- **Purpose**: System-level integration testing
- **Coverage**: Full brain simulations, sensory-motor integration, memory systems
- **Framework**: pytest
- **Focus**: Component interactions, complete workflows

#### Performance Tests (`test_performance.py`)
- **Purpose**: Performance benchmarking and stress testing
- **Coverage**: Large network tests, long-run simulations, memory efficiency
- **Framework**: pytest
- **Focus**: Scalability, reliability under load

#### Python-Specific Tests (`test_pybind11.py`)
- **Purpose**: Python bindings quality assurance
- **Coverage**: pybind11 integration, object lifetime, exception handling
- **Framework**: pytest
- **Focus**: Python-C++ interop quality

#### Configuration Tests (`test_configuration.py`)
- **Purpose**: Configuration loading and validation
- **Coverage**: JSON/YAML config, command-line, serialization
- **Framework**: pytest
- **Focus**: Configuration system robustness

### C++ Tests (`test_suite/cpp/`)

#### Unit Tests (`test_unit_tests.cpp`)
- **Purpose**: C++ component unit testing
- **Coverage**: Neuron, Synapse, Config, Memory systems, Neuromodulation
- **Framework**: Google Test (gtest)
- **Focus**: Low-level neural computation correctness

## Running Tests

### Quick Tests
```bash
python test_suite/quick_test.py
```

### Full Python Test Suite
```bash
python -m pytest test_suite/python/ -v
```

### All Tests (Python + C++)
```bash
python test_suite/test_runner.py
```

### C++ Tests Only
```bash
mkdir -p build
cd build
cmake ..
make
./nlm_test
```

## Test Categories Details

### 1. Core Python API Tests
- **Neuron operations**: Creation, type changes, membrane potential dynamics
- **Synapse operations**: Connection, plasticity, transmission
- **Brain management**: Initialization, step execution, region management
- **Sensory-Motor integration**: Input processing, action production

### 2. C++ Unit Tests
- **Neuron class**: LIF dynamics, firing states, plasticity mechanisms
- **Synapse class**: STDP, Hebbian learning, short-term plasticity
- **Memory systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection

### 3. Integration Tests
- **Full brain simulation**: Complete system initialization and execution
- **Memory-plasticity interaction**: Cross-system dynamics
- **Neuromodulation-plasticity**: Regulation mechanisms
- **Attention-memory interaction**: Cognitive control

### 4. Performance Tests
- **Large networks**: 1000+ neurons, 10000+ synapses
- **Long-run simulations**: Checkpoint/restart, stability over time
- **Stress testing**: Concurrent operations, extreme conditions
- **Memory efficiency**: Resource usage patterns

### 5. Python-Specific Tests
- **pybind11 integration**: Binding quality, object lifetime
- **Exception handling**: Error propagation and catching
- **Performance**: Python object creation and method calls

### 6. Configuration Tests
- **JSON/YAML loading**: Configuration file formats
- **Validation**: Parameter checking and error handling
- **Command-line**: Argument parsing and configuration
- **Serialization**: Save/load operations

## Test Quality Assurance

### Deterministic Tests
- All tests use fixed random seeds
- No external dependencies that vary between runs
- Identical results across multiple executions

### Isolated Tests
- Each test runs independently
- No shared state between tests
- Proper cleanup after each test

### Performance Requirements
- Test execution time: < 5 minutes for full suite
- Memory usage: < 1GB during test execution
- Scalability: Support for large-scale network testing

### Coverage Goals
- Python code coverage: >90%
- C++ code coverage: >85%
- Integration coverage: >88%
- Public API coverage: 100%

## Testing Strategy

### Phase 1: Unit Tests
- Test individual components in isolation
- Mock dependencies where appropriate
- Focus on correctness and edge cases

### Phase 2: Integration Tests
- Test component interactions
- Test complete workflows
- Test system-level behavior

### Phase 3: Performance Tests
- Test scalability and performance
- Test memory efficiency
- Test real-world usage patterns

### Phase 4: Regression Tests
- Maintain test suite for existing functionality
- Ensure backward compatibility
- Monitor performance over time

## Best Practices for Test Development

### Python Tests
- Use pytest fixtures for setup/teardown
- Parameterize tests for multiple scenarios
- Use context managers for resource management
- Write clear, descriptive test names

### C++ Tests
- Use Google Test macros (TEST, TEST_F, etc.)
- Follow established test patterns in existing tests
- Mock external dependencies for unit testing
- Include assertions for expected behavior

### Integration Tests
- Test both success and failure paths
- Include edge cases and boundary conditions
- Test error handling and exception propagation
- Validate system state before and after operations

### Performance Tests
- Measure and report execution time
- Monitor memory usage during tests
- Test with varying scales (small, medium, large)
- Include baseline performance measurements

## Test Maintenance

### Adding New Tests
1. Identify the test category (Core API, Integration, Performance, etc.)
2. Choose appropriate test framework (pytest for Python, gtest for C++)
3. Follow existing test patterns and conventions
4. Add tests to the appropriate test file
5. Update test runner if needed

### Updating Tests
- Maintain backward compatibility
- Update test expectations for new features
- Keep test execution time within limits
- Ensure proper cleanup of resources

### Debugging Tests
- Use verbose output (`-v` flag for pytest)
- Check test logs and output
- Run individual tests to isolate issues
- Use deterministic seeds for reproducibility

## CI/CD Integration

### GitHub Actions
```yaml
tests:
  runs-on: ubuntu-latest
  strategy:
    matrix:
      python-version: [3.8, 3.9, 3.10, 3.11, 3.12]
  steps:
    - uses: actions/checkout@v2
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v2
      with:
        python-version: ${{ matrix.python-version }}
    - name: Install dependencies
      run: |
        pip install pytest
        pip install -e .
    - name: Run Python tests
      run: python -m pytest test_suite/python/ -v
    - name: Build and run C++ tests
      run: |
        mkdir -p build
        cd build
        cmake ..
        make
        ./nlm_test
```

### Jenkins Pipeline
```groovy
pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'mkdir -p build'
                sh 'cd build && cmake ..'
                sh 'cd build && make'
            }
        }
        stage('Test') {
            steps {
                sh 'cd build && ./nlm_test'
                sh 'python -m pytest test_suite/python/ -v'
            }
        }
    }
}
```

## Troubleshooting

### Common Issues

#### Module Import Errors
- Ensure all dependencies are installed
- Check Python path configuration
- Verify pybind11 module compilation

#### Test Failures
- Check test logs for specific error messages
- Verify system resources (memory, CPU)
- Ensure deterministic random seeds are set

#### Performance Issues
- Increase test timeout values
- Reduce test scale for debugging
- Monitor system resource usage

#### C++ Build Errors
- Check CMake configuration
- Verify compiler version and flags
- Ensure all source files are included

### Getting Help

For additional assistance:
- Check test logs and error messages
- Review existing test patterns
- Consult test configuration (`test_config.json`)
- Refer to the main test documentation

## License

This test suite is provided under the same license as the Enhanced NLM project.
All tests are designed to be open-source and community-contributed.

## Version

Test Suite Version: 1.0.0
Created by: Kilo (Enhanced NLM Python API)
Date: 2026-10-04
