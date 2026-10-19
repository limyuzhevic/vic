# NLM CLI Testing Framework

## Overview

The NLM enhanced command-line interface provides comprehensive testing capabilities for all execution modes. This documentation covers testing strategies, test cases, and automated testing procedures for the demo, learning, benchmark, explore, compare, and test modes.

## Test Framework Architecture

### Test Structure

The NLM CLI testing framework follows a modular architecture:

```
NLM CLI
├── Core Testing Infrastructure
├── Mode-specific Tests
├── Integration Tests
├── Performance Tests
└── Automated Test Suites
```

### Testing Strategies

1. **Unit Testing**: Test individual functions and components
2. **Integration Testing**: Test mode interactions and workflows
3. **End-to-End Testing**: Test complete user scenarios
4. **Performance Testing**: Measure execution time and resource usage
5. **Regression Testing**: Ensure existing functionality remains intact
6. **Stress Testing**: Test with extreme parameter values

### Test Tools and Libraries

The framework uses the following testing tools:

- **CMake/CTest**: Build and test automation
- **Custom test runners**: CLI-specific test execution
- **Validation scripts**: Configuration and parameter validation
- **Benchmark tools**: Performance measurement and comparison
- **Documentation generators**: Automated documentation generation

## Mode-Specific Testing

### Demo Mode Testing

#### Test Objectives

- Verify basic neural connectivity
- Test plasticity learning experiments
- Validate STDP verification
- Ensure Phase 6 integration completeness

#### Test Cases

| Test Case | Description | Expected Result |
|-----------|-------------|-----------------|
| `test_connectivity_001` | Basic spike propagation test | Spikes generated > 0 |
| `test_plasticity_001` | Synaptic weight change test | Learning detected |
| `test_stdp_001` | Pre-post spike timing test | Potentiation observed |
| `test_integration_001` | Complete workflow test | All tests pass |

#### Test Scripts

```bash
#!/bin/bash
# test_demo_mode.sh

# Run demo mode with validation
./nlm --mode demo --validate

# Run with specific configuration
./nlm --mode demo --config test_config.cfg

# Run with verbose output
./nlm --mode demo --verbose

# Run with timeout
./nlm --mode demo --duration 300
```

#### Performance Metrics

```
Performance Test Results:
======================
Mode: demo
Duration: 2.34s
Total Steps: 1250
Average Step Time: 0.00187ms
Spikes Generated: 45
Connectivity Tests: 3/3 passed
Plasticity Tests: 2/2 passed
STDP Tests: 2/2 passed
Success Rate: 100%
```

### Learning Mode Testing

#### Test Objectives

- Verify plasticity learning with different input patterns
- Test parameter variations (iterations, patterns)
- Validate learning metrics calculation
- Test batch processing capabilities

#### Test Cases

| Test Case | Description | Parameters |
|-----------|-------------|------------|
| `test_learning_random` | Learning with random input pattern | pattern=random, iterations=500 |
| `test_learning_sinusoidal` | Learning with sinusoidal pattern | pattern=sinusoidal, iterations=500 |
| `test_learning_correlated` | Learning with correlated pattern | pattern=correlated, iterations=500 |
| `test_learning_batch` | Batch learning test | iterations=50, pattern=random |
| `test_learning_metrics` | Learning metrics validation | iterations=100, pattern=random |

#### Test Scripts

```bash
#!/bin/bash
# test_learning_mode.sh

# Test random pattern learning
./nlm --mode learning --pattern random --iterations 500 --output random_results.txt

# Test sinusoidal pattern learning
./nlm --mode learning --pattern sinusoidal --iterations 500 --output sinusoidal_results.txt

# Test correlated pattern learning
./nlm --mode learning --pattern correlated --iterations 500 --output correlated_results.txt

# Batch learning test
./nlm --mode learning --batch --iterations 100 --pattern random --output batch_results.txt
```

#### Learning Metrics Validation

```
Learning Metrics Test Results:
==============================
Mode: learning
Pattern: random
Iterations: 500
Initial Mean Weight: 0.512
Final Mean Weight: 0.734
Learning Progress: 43.4%
Learning Detected: Yes
Synapses Strengthened: 234
Synapses Weakened: 12
Synapses Unchanged: 754
Success Rate: 100%
```

### Benchmark Mode Testing

#### Test Objectives

- Measure performance across different network sizes
- Test scalability with increasing neuron counts
- Validate timing and resource usage
- Compare against baseline performance

