# The NLM Framework - Comprehensive Improvements

## Overview

The NLM (Neural Learning Machine) framework has been significantly improved to provide better usability, extensibility, and maintainability. This document outlines the key improvements and how to use the new features.

## Key Improvements

### 1. Unified Command-Line Interface

The NLM framework now includes a unified command-line interface (`nlm_experiment`) that provides:

- **Flexible experiment selection**: Run specific experiments or all experiments
- **Configuration management**: Load configurations from files and command line
- **Result output**: Save experiment results to files
- **Verbose mode**: Detailed logging and progress information
- **Help system**: Built-in help with usage examples

#### Usage Examples:

```bash
# List all available experiments
./nlm_experiment --list

# Run a specific experiment
./nlm_experiment --experiment phase3_demo

# Run all experiments and save results
./nlm_experiment --all --output results.txt

# Run with custom configuration
./nlm_experiment --experiment phase4_demo --config custom.cfg

# Show help
./nlm_experiment --help
```

### 2. Comprehensive Experiment Framework

The experiment framework has been completely redesigned:

- **Base experiment class**: `Experiment` with setup/run/cleanup interface
- **Experiment factory**: `ExperimentFactory` for creating experiments
- **Experiment utilities**: `ExperimentUtils` with common functions
- **Result management**: Structured experiment results with timing and metrics
- **Error handling**: Robust error handling with detailed error messages

#### Key Features:

```cpp
// Create a new experiment
class MyExperiment : public Experiment {
public:
    std::string getName() const override { return "my_experiment"; }
    std::string getDescription() const override { return "My custom experiment"; }
    
    ExperimentResult run(std::shared_ptr<Brain> brain) override {
        // Run experiment and return results
    }
};
```

### 3. Comprehensive Test Framework

The test framework provides unit testing capabilities:

- **Test case utilities**: `TestCase` class with assertion methods
- **Test suites**: `TestSuite` for organizing related tests
- **Assertion methods**: `assertTrue`, `assertEqual`, `assertGreaterThan`, etc.
- **Utility functions**: Vector operations, statistical calculations, string utilities
- **Random number generation**: Various distributions and utilities
- **File I/O**: Safe file reading and writing

#### Usage Examples:

```cpp
// Basic test case
class VectorUtilsTest : public Test {
public:
    TestResult run() override {
        TestCase testCase(std::make_shared<VectorUtilsTest>(this));
        
        std::vector<float> v1 = {1.0f, 2.0f, 3.0f};
        std::vector<float> v2 = {4.0f, 5.0f, 6.0f};
        
        // Use assertion methods
        testCase.EXPECT_EQ(VectorUtils::euclideanDistance(v1, v2), 5.196152f);
        testCase.EXPECT_TRUE(VectorUtils::isEqual(v1, v1, 1e-6f));
        
        return testCase.getResult();
    }
};
```

### 4. Utility Functions

Extensive utility functions are now available:

#### Vector Utilities:
- `euclideanDistance(a, b)` - Calculate distance between vectors
- `cosineSimilarity(a, b)` - Calculate cosine similarity
- `normalize(vec)` - Normalize vector to unit length
- `clamp(vec, min, max)` - Clamp vector values
- `mean(vec)` - Calculate mean
- `standardDeviation(vec)` - Calculate standard deviation

#### Matrix Utilities:
- `multiply(a, b)` - Matrix multiplication
- `transpose(matrix)` - Matrix transpose
- `determinant(matrix)` - Matrix determinant

#### Statistical Utilities:
- `pearsonCorrelation(x, y)` - Pearson correlation coefficient
- `coefficientOfVariation(data)` - Coefficient of variation
- `median(data)` - Median value
- `percentile(data, percentile)` - Percentile value
- `removeOutliersIQR(data)` - Remove outliers

#### String Utilities:
- `format(format, ...)` - Formatted string output
- `split(str, delimiter)` - Split string by delimiter
- `trim(str)` - Trim whitespace
- `toLower(str)` - Convert to lowercase
- `toUpper(str)` - Convert to uppercase

#### Random Utilities:
- `randomFloat(min, max)` - Generate random float
- `randomInt(min, max)` - Generate random integer
- `randomBool(probability)` - Generate random boolean
- `randomPermutation(n)` - Generate random permutation

#### File Utilities:
- `readFile(filepath)` - Read file content
- `writeFile(filepath, content)` - Write content to file
- `exists(filepath)` - Check if file exists
- `getExtension(filepath)` - Get file extension

