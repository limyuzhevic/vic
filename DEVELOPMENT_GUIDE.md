# NLM - Development Guide

## Overview

This development guide provides comprehensive information for developers working on the NLM project. It covers development setup, coding standards, testing, debugging, performance optimization, and best practices for maintaining and extending the codebase.

## Development Setup

### Prerequisites

#### System Requirements

| Requirement | Minimum | Recommended |
|-------------|----------|-------------|
| Operating System | Linux, macOS, Windows | Linux (latest) |
| Memory | 2GB RAM | 8GB+ RAM |
| CPU | 1 core | 4+ cores |
| Storage | 100MB free space | 1GB+ free space |
| Compiler | GCC 9+, Clang 7+, MSVC 2019+ | GCC 12+, Clang 12+ |
| CMake | 3.16 | 3.20+
| Python | 3.8 | 3.10+

#### Development Tools

- **Code Editor**: VS Code, CLion, or any IDE with C++ support
- **Version Control**: Git
- **Build Tools**: CMake, Make, Ninja
- **Package Manager**: pip (Python), apt (Linux), brew (macOS)
- **Debugging Tools**: GDB, LLDB, Visual Studio Code debugger
- **Profiling Tools**: perf, valgrind, Intel VTune

### Installing Dependencies

#### Linux/macOS

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    python3-dev \
    python3-pip \
    python3-venv

# Install Python dependencies
pip3 install pybind11 scikit-build-core numpy pytest matplotlib

# Clone and initialize NLM project
git clone https://github.com/yourusername/nlm.git
cd nlm
```

#### Windows

```powershell
# Install Chocolatey package manager
# Install Visual Studio Build Tools
# Install Python from python.org

# Clone NLM project
git clone https://github.com/yourusername/nlm.git
cd nlm

# Install Python dependencies
pip install pybind11 scikit-build-core numpy pytest matplotlib
```

### Project Structure

```
NLM/
├── CMakeLists.txt                    # Main CMake configuration
├── README.md                          # Project overview
├── docs/                              # Documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENE.md                    # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   └── ...
├── src/                              # Source code
│   ├── core/                        # Core utilities
│   │   ├── Config/                  # Configuration system
│   │   ├── Logger/                 # Logging system
│   │   └── ...
│   ├── brain/                       # Neural components
│   ├── agent/                       # Agent system
│   ├── world/                       # World simulation
│   ├── memory/                      # Memory systems
│   ├── neuromodulation/             # Neuromodulators
│   ├── development/                 # Developmental system
│   ├── prediction/                  # Prediction systems
│   ├── plasticity/                  # Plasticity mechanisms
│   └── ...
├── tests/                           # Test suite
├── examples/                        # Example projects
├── docs/                            # Documentation
│   ├── API_REFERENCE.md             # API documentation
│   ├── CONFIGURATION_GUIDE.md       # Configuration guide
│   ├── ERROR_HANDLING_REFERENCE.md # Error handling reference
│   ├── PERFORMANCE_GUIDE.md        # Performance guide
│   └── DEVELOPMENT_GUIDE.md        # Development guide
├── .gitignore                        # Git ignore patterns
├── pyproject.toml                  # Python package configuration
├── requirements.txt                 # Python requirements (if needed)
└── LICENSE                          # License file
```

## Coding Standards

### Code Style

#### C++ Code Style

- **Language**: C++17
- **Indentation**: 4 spaces
- **Line length**: 120 characters
- **Braces**: K&R style (if-else, for, while)
- **Naming conventions**:
  - Classes: PascalCase
  - Functions: camelCase
  - Variables: camelCase
  - Constants: UPPER_CASE

#### Python Code Style

- **Language**: Python 3.8+
- **Indentation**: 4 spaces
- **Line length**: 79 characters (black formatting)
- **Naming conventions**:
  - Classes: PascalCase
  - Functions: snake_case
  - Variables: snake_case
  - Constants: UPPER_CASE

### Code Quality Standards

#### File Structure

```cpp
// Example C++ file structure
#include "header.hpp"

namespace nlm {

class ClassName {
public:
    // Constructor/destructor
    ClassName() noexcept;
    ~ClassName() noexcept;
    
    // Public methods
    void publicMethod();
    int getValue() const noexcept;
    
