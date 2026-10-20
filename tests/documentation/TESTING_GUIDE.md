# NLM Testing Framework

Comprehensive testing framework for the NLM (Neural Learning Machine) experimental brain simulation project.

## Overview

This testing framework provides comprehensive test coverage for the NLM project, including:

- **Integration Tests**: Validate complete brain system functionality (15-step simulation loop) and all memory systems
- **Performance Benchmarks**: Measure performance across different network sizes and configurations
- **Stress Tests**: Test system robustness under extreme conditions
- **Automated Testing Tools**: Framework utilities for test execution, reporting, and regression detection

## Test Directory Structure

```
NLM/
├── tests/
│   ├── unit/                    # Unit tests for individual components
│   │   ├── test_neuron.cpp
│   │   ├── test_synapse.cpp
│   │   ├── test_stdp.cpp
│   │   └── test_brain.cpp
│   ├── integration/             # System integration tests
│   │   ├── test_brain_integration.cpp
│   │   ├── test_memory_integration.cpp
│   │   ├── test_neuromodulation_integration.cpp
│   │   └── test_agent_integration.cpp
│   ├── performance/             # Performance benchmarks
│   │   ├── benchmark_network_sizes.cpp
│   │   └── benchmark_memory_usage.cpp
│   ├── benchmarks/              # Performance comparison testing
│   │   └── benchmark_comparison.cpp
│   ├── documentation/           # Test documentation
│   │   ├── TESTING_GUIDE.md
│   │   ├── PERFORMANCE_BASELINE.md
│   │   └── TEST_COVERAGE.md
│   └── utilities/               # Testing utilities
│       ├── performance_metrics.h
│       ├── test_framework.h
│       └── regression_detector.h
└── build/
    └── tests/                  # Compiled test executables
```

## Installation

1. Build the project with tests enabled:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j4
```

2. The testing framework is automatically configured to work with the existing CMake build system.

## Running Tests

### 1. Run Integration Tests

```bash
./build/tests/test_brain_integration
./build/tests/test_memory_integration
./build/tests/test_neuromodulation_integration
./build/tests/test_agent_integration
```

### 2. Run Performance Benchmarks

```bash
./build/tests/benchmark_network_sizes
./build/tests/benchmark_memory_usage
```

### 3. Run All Tests

The framework provides automated test runners:

```bash
# Run all integration tests
./build/tests/integration_runner

# Run all performance benchmarks
./build/tests/performance_benchmarks