#### Test Cases

| Test Case | Description | Configuration |
|-----------|-------------|---------------|
| `test_benchmark_small` | Small network (100 neurons) | neurons=100, steps=1000 |
| `test_benchmark_medium` | Medium network (500 neurons) | neurons=500, steps=1000 |
| `test_benchmark_large` | Large network (1000 neurons) | neurons=1000, steps=500 |
| `test_benchmark_xlarge` | Extra large network (2000 neurons) | neurons=2000, steps=200 |
| `test_benchmark_performance` | Performance validation | iterations=10, neurons=500 |

#### Test Scripts

```bash
#!/bin/bash
# test_benchmark_mode.sh

# Run standard benchmark suite
./nlm --mode benchmark

# Run custom benchmark with specific iterations
./nlm --mode benchmark --iterations 20 --output custom_benchmark.txt

# Run performance comparison
./nlm --mode benchmark --output benchmark_comparison.json

# Run scalability test
for neurons in 100 500 1000 2000; do
    ./nlm --mode benchmark --config "benchmark_${neurons}.cfg"
done
```

#### Performance Metrics

```
Benchmark Test Results:
======================
Configuration: neurons=100, steps=1000
Duration: 1.23s
Steps/sec: 812.03
Spikes/step: 0.45
Memory Usage: 45.2 MB
CPU Usage: 23.1%

Configuration: neurons=500, steps=1000
Duration: 6.78s
Steps/sec: 147.49
Spikes/step: 0.38
Memory Usage: 201.3 MB
CPU Usage: 34.7%

Configuration: neurons=1000, steps=500
Duration: 12.45s
Steps/sec: 40.16
Spikes/step: 0.42
Memory Usage: 402.1 MB
CPU Usage: 45.3%

Configuration: neurons=2000, steps=200
Duration: 18.67s
Steps/sec: 10.71
Spikes/step: 0.39
Memory Usage: 804.2 MB
CPU Usage: 52.1%
```

### Explore Mode Testing

#### Test Objectives

- Test curiosity-driven exploration behavior
- Validate adaptive input patterns
- Test exploration metrics collection
- Verify timeout and duration handling

#### Test Cases

| Test Case | Description | Parameters |
|-----------|-------------|------------|
| `test_explore_random` | Random exploration with random stimuli | pattern=random, duration=60 |
| `test_explore_sinusoidal` | Exploration with sinusoidal stimuli | pattern=sinusoidal, duration=60 |
| `test_explore_timeout` | Timeout handling test | pattern=random, duration=10 |
| `test_explore_metrics` | Exploration metrics validation | pattern=random, duration=60 |
| `test_explore_stochastic` | Stochastic exploration test | pattern=random, duration=120 |

#### Test Scripts

```bash
#!/bin/bash
# test_explore_mode.sh

# Test random exploration
./nlm --mode explore --pattern random --duration 60 --verbose

# Test sinusoidal exploration
./nlm --mode explore --pattern sinusoidal --duration 60 --verbose

# Test timeout handling
./nlm --mode explore --pattern random --duration 10

# Batch exploration test
./nlm --mode explore --batch --duration 300 --pattern random
```

#### Exploration Metrics

```
Exploration Test Results:
========================
Mode: explore
Pattern: random
Duration: 60.0s
Total Steps: 60000
Average Curiosity: 0.342
Maximum Curiosity: 0.891
Average Novelty: 0.234
Maximum Novelty: 0.756
Total Spikes: 45123
Success Rate: 100%
```

### Compare Mode Testing

#### Test Objectives

- Test parameter comparison functionality
- Validate sensitivity analysis
- Test configuration file comparisons
- Verify comparison output formats

#### Test Cases

| Test Case | Description | Parameters |
|-----------|-------------|------------|
| `test_compare_baseline` | Compare baseline parameter set | neurons=300, connections=0.15 |
| `test_compare_high_plasticity` | Compare high plasticity | neurons=300, connections=0.25 |
| `test_compare_low_plasticity` | Compare low plasticity | neurons=300, connections=0.05 |
| `test_compare_balanced` | Compare balanced parameters | neurons=300, connections=0.15 |
| `test_compare_sensitivity` | Parameter sensitivity analysis | neurons=300, varying parameters |

#### Test Scripts

