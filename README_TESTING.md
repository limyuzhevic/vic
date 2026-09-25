# NLM Testing Infrastructure

This directory contains the comprehensive testing infrastructure for the NLM (Neural Learning Machine) project.

## Overview

This testing infrastructure provides:

- **Core testing framework** (src/testing/)
- **Test categories** (unit, integration, performance, stress)
- **Test organization** (tests/, data/, scripts/)
- **CI/CD integration** (ci/)

## Directory Structure

### Core Testing Framework (src/testing/)

- **TestFramework.hpp/cpp**: Core testing infrastructure with test runner and utilities
- **TestSuite.hpp/cpp**: Test suite management system
- **TestCase.hpp/cpp**: Individual test case implementation
- **Benchmark.hpp/cpp**: Performance benchmarking tools
- **Profiler.hpp/cpp**: Code profiling and performance analysis
- **CoverageAnalyzer.hpp/cpp**: Code coverage analysis
- **Assert.hpp/cpp**: Custom assertion framework

### Test Categories

1. **Unit Tests** (src/testing/unit/)
   - brain/: Core brain system unit tests
   - memory/: Memory system unit tests
   - plasticity/: Plasticity rule unit tests
   - neuromodulation/: Neuromodulation system unit tests
   - cognition/: Cognitive system unit tests
   - prediction/: Prediction system unit tests

2. **Integration Tests** (src/testing/integration/)
   - full_brain/: Complete brain integration tests
   - learning/: Learning process integration tests
   - memory/: Memory integration tests

3. **Performance Tests** (src/testing/performance/)
   - memory/: Memory performance benchmarks
   - neural/: Neural performance benchmarks
   - plasticity/: Plasticity performance benchmarks

4. **Stress Tests** (src/testing/stress/)
   - large_networks/: Large network stress tests
   - long_sims/: Long simulation stress tests

### Test Organization