    // Deleted methods
    ClassName(const ClassName&) = delete;
    ClassName& operator=(const ClassName&) = delete;
    
protected:
    // Protected methods
    void protectedMethod();
    
private:
    // Private members
    int private_value_;
    std::vector<double> private_data_;
};

} // namespace nlm
```

#### Python Class Structure

```python
# Example Python class structure
class ClassName:
    """Class documentation."""
    
    def __init__(self, parameter1, parameter2):
        """Initialize class."""
        self.parameter1 = parameter1
        self.parameter2 = parameter2
    
    def public_method(self):
        """Public method."""
        pass
    
    def _protected_method(self):
        """Protected method."""
        pass
    
    def __private_method(self):
        """Private method."""
        pass
    
    @property
    def property(self):
        """Property getter."""
        return self._property
    
    @property
    def property(self, value):
        """Property setter."""
        self._property = value
```

### Documentation Standards

#### Doxygen Comments (C++)

```cpp
/**
 * @class ClassName
 * @brief Brief description of the class.
 * 
 * Detailed description of the class, its purpose, and behavior.
 * 
 * @note Important notes about the class.
 * @warning Warning information.
 * @see Related classes/functions.
 * 
 * @par Examples:
 * @code
 * ClassName obj;
 * obj.method();
 * @endcode
 * 
 * @par File:
 * This class is defined in file.cpp
 * 
 * @par Since:
 * Version 1.0.0
 */
class ClassName {
    /**
     * @brief Brief description of the method.
     * 
     * Detailed description of the method, its parameters, and behavior.
     * 
     * @param[in] parameter1 Description of first parameter.
     * @param[out] parameter2 Description of second parameter.
     * @param[in] parameter3 Description of third parameter, with default value = 0.
     * 
     * @return Return value description.
     * @retval value1 Description of return value 1.
     * @retval value2 Description of return value 2.
     * 
     * @throws std::invalid_argument If parameter is invalid.
     * @throws std::runtime_error If operation fails.
     * 
     * @note This method is noexcept when no exceptions are thrown.
     * 
     * @see relatedMethod()
     * 
     * @par Examples:
     * @code
     * ClassName obj;
     * int result = obj.method(42);
     * @endcode
     */
    int method(int parameter1, double parameter2 = 0.0);
};
```

#### Docstring Comments (Python)

```python
class ClassName:
    """Brief description of the class.
    
    Detailed description of the class, its purpose, and behavior.
    
    Attributes:
        parameter1 (type): Description of first parameter.
        parameter2 (type): Description of second parameter.
    
    Args:
        parameter1 (type): Description of first parameter.
        parameter2 (type): Description of second parameter, default = 0.0.
    
    Returns:
        type: Description of return value.
        
    Raises:
        ValueError: If parameter is invalid.
        RuntimeError: If operation fails.
    
    Note:
        Important notes about the class.
    
    See Also:
        related_method(): Related function.
    
    Examples:
        >>> obj = ClassName(42)
        >>> result = obj.method()
        >>> print(result)
        42
    """
    
    def __init__(self, parameter1, parameter2=0.0):
        """Initialize class."""
        self.parameter1 = parameter1
        self.parameter2 = parameter2
    
    def method(self, parameter1, parameter2=0.0):
        """Brief description.
        
        Detailed description.
        
        Args:
            parameter1 (type): Description.
            parameter2 (type): Description, default = 0.0.
        
        Returns:
            type: Description.
        
        Raises:
            ValueError: If parameter invalid.
        
        Examples:
            >>> obj = ClassName(42)
            >>> result = obj.method(10)
        """
        pass
```

## Testing

### Testing Framework

#### C++ Testing

NLM uses Google Test (gtest) for testing:

```bash
# Install Google Test
mkdir build_gtest
cd build_gtest
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
make -j4
```

#### Python Testing

NLM uses pytest for Python testing:

```bash
# Install pytest
pip install pytest pytest-cov

# Run tests
pytest tests/ -v --cov=pynlm
```

### Unit Tests

#### C++ Unit Tests

```cpp
// Example test file
test_brain.cpp
#include "gtest/gtest.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace test {

TEST(BrainTest, Initialization) {
    Config config;
    config.set("brain.neuron_count", 100);
    
    Brain brain(config);
    EXPECT_NO_THROW(brain.initialize());
}

TEST(BrainTest, NeuronCount) {
    Config config;
    config.set("brain.neuron_count", 200);
    
    Brain brain(config);
    brain.initialize();
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 200);
}

TEST(BrainTest, StepExecution) {
    Config config;
    config.set("brain.neuron_count", 50);
    
    Brain brain(config);
    brain.initialize();
    
    EXPECT_NO_THROW(brain.step(0));
    EXPECT_NO_THROW(brain.step(1));
}

} // namespace test
} // namespace nlm
```

#### Python Unit Tests

```python
# Example test file
test_brain.py
import pynlm
import pytest