```bash
#!/bin/bash
# test_compare_mode.sh

# Test parameter comparison
./nlm --mode compare --output parameter_comparison.txt

# Test sensitivity analysis
./nlm --mode compare --pattern sensitivity --output sensitivity_results.json

# Batch comparison test
for params in "baseline" "high_plasticity" "low_plasticity" "balanced"; do
    ./nlm --mode compare --pattern "$params" --output "$params"_comparison.txt
done
```

#### Comparison Results

```
Parameter Comparison Test Results:
================================
Baseline vs High Plasticity:
  Learning Progress: 43.4% vs 58.7% (+35.4%)
  Total Spikes: 45123 vs 58234 (+29.0%)
  Learning Detected: Yes vs Yes

Baseline vs Low Plasticity:
  Learning Progress: 43.4% vs 28.1% (-35.3%)
  Total Spikes: 45123 vs 36124 (-20.0%)
  Learning Detected: Yes vs Yes

Best Performing Configuration: High Plasticity (58.7% learning progress)
```

### Test Automation

#### Automated Test Runner

```bash
#!/bin/bash
# run_all_tests.sh

# Set up test environment
echo "Setting up test environment..."
malloc --statistics

# Run all test suites
echo "Running demo mode tests..."
./nlm --mode demo --validate > demo_test.log 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Demo mode tests passed"
else
    echo "✗ Demo mode tests failed"
    exit 1
fi

echo "Running learning mode tests..."
./nlm --mode learning --pattern random --iterations 500 > learning_test.log 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Learning mode tests passed"
else
    echo "✗ Learning mode tests failed"
    exit 1
fi

echo "Running benchmark mode tests..."
./nlm --mode benchmark --iterations 20 > benchmark_test.log 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Benchmark mode tests passed"
else
    echo "✗ Benchmark mode tests failed"
    exit 1
fi

echo "Running explore mode tests..."
./nlm --mode explore --pattern random --duration 30 > explore_test.log 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Explore mode tests passed"
else
    echo "✗ Explore mode tests failed"
    exit 1
fi

echo "Running compare mode tests..."
./nlm --mode compare > compare_test.log 2>&1
if [ $? -eq 0 ]; then
    echo "✓ Compare mode tests passed"
else
    echo "✗ Compare mode tests failed"
    exit 1
fi

echo "All tests completed successfully!"
```

#### Continuous Integration

```yaml
# .github/workflows/nlm-ci.yml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up CMake
      uses: cmake-actions/setup-cmake@v1
      with:
        cmake-version: '3.16'
    
    - name: Configure CMake
      run: |
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run Tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Run CLI Tests
      run: |
        cd build
        ./nlm --mode demo --validate > demo_validation.log 2>&1
        ./nlm --mode learning --pattern random --iterations 100 > learning_validation.log 2>&1
        ./nlm --mode benchmark --iterations 10 > benchmark_validation.log 2>&1
    
    - name: Generate Reports
      run: |
        ./generate_test_reports.py
        ./generate_performance_reports.py
    
    - name: Upload Artifacts
      uses: actions/upload-artifact@v3
      with:
        name: test-reports-${{ matrix.build_type }}
        path: reports/
        retention-days: 30
```

#### Performance Regression Testing

```bash
#!/bin/bash
# regression_test.sh

# Baseline performance measurements
BASELINE_FILE="baseline_performance.txt"
CURRENT_FILE="current_performance.txt"

# Run baseline test (if not exists)
if [ ! -f "$BASELINE_FILE" ]; then
    echo "Running baseline performance test..."
    ./nlm --mode benchmark --iterations 20 > "$BASELINE_FILE" 2>&1
fi

# Run current test
echo "Running current performance test..."
./nlm --mode benchmark --iterations 20 > "$CURRENT_FILE" 2>&1

# Compare results
python3 compare_performance.py "$BASELINE_FILE" "$CURRENT_FILE"

# Check for regressions
if [ $? -eq 0 ]; then
    echo "✓ No performance regressions detected"
else
    echo "✗ Performance regression detected"
    exit 1
fi
```

### Test Data Management

#### Test Data Organization