- **tests/coverage/**: Code coverage reports and analysis
- **tests/reports/**: Test execution reports and summaries
- **tests/benchmarks/**: Performance benchmark results
- **tests/results/**: All test results and logs

### Test Data

- **data/test/**: Test data and configurations
- **data/benchmarks/**: Benchmark datasets
- **data/stress/**: Stress test scenarios

### Automation Scripts

- **scripts/run_tests.sh**: Main test execution script
- **scripts/run_benchmarks.sh**: Benchmark execution script
- **scripts/validate_build.sh**: Build validation script
- **scripts/test_coverage.sh**: Coverage analysis script

### CI/CD Integration

- **ci/test_suite.yml**: GitHub Actions workflow for test suite
- **ci/benchmark.yml**: GitHub Actions workflow for performance benchmarks
- **ci/coverage.yml**: GitHub Actions workflow for coverage analysis

## Getting Started

### Building and Running Tests

1. **Build the project with tests enabled:**

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_TESTS=ON ..
make -j$(nproc)
```

2. **Run all tests:**

```bash
cd build
./nlm_tests
```

3. **Run tests with specific options:**

```bash
./scripts/run_tests.sh -v --integration --performance
```

### Running Benchmarks

```bash
./scripts/run_benchmarks.sh -v -o benchmark_results.json
```

### Running Coverage Analysis

```bash
./scripts/test_coverage.sh -v -m 80 --html
```

### Using the Testing Framework

The testing framework provides several utilities:

1. **Test Suite Management:**

```cpp
#include "TestFramework.hpp"

int main() {
    nlm::TestFramework framework;
    
    auto suite = framework.createSuite("Brain Tests");
    
    suite->addTest("testBrainCreation", []() {
        // Test implementation
        return true;
    });
    
    suite->addTest("testBrainInitialization", []() {
        // Test implementation
        return true;
    });
    
    framework.runAllTests();
    return 0;
}
```

2. **Benchmarking:**

```cpp
#include "Benchmark.hpp"

int main() {
    nlm::Benchmark::Result result = nlm::Benchmark::benchmark(
        []() {
            // Code to benchmark
            for (int i = 0; i < 1000; ++i) {
                // Do work
            }
        },
        1000
    );
    
    std::cout << "Benchmark: " << result.name << std::endl;
    std::cout << "Time: " << result.timeMs << " ms" << std::endl;
    std::cout << "Average: " << result.averageTimeMs << " ms" << std::endl;
    
    return 0;
}
```

3. **Profiling:**

```cpp
#include "Profiler.hpp"

int main() {
    nlm::Profiler profiler;
    
    auto session = profiler.startSession("Neural Processing");
    
    // Code to profile
    for (int i = 0; i < 1000; ++i) {
        // Do work
    }
    
    profiler.endSession(session);
    profiler.generateReport("profiler_report.html");
    
    return 0;
}
```

4. **Assertions:**

```cpp
#include "Assert.hpp"

int main() {
    // Basic assertions
    bool success = nlm::Assert::isTrue(1 + 1 == 2, "Math check");
    
    // Value comparisons
    bool equal = nlm::Assert::equals(5, 5, "Values should be equal");
    bool notEqual = nlm::Assert::notEquals(5, 10, "Values should not be equal");
    
    // Floating point comparisons
    bool approxEqual = nlm::Assert::equals(3.14159, 3.1416, 0.00001, "Pi approximation");
    
    // Null checks
    bool isNull = nlm::Assert::isNull(nullptr, "Pointer should be null");
    
    return 0;
}
```

## Test Categories Explained

### Unit Tests
Test individual classes and methods in isolation. Ideal for:
- Testing individual algorithms
- Validating data structures
- Testing utility functions
- Unit testing core components

### Integration Tests
Test system interactions and component integration. Ideal for:
- Testing brain with memory systems
- Testing learning with plasticity
- Validating end-to-end workflows
- Testing system composition

### Performance Tests
Benchmark system performance and optimize. Ideal for:
- Measuring execution speed
- Analyzing memory usage
- Comparing different implementations
- Identifying performance bottlenecks

### Stress Tests
Test system under extreme conditions. Ideal for:
- Testing with large networks
- Extended simulation times
- Extreme plasticity conditions
- System stability under load

## CI/CD Pipeline

The testing infrastructure includes CI/CD workflows that:

1. **Test Suite Workflow**:
   - Runs unit, integration, and stress tests
   - Generates coverage reports
   - Commits test results
   - Notifies on failures

2. **Benchmark Workflow**:
   - Runs performance benchmarks
   - Compares with baseline
   - Detects performance regressions
   - Generates benchmark reports

3. **Coverage Workflow**:
   - Analyzes code coverage
   - Ensures minimum coverage requirements
   - Generates HTML reports
   - Comments on PRs with coverage status

## Contributing

1. **Add new tests:**
   - Create test files in the appropriate test category directory
   - Follow existing test patterns and conventions
   - Ensure tests are self-contained and repeatable

2. **Add new test data:**
   - Add datasets to data/test/ directory
   - Follow JSON format for structured data
   - Ensure test data is representative of real scenarios

3. **Update scripts:**
   - Update shell scripts for new test categories
   - Add new CI/CD workflows if needed
   - Update documentation as needed

## Best Practices

1. **Test Organization:**
   - Group related tests in appropriate categories
   - Keep tests focused and isolated
   - Use descriptive test names

2. **Test Quality:**
   - Ensure tests are deterministic
   - Use appropriate assertions
   - Test edge cases and error conditions
   - Validate expected behavior

3. **Performance Considerations:**
   - Benchmark test execution time
   - Use appropriate iteration counts
   - Measure memory usage
   - Profile test performance

4. **Coverage Requirements:**
   - Maintain minimum code coverage (80%)
   - Prioritize testing critical paths
   - Ensure new code is tested

## License

This testing infrastructure is part of the NLM project and is licensed under the same terms as the main codebase.