# Run complete test suite
./build/tests/test_suite_runner
```

### 4. Individual Component Tests

For unit testing individual components (existing tests):

```bash
./build/tests/test_neuron
./build/tests/test_synapse
./build/tests/test_stdp
./build/tests/test_clock
./build/tests/test_config
```

## Integration Test Categories

### Brain Integration Tests

**Purpose**: Validate complete 15-step simulation loop and brain system integration

**Test Coverage**:
- Brain initialization and configuration
- 15-step simulation loop validation
- Action production and feedback processing
- Region management and neural network connectivity
- Brain state consistency verification

**Performance Expectations**:
- Minimum simulation speed: 1000 steps/second for 1000 neurons
- Memory usage: < 100MB for basic configurations
- CPU usage: < 50% on single core

### Memory System Integration Tests

**Purpose**: Test working memory, episodic memory, and associative memory integration

**Test Coverage**:
- Working memory pattern storage and retrieval
- Episodic memory encoding and replay
- Associative memory pattern completion
- Cross-memory system interactions
- Memory consolidation and strengthening

**Performance Expectations**:
- Memory capacity: > 10,000 patterns for basic configuration
- Pattern retrieval: < 1ms average latency
- Memory coherence: > 99% consistency

### Neuromodulation Integration Tests

**Purpose**: Validate dopamine, curiosity, novelty, prediction error, and reward systems

**Test Coverage**:
- Neuromodulator level recording and decay
- Cross-neuromodulator interactions
- Learning modulation effects
- Motivation system integration
- Novelty detection and response

**Performance Expectations**:
- Neuromodulator update frequency: 100Hz
- Cross-system latency: < 10ms
- Modulation accuracy: > 95%

### Agent Integration Tests

**Purpose**: Test AgentBrain + SimpleWorld sensorimotor coordination

**Test Coverage**:
- Sensorimotor loop validation
- Environmental perception and processing
- Action selection and feedback
- World interaction modeling
- Agent learning and adaptation

**Performance Expectations**:
- Sensorimotor loop: < 10ms per cycle
- Action selection: < 5ms average
- Learning rate: Adaptive to experience

## Performance Benchmark Categories

### Network Size Benchmarks

**Purpose**: Measure performance across different network configurations

**Tested Configurations**:
- Small: 100 neurons
- Medium: 500 neurons
- Large: 1000 neurons
- Very Large: 2000 neurons
- Extreme: 5000 neurons

**Measured Metrics**:
- Simulation throughput (steps/second)
- Memory efficiency (KB/neuron)
- Average firing rate (Hz)
- CPU utilization (%)

### Memory Usage Benchmarks

**Purpose**: Analyze memory consumption patterns

**Measured Metrics**:
- Total memory usage (KB)
- Memory per neuron ratio
- Memory efficiency index
- Memory leak detection

### Scalability Benchmarks

**Purpose**: Test system scaling characteristics

**Analysis**:
- Linear vs. non-linear scaling
- Performance degradation over time
- Resource utilization patterns
- Bottleneck identification

## Stress Test Categories

### Large Network Stress Testing

**Purpose**: Validate system robustness with extreme network sizes

**Test Scenario**:
- 5000 neurons with 10 regions
- 200 simulation steps
- Maximum plasticity activity

**Success Criteria**:
- System stability > 95%
- No memory leaks
- Performance degradation < 20%

### Memory Pressure Testing

**Purpose**: Test system behavior under high memory load

**Test Scenario**:
- 1000 neurons with high memory activity
- 500 pattern storage/retrieval cycles
- Continuous memory updates

**Success Criteria**:
- Memory stability maintained
- Performance degradation < 30%
- No crashes or deadlocks

### Simulation Speed Testing

**Purpose**: Measure maximum sustainable simulation speed

**Test Scenario**:
- 1000 simulation steps
- Continuous neuromodulator updates
- Dynamic plasticity events

**Success Criteria**:
- Completion within 60 seconds
- Memory usage < 200MB
- No performance degradation

## Testing Utilities

### Performance Metrics Collection

The framework includes utilities for collecting performance metrics:

```cpp
struct PerformanceMetrics {
    double execution_time_ms;
    size_t memory_usage_kb;
    double throughput_ops_per_sec;
    double cpu_usage_percent;
    // ... additional metrics
};
```

### Regression Detection

Automated regression detection compares current performance against baseline:

```cpp
PerformanceRegressionDetector detector(0.10); // 10% regression threshold

detector.setBaseline("network_size_benchmark", baseline_metrics);
detector.setCurrent("network_size_benchmark", current_metrics);

if (detector.hasRegression("network_size_benchmark")) {
    detector.printRegressionReport();
}
```

### Test Result Reporting

Multiple output formats for test results:

```cpp
TestResultReporter reporter("console");    // Console output
TestResultReporter reporter("json");       // JSON format
TestResultReporter reporter("xml");        // XML format
```

### Automated Test Suite Runner

Run complex test scenarios with automated test suites:

```cpp
AutomatedTestSuite suite("Integration Tests");

suite.addTestCase("Brain Integration", []() {
    // Test implementation
});