### 5. Configuration Improvements

The configuration system has been enhanced:

- **Better validation**: Configuration values are validated
- **Multiple sources**: Configuration can come from files, command line, or defaults
- **Type safety**: Strong typing for configuration values
- **Default values**: Sensible defaults for different experiment types

### 6. Code Quality Improvements

General code quality has been significantly improved:

- **Modern C++**: Full use of C++11/14/17 features
- **RAII**: Proper resource management
- **Error handling**: Comprehensive error handling throughout
- **Documentation**: Better code documentation and comments
- **Organization**: Better file and directory organization
- **Maintainability**: Easier to understand and modify

## Available Experiments

The framework includes experiments for all NLM phases:

- **Phase 2**: Basic neural computation and plasticity
- **Phase 3**: World interaction and learning
- **Phase 4**: Cognition and higher-level processing
- **Phase 5**: Performance optimizations and parallel processing
- **Phase 6**: Final integration and comprehensive testing

## Building and Running

### Build Instructions

```bash
# Build the project
cmake -B build
cmp --build build --target all

# Build with tests
./configure.py --enable-tests
make -j4
```

### Running Experiments

```bash
# Run all experiments
./nlm_experiment --all

# Run specific experiment
./nlm_experiment --experiment phase3_demo

# Run with custom configuration
./nlm_experiment --experiment phase4_demo --config my_config.cfg

# Save results to file
./nlm_experiment --all --output results.txt
```

### Running Tests

```bash
# Build and run tests
./nlm_experiment --test

# Or build with tests
./configure.py --enable-tests
make -j4
./tests/nlm_tests
```

## Example: Creating a New Experiment

To create a new experiment:

1. Create an experiment header file in `src/experiments/`:

```cpp
// src/experiments/MyExperiment.hpp
#ifndef NLM_MY_EXPERIMENT_HPP
#define NLM_MY_EXPERIMENT_HPP

#include "Experiment.hpp"

class MyExperiment : public Experiment {
public:
    std::string getName() const override;
    std::string getDescription() const override;
    ExperimentResult run(std::shared_ptr<Brain> brain) override;
};

#endif // NLM_MY_EXPERIMENT_HPP
```

2. Create an experiment implementation file:

```cpp
// src/experiments/MyExperiment.cpp
#include "MyExperiment.hpp"

std::string MyExperiment::getName() const {
    return "my_experiment";
}

std::string MyExperiment::getDescription() const {
    return "My custom experiment for testing";
}

ExperimentResult MyExperiment::run(std::shared_ptr<Brain> brain) {
    ExperimentResult result("my_experiment", "My custom experiment for testing");
    
    // Run experiment logic here
    // ...
    
    result.success = true;
    result.output = "Experiment completed successfully";
    
    return result;
}
```

3. Add to ExperimentFactory:

```cpp
// src/experiments/ExperimentFramework.cpp
// Add to registerExperiments():
experiments.push_back(std::make_unique<MyExperiment>());
```

## Example: Creating a New Unit Test

To create a new unit test:

1. Create a test header file in `src/tests/`:

```cpp
// src/tests/MyTest.hpp
#ifndef NLM_MY_TEST_HPP
#define NLM_MY_TEST_HPP

#include "TestFramework.hpp"

class MyTest : public Test {
public:
    std::string getName() const override;
    std::string getDescription() const override;
    TestResult run() override;
};

#endif // NLM_MY_TEST_HPP
```

2. Create a test implementation file:

```cpp
// src/tests/MyTest.cpp
#include "MyTest.hpp"

std::string MyTest::getName() const {
    return "my_test";
}

std::string MyTest::getDescription() const {
    return "Tests for my functionality";
}

TestResult MyTest::run() {
    TestCase testCase(std::make_shared<MyTest>(this));
    
    // Run test logic
    testCase.EXPECT_TRUE(true, "Basic test");
    
    return testCase.getResult();
}
```

3. Add to TestFactory:

```cpp
// src/tests/TestFramework.cpp
// Add to registerTestSuites():
suites.push_back(std::make_shared<TestSuite>("MyTestSuite", "Tests for my functionality"));
```

## Contributing

This project welcomes contributions. Please read the [contributing guide](CONTRIBUTING.md) for details on how to contribute.

## License

This project is licensed under the MIT License.