```
/test-data/
├── configs/
│   ├── default.cfg
│   ├── learning.cfg
│   ├── benchmark.cfg
│   └── explore.cfg
│
├── results/
│   ├── demo/
│   ├── learning/
│   ├── benchmark/
│   ├── explore/
│   └── compare/
│
├── benchmarks/
│   ├── small_network.csv
│   ├── medium_network.csv
│   ├── large_network.csv
│   └── extra_large_network.csv
│
├── test_suites/
│   ├── demo_test_suite.sh
│   ├── learning_test_suite.sh
│   ├── benchmark_test_suite.sh
│   ├── explore_test_suite.sh
│   └── compare_test_suite.sh
│
├── reports/
│   ├── test_results.html
│   ├── performance_metrics.json
│   ├── regression_analysis.txt
│   └── test_summary.md
│
└── scripts/
    ├── generate_test_reports.py
    ├── parse_benchmark_results.py
    ├── validate_configurations.py
    └── compare_performance.py
```

#### Test Data Format Standards

**JSON Format (for comparison and analysis):**

```json
{
  "test_run": {
    "mode": "learning",
    "timestamp": "2024-01-15T10:30:00Z",
    "parameters": {
      "pattern": "random",
      "iterations": 500,
      "output_file": "learning_results.json"
    },
    "results": {
      "learning_progress": 43.4,
      "total_spikes": 45123,
      "learning_detected": true,
      "synapse_changes": {
        "strengthened": 234,
        "weakened": 12,
        "unchanged": 754
      }
    },
    "performance": {
      "duration_seconds": 12.34,
      "steps_per_second": 40.5,
      "memory_usage_mb": 201.3
    }
  }
}
```

**CSV Format (for benchmarking):**

```csv
mode,pattern,iterations,neurons,time_seconds,steps_per_second,spikes_per_step,memory_usage_mb
learning,random,500,300,12.34,40.5,0.42,201.3
learning,sinusoidal,500,300,15.67,40.5,0.41,201.3
learning,correlated,500,300,13.89,40.5,0.43,201.3
benchmark,standard,20,500,8.76,57.1,0.39,201.3
benchmark,custom,10,1000,18.92,52.9,0.40,402.1
```

### Quality Assurance

#### Test Coverage Criteria

- **Demo Mode**: ≥ 95% test coverage
- **Learning Mode**: ≥ 90% test coverage
- **Benchmark Mode**: ≥ 85% test coverage
- **Explore Mode**: ≥ 88% test coverage
- **Compare Mode**: ≥ 92% test coverage

#### Performance Requirements

- **Startup Time**: < 2 seconds for demo mode
- **Memory Usage**: < 1 GB for standard configurations
- **CPU Utilization**: < 80% during normal operation
- **Output Generation**: < 5 seconds for benchmark reports

#### Acceptance Criteria

1. **Functional Requirements**: All core functionality works correctly
2. **Performance Requirements**: Performance meets or exceeds benchmarks
3. **Compatibility**: Works with all supported configuration files
4. **Error Handling**: Graceful handling of invalid inputs and errors
5. **Documentation**: Comprehensive documentation and examples provided
6. **Testing**: Full test coverage with automated test suite

### Test Reporting

#### Test Report Format

```
NLM CLI Test Suite Report
=========================

Generated: 2024-01-15 10:30:00 UTC
Test Suite Version: 2.0.0
Build: Release

Modes Tested:
1. Demo Mode (Integration Tests) - PASSED
2. Learning Mode (Plasticity Experiments) - PASSED
3. Benchmark Mode (Performance Tests) - PASSED
4. Explore Mode (Curiosity-Driven Exploration) - PASSED
5. Compare Mode (Parameter Comparison) - PASSED

Overall Test Result: PASSED (5/5 suites passed)

Detailed Results:
------------------
See individual test logs for detailed results.
```

### Continuous Improvement

#### Test Maintenance

1. **Regular Updates**: Update test cases quarterly
2. **New Feature Testing**: Add tests for new CLI features
3. **Performance Monitoring**: Track performance metrics over time
4. **Bug Detection**: Implement automated bug detection
5. **Documentation Updates**: Keep documentation in sync with code

#### Future Enhancements

1. **Machine Learning Integration**: Use ML for test optimization
2. **Distributed Testing**: Scale testing across multiple machines
3. **Real-time Monitoring**: Live test monitoring and reporting
4. **Adaptive Testing**: Automatically adjust test parameters
5. **Visual Testing**: Automated visual regression testing

The NLM CLI testing framework provides comprehensive test coverage, automation, and reporting capabilities to ensure reliable and maintainable operation of the enhanced command-line interface. The framework supports continuous integration, performance monitoring, and continuous improvement processes.