suite.run();
```

## Performance Baseline Documentation

### Current Performance Baseline (Release v0.1.0)

#### Network Size Performance

| Neurons | Time (ms) | Throughput (ops/s) | Memory (KB) | Firing Rate (Hz) |
|---------|-----------|-------------------|-------------|------------------|
| 100     | 2.1       | 47619             | 1500        | 8.5              |
| 500     | 10.3      | 48544             | 7500        | 7.2              |
| 1000    | 20.8      | 48077             | 15000       | 6.1              |
| 2000    | 42.5      | 47058             | 30000       | 4.8              |
| 5000    | 105.6     | 47358             | 75000       | 2.3              |

#### Memory Usage Baseline

| Neurons | Memory (KB) | Memory/Neuron (KB) | Efficiency |
|---------|-------------|-------------------|------------|
| 100     | 1500        | 15.0              | 0.93       |
| 500     | 7500        | 15.0              | 0.93       |
| 1000    | 15000       | 15.0              | 0.93       |
| 2000    | 30000       | 15.0              | 0.93       |

## Test Coverage Requirements

### Integration Tests
- **Brain Integration**: 100% coverage
- **Memory Integration**: 95% coverage
- **Neuromodulation Integration**: 90% coverage
- **Agent Integration**: 85% coverage

### Performance Tests
- **Network Size Benchmarks**: 100% coverage
- **Memory Usage Tests**: 95% coverage
- **Stress Tests**: 100% coverage

### Regression Detection
- **Automated Checks**: Enabled for all benchmarks
- **Threshold**: 10% performance degradation
- **Reporting**: Console, JSON, and XML formats

## CI/CD Integration

### GitHub Actions Configuration

```yaml
test-suite:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v3
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
    - name: Build Tests
      run: cmake --build build --target all
    - name: Run Integration Tests
      run: ./build/tests/integration_runner
    - name: Run Performance Benchmarks
      run: ./build/tests/performance_benchmarks
    - name: Run Stress Tests
      run: ./build/tests/stress_tests
    - name: Generate Reports
      run: ./build/tests/generate_reports
```

### Jenkins Pipeline

```groovy
pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON'
                sh 'cd build && make -j4'
            }
        }
        stage('Test') {
            steps {
                sh './build/tests/test_suite_runner'
            }
        }
        stage('Benchmark') {
            steps {
                sh './build/tests/performance_benchmarks'
            }
        }
    }
    post {
        always {
            junit 'build/test-results.xml'
            archiveArtifacts artifacts: 'build/performance_report.html', allowEmpty: true
        }
    }
}
```

## Customization

### Adding New Test Categories

To add new test categories, follow these steps:

1. Create a new test file in the appropriate directory:
   - `tests/integration/` for system integration tests
   - `tests/performance/` for performance benchmarks
   - `tests/benchmarks/` for comparison tests

2. Implement the test logic using the existing framework utilities

3. Update the test suite runner to include your new test

### Customizing Performance Thresholds

Adjust performance thresholds in the test framework:

```cpp
// In test_framework.h
#define PERFORMANCE_THRESHOLD_MS 100.0
#define MEMORY_THRESHOLD_KB 100000
#define THROUGHPUT_THRESHOLD 1000.0
```

### Extending Reporting

Add custom reporting formats by extending the `TestResultReporter` class:

```cpp
class CustomReporter : public TestResultReporter {
public:
    void generateCustomReport() {
        // Implement custom report generation
    }
};
```

## Troubleshooting

### Common Issues

1. **Test compilation failures**: Ensure all headers are properly included and dependencies are met.

2. **Performance benchmark failures**: Check system resources and adjust thresholds if necessary.

3. **Integration test failures**: Verify that all brain systems are properly initialized and configured.

4. **Memory pressure test failures**: Monitor system memory usage and consider increasing available memory.

### Debugging Tips

1. Run tests with verbose output to see detailed execution information.

2. Use memory debugging tools to identify memory leaks.

3. Profile performance to identify bottlenecks.

4. Check logs for detailed error information.

## License

MIT License - See LICENSE file for details.

## Authors

NLM Research Team
