# Test Setup and Teardown Documentation

## Overview

This document provides comprehensive information about test setup, configuration, execution, and teardown procedures for the NLM C++ and Python bindings. It covers everything from basic test execution to advanced testing scenarios and best practices.

## Table of Contents

1. [Test Environment Setup](#test-environment-setup)
2. [Test Configuration](#test-configuration)
3. [C++ Test Setup](#cpp-test-setup)
4. [Python Test Setup](#python-test-setup)
5. [Test Execution Procedures](#test-execution-procedures)
6. [Test Teardown and Cleanup](#test-teardown-and-cleanup)
7. [Advanced Testing Features](#advanced-testing-features)
8. [Troubleshooting and Common Issues](#troubleshooting-and-common-issues)
9. [Performance Considerations](#performance-considerations)
10. [Continuous Integration Setup](#continuous-integration-setup)

## Test Environment Setup

### Prerequisites

#### C++ Development Environment
```bash
# Required packages
- GCC 10+ or Clang 12+ or MSVC 2019+
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

# Optional but recommended
- Python development headers
- NumPy (for Python tests)
- PyTest framework
- gtest/gmock (if using alternative testing framework)
```

#### Python Development Environment
```bash
# Install Python test dependencies
pip install pytest numpy scipy
pip install pybind11 scikit-build-core

# For development with editable installs
pip install -e .
```

### Directory Structure

```
/workspace/app/
├── src/                    # C++ source code
├── tests/                 # Test files
│   ├── test_*.cpp         # C++ unit tests
│   ├── test_*.py          # Python tests
│   └── CMakeLists.txt     # Test build configuration
├── examples/              # Example scripts
│   ├── simple_brain.py
│   ├── agent_world.py
│   ├── advanced_config.py
│   ├── plasticity_experiment.py
│   ├── neuromodulation_study.py
│   ├── memory_system_test.py
│   ├── performance_benchmark.py
│   └── debugging_tools.py
├── python/               # Python bindings
│   └── bindings.cpp       # C++/Python interface
├── CMakeLists.txt        # Main build configuration
├── pyproject.toml       # Python build configuration
└── docs/                # Documentation
    ├── ARCHITECTURE.md
    ├── SCIENCE.md
    ├── ROADMAP.md
    ├── EXPERIMENTS.md
    ├── LIMITATIONS.md
    └── PHASE6_FINAL_AUDIT.md
```

### Build Directory Configuration

#### Standard Build with Tests
```bash
# Create build directory
mkdir -p build
cd build

# Configure with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build with tests
make -j$(nproc)

# Verify test targets
ls -la | grep nlm_test
```

#### Development Build
```bash
# For development with hot-reloading
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release

# Build only test targets
make test_types
make test_brain
make test_stdp
```

### Environment Variables

| Variable | Description | Default | Example |
|----------|-------------|---------|-------------|
| `NLM_TEST_PATH` | Path to test directory | `./tests` | `/workspace/app/tests` |
| `NLM_PYTHON_PATH` | Path to Python bindings | `./python` | `/workspace/app/python` |
| `NLM_BUILD_TYPE` | Build configuration | `Release` | `Debug`, `Release`, `RelWithDebInfo` |
| `NLM_ENABLE_COVERAGE` | Enable code coverage | `OFF` | `ON` |
| `NLM_TEST_TIMEOUT` | Test timeout in seconds | `300` | `600` |

## Test Configuration

### C++ Test Configuration

#### Test Categories

The C++ test suite is organized into logical categories:

1. **Core Functionality Tests** (`test_types.cpp`, `test_config.cpp`, `test_random.cpp`, `test_clock.cpp`)
   - Type system validation
   - Configuration management
   - Random number generation
   - Simulation timing

2. **Neural Component Tests** (`test_neuron.cpp`, `test_synapse.cpp`, `test_stdp.cpp`)
   - LIF neuron dynamics
   - Synaptic transmission
   - Spike-timing dependent plasticity

3. **System Integration Tests** (`test_brain.cpp`, `test_comprehensive.cpp`)
   - Complete brain simulation
   - Memory management
   - Thread safety
   - Plasticity mechanisms

#### Test Configuration Options

```cmake
# In tests/CMakeLists.txt
option(BUILD_TESTS "Build test targets" ON)

# Test compilation flags
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../python
)

# Test framework configuration
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
```

### Python Test Configuration

#### pytest Configuration

```toml
# pyproject.toml
[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
addopts = [
    "--verbose",
    "--tb=short",
    "--cov=pynlm",
    "--cov-report=html",
    "--cov-report=term-missing"
]

[tool.coverage.run]
source = ["python/"]

[tool.coverage.report]
exclude_lines = [
    "pragma: no cover",
    "def __repr__",
    "if __name__ == __main__:",
    "raise AssertionError",
    "raise NotImplementedError"
]
```

#### Test Fixtures

```python
# conftest.py
import pytest
import pynlm

@pytest.fixture
def default_config():
    """Fixture for default configuration"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 100)
    return config

@pytest.fixture
def default_brain(default_config):
    """Fixture for default brain"""
    brain = pynlm.createBrain(default_config)
    brain.initialize()
    return brain

@pytest.fixture
def test_setup():
    """Fixture for complete test setup"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    return {
        'config': config,
        'brain': brain,
        'agent': agent,
        'world': world
    }
```

## C++ Test Setup

### Test Compilation

#### Prerequisites
```bash
# Ensure all source files are present
ls src/{brain,synaptic,plasticity,neuromodulation,memory,cognition,development,sensory,motor,prediction,environment,experiments,dynamics,agent,visualization,world}/**/*.hpp
```

#### Build Commands
```bash
# Full build with tests
cd build
make nlm_test

# Build specific test targets
make test_types
make test_brain
make test_stdp

# Build with custom options
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DNLM_ENABLE_TESTS=ON
make -j4 nlm_test
```

#### Test Dependencies
```cmake
# Link libraries for test targets
target_link_libraries(nlm_test
    PRIVATE
    nlm_core
    nlm_agent
    nlm_world
)

target_compile_features(nlm_test PRIVATE cxx_std_20)
```

### Test Execution

#### Running C++ Tests
```bash
# Run all tests
ctest --output-on-failure

# Run specific test categories
./nlm_test  # Runs all C++ tests
./nlm_test --filter="testBrain"  # Filter by test name
```

#### Test Output Analysis
```bash
# Verbose test output
ctest -V

# XML output for integration
ctest -T Test -T "Tests" -o test-results.xml

# Custom output format
./nlm_test --verbose
```

## Python Test Setup

### Installation Requirements

#### System Dependencies
```bash
# Install Python and pip
# On Ubuntu/Debian:
apt-get update
apt-get install python3 python3-pip python3-dev

# On macOS:
brew install python3

# On Windows (using WSL or Git Bash):
# Already covered by base requirements
```

#### Python Package Dependencies
```bash
# Install from requirements.txt
pip install -r requirements.txt

# Or install directly
pip install pytest numpy scipy pybind11 scikit-build-core

# For development with editable installs
pip install -e .
```

### Test Environment Setup

#### Virtual Environment Setup
```bash
# Create virtual environment
python3 -m venv venv

# Activate virtual environment
source venv/bin/activate  # On Unix
# Or
venv\Scripts\activate      # On Windows

# Install dependencies in virtual environment
pip install -r requirements.txt
pip install -e .
```

#### Docker Environment
```dockerfile
# Dockerfile for testing
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    build-essential \
    cmake \
    git

WORKDIR /app

COPY . /app

RUN pip3 install -r requirements.txt
RUN pip3 install -e .

CMD ["python3", "-m", "pytest", "tests/"]
```

### Python Test Execution

#### Running Python Tests
```bash
# Run all Python tests
python3 -m pytest tests/ -v

# Run with coverage
python3 -m pytest tests/ --cov=pynlm --cov-report=html

# Run specific test files
python3 -m pytest tests/test_python_bindings.py
python3 -m pytest tests/test_cpython_integration.py

# Run with custom configuration
python3 -m pytest tests/ -k "test_brain" --tb=short
```

#### Test Command Line Options
```bash
# Run tests with specific options
python3 -m pytest tests/ \
    --verbose \
    --tb=short \
    --timeout=300 \
    --ignore=tests/test_benchmark.py

# Run tests in parallel
python3 -m pytest tests/ -n auto

# Run tests with xdist for distributed testing
python3 -m pytest tests/ --dist=loadscope
```

## Test Execution Procedures

### C++ Test Execution Procedures

#### Step-by-Step Execution

1. **Build Preparation**
```bash
# Create build directory
mkdir -p build
cd build

# Configure with tests
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build tests
make nlm_test
```

2. **Test Execution**
```bash
# Run comprehensive test suite
./nlm_test

# Or run with ctest
ctest --output-on-failure
```

3. **Output Analysis**
```bash
# Check test results
cat test_results.txt

# Generate test report
./nlm_test > test_output.txt 2>&1
```

4. **Validation**
```bash
# Verify expected output files
ls -la *.xml *.json *.txt

# Check for failures
grep -i "FAILED\|ERROR" test_output.txt
```

#### C++ Test Categories

1. **Core Functionality Tests**
   - Test types, configurations, and basic operations
   - Run: `./nlm_test` or `ctest --filter="test_types"`

2. **Neural Dynamics Tests**
   - Test neuron and synapse behavior
   - Run: `./nlm_test` or `ctest --filter="test_neuron"`

3. **Plasticity Tests**
   - Test STDP and Hebbian mechanisms
   - Run: `./nlm_test` or `ctest --filter="test_stdp"`

4. **System Integration Tests**
   - Test complete brain functionality
   - Run: `./nlm_test` or `ctest --filter="test_brain"`

5. **Comprehensive Tests**
   - Test all critical areas
   - Run: `./nlm_test` or `ctest --filter="test_comprehensive"`

### Python Test Execution Procedures

#### Step-by-Step Execution

1. **Environment Setup**
```bash
# Ensure Python environment is set up
python3 --version
pip --version

# Install dependencies
pip install -r requirements.txt
pip install -e .
```

2. **Test Execution**
```bash
# Run all Python tests
python3 -m pytest tests/ -v

# Run integration tests
python3 tests/test_python_bindings.py
python3 tests/test_cpython_integration.py
```

3. **Output Analysis**
```bash
# Check test results
python3 -m pytest tests/ --tb=short

# Generate coverage report
python3 -m pytest tests/ --cov=pynlm --cov-report=html
```

#### Python Test Categories

1. **Exception Hierarchy Tests**
   - Test custom exception classes
   - Run: `python3 tests/test_python_bindings.py` (within exception tests)

2. **Parameter Validation Tests**
   - Test configuration validation
   - Run: `python3 tests/test_python_bindings.py` (within parameter tests)

3. **Context Manager Tests**
   - Test resource cleanup
   - Run: `python3 tests/test_python_bindings.py` (within context tests)

4. **Factory Function Tests**
   - Test convenience factory methods
   - Run: `python3 tests/test_python_bindings.py` (within factory tests)

5. **Advanced Feature Tests**
   - Test power user features
   - Run: `python3 tests/test_python_bindings.py` (within advanced tests)

6. **Interoperability Tests**
   - Test C++/Python integration
   - Run: `python3 tests/test_cpython_integration.py`

## Test Teardown and Cleanup

### C++ Test Teardown

#### Cleanup Procedures
```bash
# Clean build artifacts
cd build
make clean
rm -rf *.o *.so *.a *.dylib

# Clean test results
rm -f test_results.txt
rm -f *.xml *.json
rm -f ctest-testDetails-*.xml
```

#### Resource Management
```cpp
// In test implementations
#include <memory>

// Use smart pointers for resource management
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

// Use RAII for test resources
class TestFixture {
public:
    TestFixture() {
        // Setup
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
    }
    
    ~TestFixture() {
        // Cleanup
        brain_.reset();
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};
```

### Python Test Teardown

#### Context Manager Cleanup
```python
# Using context managers for test cleanup
class TestEnvironment:
    def __enter__(self):
        # Setup
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup
        if hasattr(self.brain, 'reset'):
            self.brain.reset()
        # Clear references
        self.brain = None
        self.config = None
```

#### Temporary File Cleanup
```python
import tempfile
import os

# Use temporary files for test output
with tempfile.NamedTemporaryFile(mode='w', delete=False) as f:
    test_file = f.name
    # Write test output

# Clean up after use
os.unlink(test_file)

# Or use temporary directory
with tempfile.TemporaryDirectory() as temp_dir:
    # Write test files
    test_path = os.path.join(temp_dir, 'test_output.txt')
    with open(test_path, 'w') as f:
        f.write('test results')
    
    # Directory automatically cleaned up
```

## Advanced Testing Features

### C++ Advanced Testing Features

#### Mock Objects and Stubs
```cpp
// Create mock objects for testing
class MockNeuron {
public:
    virtual float getMembranePotential() const { return -70.0f; }
    virtual void setMembranePotential(float v) {}
    virtual bool isFiring() const { return false; }
};

// Test with mock objects
void testWithMockNeuron() {
    MockNeuron mock;
    assert(mock.getMembranePotential() == -70.0f);
    mock.setMembranePotential(-65.0f);
    assert(!mock.isFiring());
}
```

#### Test Parameters and Fixtures
```cpp
// Parameterized tests
class NeuronTest : public ::testing::TestWithParamInterface<float> {
protected:
    void SetUp() override {
        neuron_ = std::make_unique<Neuron>();
        test_value_ = GetParam();
    }
    
    void TestBody() override {
        neuron_->setMembranePotential(test_value_);
        EXPECT_EQ(neuron_->getMembranePotential(), test_value_);
    }
    
private:
    std::unique_ptr<Neuron> neuron_;
    float test_value_;
};

// Register parameterized tests
INSTANTIATE_TEST_SUITE_P(NeuronTest,
    ::testing::Values(-80.0f, -70.0f, -60.0f, -50.0f));
```

### Python Advanced Testing Features

#### Pytest Advanced Features
```python
# Test class with multiple fixtures
@pytest.mark.parametrize("neuron_type", ["excitatory", "inhibitory"])
@pytest.mark.parametrize("threshold", [-50.0, -55.0, -60.0])
class TestNeuronBehavior:
    def test_creation(self, neuron_type, threshold):
        # Test neuron creation with different parameters
        pass
    
    def test_activation(self, default_brain):
        # Test with default brain fixture
        pass
    
    @pytest.mark.skip(reason="Feature not yet implemented")
    def test_advanced_feature(self):
        pass
```

#### Custom Test Markers
```python
# Custom test markers for categorization
pytest.mark.unit = pytest.mark.skip(reason="Integration test required")
pytest.mark.integration = pytest.mark.skip(reason="Unit test only")
pytest.mark.performance = pytest.mark.skip(reason="Performance test")
pytest.mark.memory = pytest.mark.skip(reason="Memory intensive test")

# Use markers in tests
@pytest.mark.unit
def test_basic_functionality():
    pass

@pytest.mark.integration  
def test_complete_workflow():
    pass
```

## Troubleshooting and Common Issues

### Common Issues and Solutions

#### C++ Compilation Errors
```bash
# Issue: Cannot find header files
# Solution: Ensure source directory is correct
mkdir -p build
cd build
cmake .. -I../src

# Issue: Missing pybind11
# Solution: Install pybind11
pip install pybind11

# Issue: Compiler version mismatch
# Solution: Use supported compiler
gcc --version  # Check version
```

#### Python Import Errors
```bash
# Issue: Cannot import pynlm
# Solution: Build Python bindings first
python3 -m pip install -e .

# Issue: Missing dependencies
# Solution: Install requirements
pip install -r requirements.txt

# Issue: Virtual environment not activated
# Solution: Activate virtual environment
source venv/bin/activate
```

#### Test Failures
```bash
# Issue: Tests failing without clear error
# Solution: Run tests with verbose output
python3 -m pytest tests/ -v

# Issue: Memory issues in tests
# Solution: Limit memory usage
python3 -m pytest tests/ --max-memory=1024

# Issue: Slow test execution
# Solution: Use parallel execution
python3 -m pytest tests/ -n auto
```

### Diagnostic Commands

#### Test Environment Diagnosis
```bash
# Check build configuration
cat build/CMakeCache.txt | grep NLM

# Check test targets
make help | grep test

# Check Python environment
python3 -c "import sys; print(sys.path)"
python3 -c "import pynlm; print(pynlm.__version__)"
```

#### Test Output Diagnosis
```bash
# Analyze test output
python3 -m pytest tests/ --tb=long

# Generate detailed report
python3 -m pytest tests/ --json-report --json-report-file=report.json

# Check for specific test failures
python3 -m pytest tests/ -v --lf
```

## Performance Considerations

### Test Performance Optimization

#### C++ Performance Optimization
```bash
# Optimize build for tests
cmake .. -DCMAKE_BUILD_TYPE=Release -DNDEBUG=ON -O3

# Parallel build
make -j$(nproc)

# Use ccache for faster builds
ccache -M 20G
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

#### Python Performance Optimization
```bash
# Use PyPy for faster Python execution
pypy3 -m pytest tests/

# Optimize test collection
python3 -m pytest tests/ --collect-only

# Cache test results
python3 -m pytest tests/ --cache-clear
```

### Resource Management

#### Memory Management
```cpp
// Use memory pools in tests
class TestMemoryPool {
public:
    TestMemoryPool() {
        // Allocate memory pool
        pool_ = std::make_unique<MemoryPool<float>>(1024);
    }
    
    ~TestMemoryPool() {
        // Cleanup memory pool
        pool_.reset();
    }
    
private:
    std::unique_ptr<MemoryPool<float>> pool_;
};
```

#### File Handle Management
```python
import os
import tempfile

# Use context managers for file handling
with tempfile.NamedTemporaryFile(mode='w', delete=False) as f:
    file_path = f.name
    # Write to file

# Ensure file is closed and cleaned up
try:
    with open(file_path, 'r') as f:
        content = f.read()
finally:
    os.unlink(file_path)  # Clean up
```

## Continuous Integration Setup

### GitHub Actions Workflow

```yaml
# .github/workflows/test.yml
name: NLM Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test-cpp:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake g++ python3 python3-pip
    
    - name: Build tests
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    
    - name: Run C++ tests
      run: |
        cd build
        ctest --output-on-failure
  
  test-python:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: '3.8'
    
    - name: Install dependencies
      run: |
        pip install -r requirements.txt
        pip install -e .
    
    - name: Run Python tests
      run: |
        python3 -m pytest tests/ -v
```

### GitLab CI/CD Pipeline

```yaml
# .gitlab-ci.yml
test_cpp:
  stage: test
  script:
    - mkdir build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=Release
    - make -j4
    - ctest --output-on-failure
  only:
    - merge_requests
    - main

test_python:
  stage: test
  script:
    - pip install -r requirements.txt
    - pip install -e .
    - python3 -m pytest tests/ -v
  only:
    - merge_requests
    - main
```

### Local CI Setup

#### GitHub CLI
```bash
# Create GitHub Actions workflow locally
gitHub workflows create

# Run local CI checks
gitHub ci check
```

#### GitLab Runner
```bash
# Register local GitLab runner
sudo gitlab-runner register

# Run CI pipeline locally
sudo gitlab-runner run --docker-image alpine:latest
```

## Conclusion

This comprehensive test setup and teardown documentation provides complete guidance for setting up, running, and maintaining the NLM testing ecosystem. The documentation covers:

1. **Test Environment Setup**: Prerequisites, directory structure, and configuration
2. **Test Configuration**: Both C++ and Python test configuration options
3. **Test Execution Procedures**: Step-by-step instructions for running tests
4. **Test Teardown and Cleanup**: Proper resource management and cleanup procedures
5. **Advanced Testing Features**: Mock objects, parameterized tests, custom markers
6. **Troubleshooting and Common Issues**: Common problems and their solutions
7. **Performance Considerations**: Optimization techniques and resource management
8. **Continuous Integration Setup**: CI/CD pipeline configurations

The testing ecosystem is designed to be robust, maintainable, and scalable, supporting both local development and CI/CD workflows. All tests are organized by functionality and can be run individually or as part of the complete test suite.

**Key Features:**
- Comprehensive test coverage for all NLM functionality
- Strong support for both C++ and Python testing
- Advanced testing features and best practices
- Robust troubleshooting and error handling
- Performance-optimized test execution
- Full CI/CD integration support

This documentation ensures that developers can efficiently set up, run, and maintain the NLM testing infrastructure throughout the development lifecycle.