def test_brain_initialization():
    """Test brain initialization."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Should not raise exception
    brain.initialize()
    
    # Check initial state
    assert brain.getTotalNeuronCount() > 0

def test_brain_step():
    """Test brain simulation step."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run multiple steps
    for i in range(10):
        brain.step(i)
        
        # Check state
        assert brain.getFiringNeuronCount() >= 0

def test_brain_save_load():
    """Test brain save and load functionality."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    # Save state
    brain.save("test_checkpoint.bin")
    
    # Create new brain and load state
    brain2 = pynlm.createBrain(config)
    brain2.load("test_checkpoint.bin")
    
    # Check loaded state
    assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
```

### Integration Tests

#### C++ Integration Tests

```cpp
// Integration test for brain-world interaction
test_integration.cpp
#include "gtest/gtest.h"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

namespace nlm {
namespace test {

class BrainWorldIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_unique<Config>();
        config_->set("brain.neuron_count", 100);
        config_->set("brain.synapse_density", 0.1);
        
        brain_ = std::make_unique<Brain>(*config_);
        brain_->initialize();
        
        world_ = std::make_unique<SimpleWorld>();
        world_->configure(20, 20, 8, 8);
        world_->reset();
        
        agent_ = std::make_unique<AgentBrain>(*brain_);
        agent_->initialize(*world_);
    }
    
    void TearDown() override {
        brain_.reset();
        agent_.reset();
        world_.reset();
        config_.reset();
    }
    
    std::unique_ptr<Config> config_;
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<AgentBrain> agent_;
    std::unique_ptr<SimpleWorld> world_;
};

TEST_F(BrainWorldIntegrationTest, AgentStep) {
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        world_->update(0.1);
        
        auto percept = world_->getSensoryPercept();
        agent_->processSensoryInput(percept);
        
        brain_->step(step);
        
        auto action = agent_->decodeMotorCommand();
        world_->applyMotorCommand(action, world_->getSimulationTime());
    }
    
    // Verify state
    EXPECT_GT(brain_->getFiringNeuronCount(), 0);
    EXPECT_GT(agent_->getCuriosityLevel(), 0.0);
}

} // namespace test
} // namespace nlm
```

#### Python Integration Tests

```python
# Integration test for Python API
test_integration.py
import pynlm
import pytest

def test_complete_agent_simulation():
    """Test complete agent simulation with all components."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify subsystem states
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        assert brain.getDevelopmentProgress() >= 0.0
    
    # Final verification
    assert brain.getTotalNeuronCount() == 200
    assert agent.getNeuromodulationLevel() > 0.0

def test_error_handling():
    """Test error handling in Python API."""
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test invalid operation
    with pytest.raises(Exception):
        # This should raise an error
        pass
```

### Performance Testing

#### Performance Benchmarks

```cpp
// Performance benchmark
test_performance.cpp
#include "gtest/gtest.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <chrono>

namespace nlm {
namespace test {

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_unique<Config>();
        config_->set("brain.neuron_count", 1000);
        
        brain_ = std::make_unique<Brain>(*config_);
        brain_->initialize();
    }
    
    void TearDown() override {
        brain_.reset();
        config_.reset();
    }
    
    std::unique_ptr<Config> config_;
    std::unique_ptr<Brain> brain_;
};

TEST_F(PerformanceTest, StepsPerSecond) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain_->step(step);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // Calculate performance
    double steps_per_second = 1000.0 / (duration.count() / 1000000.0);
    
    // Expect at least 1000 steps per second
    EXPECT_GT(steps_per_second, 1000.0);
}

TEST_F(PerformanceTest, MemoryUsage) {
    // Measure memory usage
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain_->step(step);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // Check memory usage is reasonable
    // (Implementation depends on system)
    EXPECT_TRUE(true); // Placeholder
}

} // namespace test
} // namespace nlm
```

### Test Coverage

#### Coverage Requirements

- **Unit tests**: Minimum 80% coverage
- **Integration tests**: Minimum 70% coverage
- **Performance tests**: All performance-critical paths
- **Error handling**: All error conditions

#### Coverage Tools

```bash
# Install coverage tools
cmake .. -DENABLE_COVERAGE=ON
make -j4

# Run tests with coverage
ctest --output-on-failure

# Generate coverage report
coveragereport --output=coverage.html
```

## Debugging

### Debugging Tools

#### GDB Debugging

```bash
# Start GDB with core dump
gdb --args ./nlm_simulation
core-file core.dump

# Set breakpoints
break main
break Brain::step

# Continue execution
continue

# Examine variables
print brain.getTotalNeuronCount()
print brain.getFiringNeuronCount()
```

#### LLDB Debugging

```bash
# Start LLDB
lldb ./nlm_simulation

# Set breakpoints
breakpoint set -n main
breakpoint set -n Brain::step

# Continue execution
process launch
process continue

# Examine variables
frame variable brain
frame variable neuron_count
```

#### Visual Studio Code Debugging

```json
// launch.json configuration
{
    "name": "Debug NLM",
    "type": "cppvsdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/nlm_phase6_demo",
    "args": [],
    "stopAtEntry": "false",
    "cwd": "${workspaceFolder}",
    "environment": [],
    "externalConsole": false,
    "sourceFileMap": {},
    "preLaunchTask": "build"
}
```

### Debugging Techniques

#### Breakpoint Debugging

```cpp
// Add breakpoints
void Brain::step(int step_number) noexcept {
    // Check for error conditions
    if (step_number < 0) {
        NLM_THROW_CONFIG("Step number cannot be negative");
    }
    
    // Add debug output
    if (step_number % 100 == 0) {
        std::cout << "Step " << step_number << std::endl;
    }
    
    // Process neurons
    for (auto& neuron : neurons_) {
        neuron.update(dt_);
        if (neuron.shouldSpike()) {
            neuron.spike();
            // Debug output
            std::cout << "Neuron " << neuron.getId() 
                      << " spiked at step " << step_number << std::endl;
        }
    }
}
```

#### Logging Debugging

```cpp
#include "core/Logger/Logger.hpp"

void setupDebugLogging() {
    // Initialize logger with debug level
    initializeLogger("debug.log", LogLevel::Debug);
    
    // Set log format
    setLogFormat("[%(timestamp)s] %(level)s: %(message)s");
}

void debugBrainStep(const Brain& brain, int step) {
    LOG_DEBUG("Brain step " + std::to_string(step));
    LOG_DEBUG("Neuron count: " + std::to_string(brain.getTotalNeuronCount()));
    LOG_DEBUG("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    LOG_DEBUG("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
}
```

#### Memory Debugging

```bash
# Use valgrind for memory debugging
valgrind --leak-check=full --show-leak-kinds=all ./nlm_simulation

# Use address sanitizer
suppress_warnings=0
export ASAN_OPTIONS=detect_leaks=1
./nlm_simulation
```

### Common Debugging Issues

#### Issue 1: Segmentation Fault

**Symptoms**:
- Program crashes with segmentation fault
- Often occurs with null pointer dereference

**Solutions**:
```cpp
// Check for null pointers
void Brain::step(int step_number) noexcept {
    if (!brain_) {
        NLM_THROW_CONFIG("Brain not initialized");
    }
    
    // Check for valid neurons
    for (const auto& neuron : neurons_) {
        if (!neuron.isValid()) {
            NLM_THROW_CONFIG("Invalid neuron in brain");
        }
    }
}
```

#### Issue 2: Memory Leaks

**Symptoms**:
- Memory usage increases over time
- Valgrind shows memory leaks

**Solutions**:
```cpp
// Use smart pointers
class Brain {
private:
    std::vector<std::unique_ptr<Neuron>> neurons_;
    // ...
};

// Check for memory leaks
void checkMemoryLeaks() {
    #ifdef _WIN32
    _CrtDumpMemoryLeaks();
    #endif
}
```

#### Issue 3: Configuration Errors

**Symptoms**:
- Invalid configuration parameters
- Unexpected behavior

**Solutions**:
```cpp
// Validate configuration
void Brain::initialize() noexcept(false) {
    const auto& config = getConfig();
    
    // Validate required parameters
    if (!config.has("brain.neuron_count")) {
        NLM_THROW_CONFIG("Missing required parameter: brain.neuron_count");
    }
    
    // Validate parameter ranges
    int neuron_count = config.get<int>("brain.neuron_count");
    if (neuron_count <= 0) {
        NLM_THROW_CONFIG("Neuron count must be positive");
    }
}
```

## Build and Deployment

### Building with CMake

#### Standard Build

```bash
# Create build directory
mkdir build
cd build

# Configure with default options
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4
```

#### Development Build

```bash
# Create debug build directory
mkdir build_debug
cd build_debug

# Configure with debug options
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON -DENABLE_COVERAGE=ON

# Build
make -j4
```

#### Python Bindings Build

```bash
# Install build dependencies
pip install pybind11 scikit-build-core numpy

# Build Python module
cd python
python -m build --wheel

# Or install in development mode
pip install -e .
```

### Build Options

#### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type (Debug, Release, RelWithDebInfo) |
| `BUILD_TESTING` | OFF | Build tests |
| `ENABLE_COVERAGE` | OFF | Enable code coverage |
| `ENABLE_PYTHON_BINDINGS` | ON | Enable Python bindings |
| `ENABLE_DOCUMENTATION` | OFF | Enable documentation generation |
| `CMAKE_CXX_STANDARD` | 17 | C++ standard |
| `CMAKE_CXX_STANDARD_REQUIRED` | ON | Require C++ standard |

#### Build Scripts

```bash
#!/bin/bash
# build.sh - Build script for NLM

set -e

cd "$(dirname "$0")"

# Create build directory
mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DBUILD_TESTING=ON \
         -DENABLE_COVERAGE=ON

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Build Python bindings
cd ..
cd python
python -m build --wheel
```

### Deployment

#### Linux Deployment

```bash
# Install system-wide
sudo mkdir -p /usr/local/lib/nlm
sudo cp build/nlm /usr/local/bin/
sudo cp build/libnlm_core.so /usr/local/lib/
sudo cp -r python/dist/pynlm*.whl /usr/local/lib/

# Install Python package
sudo pip install python/dist/pynlm*.whl
```

#### Development Deployment

```bash
# Create virtual environment for development
python3 -m venv venv
source venv/bin/activate
pip install -e ./python
```

### Installation Verification

#### Verify C++ Installation

```bash
# Check if executable exists
./build/nlm_phase6_demo

# Run demo
./build/nlm_phase6_demo --help
```

#### Verify Python Installation

```python
import pynlm
print(f"NLM Python bindings version: {pynlm.__version__}")

# Test basic functionality
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

## Version Control

### Git Workflow

#### Branch Strategy

- **main**: Main development branch
- **develop**: Feature development branch
- **feature/***: Feature branches
- **bugfix/***: Bug fix branches
- **release/***: Release preparation branches

#### Commit Convention

```
<type>(<scope>): <short description>

<body>

<footer>

Co-authored-by: Kilo <kilo@kilo.ai>
```

**Commit Types**:
- **feat**: New feature
- **fix**: Bug fix

<arg_value>Implementing user authentication system
Co-authored-by: Kilo <kilo@kilo.ai>

Co-authored-by: Kilo <kilo@kilo.ai> appears twice

## Custom Commands

### 1. Build Performance Report

```bash
#!/bin/bash
# generate_perf_report.sh

# Create performance report directory
mkdir -p reports

# Run performance tests
./nlm_performance_test > reports/performance_results.txt

# Generate graph
python3 scripts/plot_performance.py reports/performance_results.txt reports/performance.png

# Create summary
python3 scripts/summarize_performance.py reports/performance_results.txt > reports/summary.txt

# Compress report
zip -r reports.zip reports/
```

### 2. Run All Tests

```bash
#!/bin/bash
# run_all_tests.sh

set -e

echo "Running all tests..."

# Build tests
mkdir -p build_test
cd build_test
cmake .. -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run C++ tests
ctest --output-on-failure

# Run Python tests
cd ..
pytest tests/ -v --cov=pynlm --cov-report=term-missing

# Run integration tests
python3 tests/integration_test.py

echo "All tests completed successfully!"
```

### 3. Clean Build

```bash
#!/bin/bash
# clean_build.sh

# Remove build directories
rm -rf build
rm -rf build_debug
rm -rf build_test

# Remove Python build artifacts
rm -rf dist
rm -rf build
rm -rf *.egg-info

# Remove documentation output
rm -rf docs/html
rm -rf docs/latex

# Remove test coverage reports
rm -rf coverage
rm -rf *.coverage

# Remove temporary files
rm -f *.log
rm -f *.prof
rm -f *.cache

# Remove Python cache
find . -name "__pycache__" -type d -exec rm -rf {} +
find . -name "*.pyc" -delete
find . -name "*.pyo" -delete
```

## Environment Configuration

### Development Environment

#### VS Code Settings

```json
// .vscode/settings.json
{
    "C_Cpp.clangFormatPath": ".clang-format",
    "C_Cpp.clangTidyEnabled": true,
    "C_Cpp.intelliSenseEngine": "enabled",
    "files.exclude": {
        "**/.git": true,
        "**/.svn": true,
        "**/.hg": true,
        "**/.gitignore": true,
        "build/": true,
        "dist/": true
    },
    "search.exclude": {
        "build/": true,
        "dist/": true
    }
}
```

#### CLion Settings

```xml
// .idea/cmake.xml
<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
    <component name="CMakeWorkspace">
        <cmakeExecutable>/usr/bin/cmake</cmakeExecutable>
        <generator>Unix Makefiles</generator>
        <buildType>Release</buildType>
        <projectFileDirectory>/workspace/app</projectFileDirectory>
    </component>
</project>
```

### Environment Variables

#### Required Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_HOME` | NLM installation directory | "/usr/local" |
| `NLM_DEBUG` | Enable debug mode | "0" |
| `NLM_LOG_LEVEL` | Logging level | "INFO" |

#### Optional Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_NEURON_COUNT` | Number of neurons | "1000" |
| `NLM_SYNAPSE_DENSITY` | Synapse density | "0.1" |
| `NLM_PYTHON_PATH` | Python path | "python" |
| `NLM_DOCUMENTATION_PATH` | Documentation path | "docs" |

#### Environment Setup Script

```bash
#!/bin/bash
# setup_environment.sh

# Set environment variables
export NLM_HOME=${NLM_HOME:-"/usr/local"}
export NLM_DEBUG=${NLM_DEBUG:-"0"}
export NLM_LOG_LEVEL=${NLM_LOG_LEVEL:-"INFO"}

# Set Python path
if [ -z "$NLM_PYTHON_PATH" ]; then
    export NLM_PYTHON_PATH="$(cd python && pwd)"
fi

# Add NLM to PATH
export PATH="$NLM_HOME/bin:$PATH"

# Set LD_LIBRARY_PATH
export LD_LIBRARY_PATH="$NLM_HOME/lib:$LD_LIBRARY_PATH"

# Source profile
echo "NLM environment variables set."
echo "NLM_HOME: $NLM_HOME"
echo "NLM_DEBUG: $NLM_DEBUG"
echo "NLM_LOG_LEVEL: $NLM_LOG_LEVEL"
```

## Best Practices

### 1. Error Handling

- Always validate input parameters
- Use specific exception types
- Implement graceful degradation
- Provide meaningful error messages
- Log errors for debugging

### 2. Memory Management

- Use smart pointers for dynamic memory
- Pre-allocate memory pools for frequent allocations
- Check for memory errors
- Monitor memory usage
- Clean up resources properly

### 3. Performance Optimization

- Profile code to identify bottlenecks
- Use appropriate algorithms for the problem
- Optimize data layouts for cache efficiency
- Use parallel processing where appropriate
- Monitor performance continuously

### 4. Testing

- Write comprehensive unit tests
- Implement integration tests
- Run performance tests
- Test error conditions
- Use continuous integration

### 5. Documentation

- Document code with comments
- Provide API documentation
- Write user guides
- Include examples
- Keep documentation up-to-date

### 6. Version Control

- Use meaningful commit messages
- Follow branching strategy
- Write informative commit messages
- Include co-author information
- Use pull requests for code review

## Conclusion

This development guide provides comprehensive information for developers working on the NLM project. By following these guidelines, developers can:

1. **Set up development environment** efficiently
2. **Write high-quality code** following best practices
3. **Test thoroughly** with unit and integration tests
4. **Debug effectively** using available tools
5. **Build and deploy** applications reliably
6. **Maintain code quality** through consistent standards

Key takeaways:

1. **Follow coding standards** for consistency and maintainability
2. **Write comprehensive tests** for reliability
3. **Profile and optimize** performance continuously
4. **Document code** thoroughly for future developers
5. **Use version control** effectively for collaboration

By adhering to these development guidelines, the NLM project can maintain high code quality, achieve optimal performance, and support continuous development and improvement.

---

*This development guide provides comprehensive information about NLM development.*
*For configuration details, see CONFIGURATION_GUIDE.md.*
*For API reference, see API_REFERENCE.md.*
*For error handling, see ERROR_HANDLING_REFERENCE.md.*
*For performance, see PERFORMANCE_GUIDE.md.*
*For usage, see HOW_TO_USE.md and easy_usage.md.*

**Last updated**: 2024-01-17
**Version**: 1.0.0

*This documentation is generated from source code comments and maintained by the NLM development team.*

