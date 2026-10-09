# NLM Developers Guide

## Overview

This guide provides comprehensive information for developers who want to contribute to the NLM project, extend its functionality, or develop new features. It covers development setup, coding standards, testing procedures, and contribution guidelines.

## Development Setup

### Prerequisites

#### System Requirements
- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++20 compatible compiler (gcc 9+, clang 12+, MSVC 2019+)
- **Python**: Python 3.8 or higher
- **CMake**: 3.16 or higher
- **Build Tools**: make, ninja, or MSBuild

#### Python Dependencies
```bash
# Install Python dependencies
pip install -r requirements.txt

# Install development dependencies
pip install -r requirements-dev.txt
```

#### NLM-Specific Dependencies
```bash
# Install NLM Python bindings in development mode
pip install -e .

# Install all optional dependencies
pip install pynlm[dev]
```

### Build Instructions

#### Standard Build
```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

#### Python Build
```bash
# Build Python bindings
cd python
pip install -e .
```

#### Development Build
```bash
# Build with debugging and development features
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_PYTHON_BINDINGS=ON -DENABLE_TESTS=ON
make -j4
```

#### Cross-Platform Build
```bash
# Windows (Visual Studio)
cmake .. -G "Visual Studio 16 2019" -A x64

# macOS
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64 arm64"

# Linux with custom options
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PYTHON_BINDINGS=ON -DBUILD_TESTS=ON
```

### Project Structure

```
NLM/
├── src/                    # Core source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components
│   ├── agent/             # Agent interface
│   ├── world/             # Environment simulation
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── memory/            # Memory systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── neuromodulation/    # Neuromodulators
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Development system
│   ├── prediction/        # Prediction systems
│   ├── dynamics/          # Neural dynamics
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization tools
├── src/cli/                # Command Line Interface
│   ├── nlm_cli.cpp        # Main CLI entry point
│   ├── CLICommand.hpp/.cpp # Command framework
│   ├── CLIParser.hpp/.cpp  # Argument parsing
│   ├── HelpSystem.hpp/.cpp # Help system
│   └── commands/           # Individual command implementations
├── python/                 # Python bindings
│   ├── bindings.cpp        # Main Python bindings
│   └── examples/           # Python examples
├── tests/                  # Test suite
│   ├── unit/              # Unit tests
│   ├── integration/       # Integration tests
│   └── performance/       # Performance tests
├── docs/                   # Documentation
│   ├── index.md           # Documentation index
│   ├── README.md          # Project overview
│   ├── easy_usage.md      # Easy usage guide
│   ├── API_REFERENCE.md   # API documentation
│   ├── CLI_REFERENCE.md   # CLI documentation
│   └── PYTHON_TUTORIAL.md # Python tutorial
├── examples/               # Working examples
│   ├── basic/             # Basic examples
│   ├── advanced/          # Advanced examples
│   └── scripts/           # Example scripts
├── configs/                # Configuration files
│   ├── default.cfg        # Default configuration
│   ├── examples/          # Example configurations
│   └── templates/         # Configuration templates
├── docs/                   # Technical documentation
│   ├── ARCHITECTURE.md    # System architecture
│   ├── SCIENCE.md         # Scientific foundations
│   ├── RESEARCH_METHODS.md # Research methodology
│   └── EXPERIMENTS.md      # Experimental setup
├── pyproject.toml         # Python package configuration
├── CMakeLists.txt          # CMake configuration
├── README.md               # Project README
├── easy_usage.md           # Easy usage guide
└── LICENSE                 # License file
```

### Development Environment Setup

#### Environment Configuration

```bash
# Set up development environment
export NLM_HOME=/path/to/nlm
export PATH=$NLM_HOME/build/bin:$PATH
export PYTHONPATH=$NLM_HOME/python:$PYTHONPATH

# Create virtual environment
python -m venv nlm_env
source nlm_env/bin/activate

# Install dependencies
pip install -r requirements.txt
pip install -r requirements-dev.txt
```

#### IDE Configuration

#### Visual Studio Code

```json
// .vscode/settings.json
c "cmake.buildDirectory" "${workspaceFolder}/build"
"cmake.configureArgs" [
    "-DCMAKE_BUILD_TYPE=Debug",
    "-DENABLE_PYTHON_BINDINGS=ON",
    "-DBUILD_TESTS=ON"
],
"python.pythonPath": "${workspaceFolder}/venv/bin/python",
"editor.formatOnSave": true,
"cmake.useCMakeConfigFromCppSettings": true
```

#### JetBrains IDEs

Add CMake integration and Python support to your IDE.

#### Makefile Configuration

```makefile
# Makefile for NLM development

# Variables
BUILD_DIR = build
SRC_DIR = src
PYTHON_DIR = python
TESTS_DIR = tests

# Default target
all: build-all

# Build all components
build-all: build-core build-python build-tests

# Build core components
build-core:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release
	cd $(BUILD_DIR) && make -j$(shell nproc)

# Build Python bindings
build-python:
	cd $(PYTHON_DIR) && pip install -e ..

# Build tests
build-tests:
	cd $(BUILD_DIR) && make tests

# Run tests
run-tests:
	cd $(BUILD_DIR) && ctest --output-on-failure

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.egg-info
	rm -rf dist

# Install development dependencies
install-dev:
	pip install -r requirements-dev.txt
	pip install -r requirements.txt

# Generate documentation
doc:
	doxygen Doxyfile

.PHONY: all build-all build-core build-python build-tests run-tests clean install-dev doc
```

---

## Coding Standards

### Code Style Guidelines

#### Naming Conventions

**File Names**
- Source files: `*.cpp`, `*.hpp`
- Header files: `*.h`, `*.hpp`
- Python files: `*.py`
- Documentation: `*.md`
- Configuration files: `*.cfg`, `*.json`

**Class Names**
- Use `CamelCase` for class names
- Example: `Brain`, `Config`, `AgentBrain`

**Function Names**
- Use `snake_case` for function names
- Example: `getTotalNeuronCount()`, `createBrain()`

**Variable Names**
- Use `snake_case` for variables
- Example: `brain_name`, `simulation_steps`

**Constant Names**
- Use `UPPER_SNAKE_CASE` for constants
- Example: `PI = 3.14159`, `MAX_NEURONS = 10000`

#### Code Formatting

**Indentation**
- Use 4 spaces for indentation
- Never use tabs
- Consistent indentation throughout the file

**Line Length**
- Maximum line length: 120 characters
- Break long lines for readability
- Use continuation lines for logical expressions

**File Organization**
- Include guard in header files
- `#pragma once` preferred over include guards
- Include dependencies in order: system, standard, project
- Forward declarations before includes when possible

#### Documentation Standards

**Comments**
- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Document public APIs with Doxygen-style comments
- Include parameter descriptions and return value documentation

**Doxygen Comments**
```cpp
/**
 * @class Brain
 * @brief Central neural simulation brain class
 * @details Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems
 *
 * @note This class is the main interface for NLM brain operations
 * @see Config, AgentBrain, SimpleWorld
 */
class Brain {
    // ... implementation ...
};
```

**Python Docstrings**
```python
"""Python bindings for NLM.

This module provides Python interfaces to NLM's neural simulation capabilities.

Features:
- Brain creation and management
- World simulation
- Agent interaction
- Advanced analysis tools

Examples:
    >>> import pynlm
    >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
    >>> brain.initialize()
    >>> brain.step(0)
    >>> print(brain.getTotalNeuronCount())
    1000
"""
```

### Code Quality Guidelines

#### Error Handling

**C++ Error Handling**
```cpp
#include <stdexcept>
#include <string>

class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Use specific exception types
class BrainCreationError : public NLMError {
public:
    BrainCreationError(const std::string& message) : NLMError(message) {}
};

// Exception handling
try {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
} catch (const BrainCreationError& e) {
    std::cerr << "Failed to create brain: " << e.what() << std::endl;
    return 1;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
    return 1;
}
```

**Python Error Handling**
```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected Error: {e}")
    # Log error and exit
    sys.exit(1)
```

#### Memory Management

**C++ Memory Management**
```cpp
#include <memory>

// Use smart pointers
auto config = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(config);

// Avoid raw pointers unless necessary
// Prefer: std::shared_ptr<T>
// Over: T*

// Use move semantics when appropriate
Brain brain1(std::move(brain2)); // Move constructor
```

**Python Memory Management**
```python
import pynlm

# Python handles memory management automatically
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# Clear references when done
brain = None
```

#### Testing Standards

**Unit Testing**
```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Test fixture
class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void TearDown() override {
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

// Test cases
TEST_F(BrainTest, Creation) {
    EXPECT_NE(brain, nullptr);
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
}

TEST_F(BrainTest, Step) {
    size_t initial_spikes = brain->getTotalSpikeCount();
    brain->step(0);
    EXPECT_GT(brain->getTotalSpikeCount(), initial_spikes);
}
```

**Python Testing**
```python
import unittest
import pynlm

class TestNLMOperations(unittest.TestCase):
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after test."""
        self.brain = None
        self.config = None
    
    def test_brain_creation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        self.brain.initialize()
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
    
    def test_brain_step(self):
        """Test brain stepping."""
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        self.assertGreater(self.brain.getTotalSpikeCount(), initial_spikes)

if __name__ == '__main__':
    unittest.main()
```

---

## Development Workflow

### Branch Management

#### Branch Naming Convention

- `main` - Main development branch
- `feature/` - New feature branches
- `bugfix/` - Bug fix branches
- `docs/` - Documentation branches
- `release/` - Release branches

#### Branch Creation

```bash
# Create feature branch
git checkout main
git checkout -b feature/new-cli-tool

# Create bugfix branch
feature_branch=$(git rev-parse --abbrev-ref HEAD)
git checkout main
git checkout -b bugfix/fix-duplicate-config
```

#### Branch Merging

```bash
# Merge feature branch into main
git checkout main
git merge feature/new-cli-tool

# Create pull request
gh pr create --title "Add unified CLI tool" --body "This PR adds a unified CLI tool..."
```

### Commit Standards

#### Commit Message Format

```
<type>: <description>

<body>

<footers>
```

#### Commit Types

- **feat**: New feature
- **fix**: Bug fix

For the commit message format and guidelines, check the contributing guidelines.

### Pull Request Process

#### PR Checklist

- [ ] Code follows project coding standards
- [ ] Tests pass
- [ ] Documentation updated
- [ ] No linting errors
- [ ] Updated CHANGELOG.md
- [ ] Updated version in pyproject.toml

#### PR Review Process

1. **Initial Review**: Code review by team members
2. **Technical Review**: Technical correctness and architecture
3. **Testing Review**: Test coverage and reliability
4. **Documentation Review**: Documentation completeness
5. **Merge Approval**: Final approval and merge

---

## Build System

### CMake Configuration

#### CMakeLists.txt

```cmake
# NLM CMake Configuration
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.2.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_DOCUMENTATION "Build documentation" OFF)

# Include directories
include_directories(src)
include_directories(src/core)
include_directories(src/brain)
include_directories(src/agent)
include_directories(src/world)

# Add subdirectories
add_subdirectory(src)

# Build Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Build tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Build examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install targets
install(TARGETS nlm DESTINATION bin)
install(TARGETS nlm_cli DESTINATION bin)
install(DIRECTORY configs/ DESTINATION share/nlm/configs)
```

#### pyproject.toml

```toml
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"

[project]
name = "pynlm"
version = "0.2.0"
description = "Enhanced Python bindings for NLM"
readme = "README.md"
license = {text = "MIT"}
requires-python = ">=3.8"

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "numpy>=1.20",
]

[tool.scikit-build]
cmake.minimum-version = "3.16"
cmake.build-type = "Release"

[tool.scikit-build.cmake.define]
NLM_SOURCE_DIR = "src"
```

### Testing Framework

#### Unit Testing

```cpp
// tests/unit/TestBrain.cpp
#include <gtest/gtest.h>
#include "Brain.hpp"

class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

TEST_F(BrainTest, Initialization) {
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
}

TEST_F(BrainTest, Step) {
    size_t initial_spikes = brain->getTotalSpikeCount();
    brain->step(0);
    EXPECT_GT(brain->getTotalSpikeCount(), initial_spikes);
}
```

#### Integration Testing

```bash
# Run integration tests
cd build
ctest --output-on-failure

# Run specific test
ctest -R BrainTest
```

#### Performance Testing

```bash
# Build with performance tests
mkdir build_perf
cd build_perf
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PERFORMANCE_TESTS=ON
make -j4

# Run performance tests
./nlm --performance-test
```

---

## Contribution Guidelines

### Code Contributions

#### Pull Request Process

1. **Fork the repository**
2. **Create a feature branch**
3. **Make your changes**
4. **Test your changes**
5. **Create a pull request**
6. **Review and iterate**

#### Code Review Checklist

- [ ] Code follows project coding standards
- [ ] Added comprehensive tests
- [ ] Updated documentation
- [ ] No linting errors
- [ ] Follows existing patterns
- [ ] Minimal changes

### Documentation Contributions

#### Documentation Guidelines

- Keep documentation up-to-date with code changes
- Use consistent formatting and style
- Provide clear examples and explanations
- Include API references for all public interfaces

#### Documentation Types

- **README.md**: Project overview and quick start
- **API_REFERENCE.md**: Complete API documentation
- **PYTHON_TUTORIAL.md**: Python-specific tutorial
- **CLI_REFERENCE.md**: Command-line interface reference
- **DEVELOPERS_GUIDE.md**: Developer and contributor guide

### Bug Reports

#### Bug Report Format

```
## Bug Report

### Description
[Clear description of the bug]

### Steps to Reproduce
1. [Step 1]
2. [Step 2]
3. [Step 3]

### Expected Behavior
[What should happen]

### Actual Behavior
[What actually happens]

### Environment
- OS: [Operating system]
- Compiler: [Compiler version]
- Python: [Python version]
- NLM: [NLM version]

### Additional Information
[Any additional relevant information]
```

### Bug Fix Process

1. **Reproduce the bug**
2. **Investigate the root cause**
3. **Create a test case**
4. **Implement a fix**
5. **Test the fix**
6. **Create a pull request**

---

## Quality Assurance

### Continuous Integration

#### GitHub Actions

```yaml
# .github/workflows/ci.yml
name: Continuous Integration

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up C++ environment
      uses: ubuntu-actions/setup-cpp@v1
      with:
        compiler: g++
        cppstd: 20
        
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: '3.10'
        
    - name: Install dependencies
      run: |
        pip install -r requirements.txt
        pip install -r requirements-dev.txt
        
    - name: Build
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
        
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure
        
    - name: Run Python tests
      run: |
        python -m pytest tests/ -v
```

#### Code Quality Checks

```bash
# Linting
cppcheck src/
pylint python/

# Formatting
clang-format --style=file --dry-run src/
black python/
isort python/

# Static analysis
gcc -Wall -Wextra -Werror -std=c++20 src/ -o build/nlm
```

### Performance Testing

#### Performance Benchmarks

```bash
# Build performance benchmarks
mkdir build_perf
cd build_perf
cmake .. -DBUILD_PERFORMANCE_TESTS=ON
make -j4

# Run performance benchmarks
./nlm --performance-benchmark

# Generate performance report
python scripts/generate_perf_report.py
```

### Security Testing

#### Security Checks

```bash
# Run security scans
semgrep --include="*.cpp" --include="*.hpp" --include="*.py" .
gitleaks --scan .github

# Check for common vulnerabilities
audit -p requirements.txt
```

---

## Release Process

### Version Management

#### Semantic Versioning

- **Major version**: Significant changes, breaking changes
- **Minor version**: New features, backward compatible
- **Patch version**: Bug fixes, minor improvements

#### Version Bumping

```bash
# Bump major version
npm version major

# Bump minor version
semver bump minor

# Bump patch version
semver bump patch
```

### Release Checklist

- [ ] Update version in pyproject.toml
- [ ] Update CHANGELOG.md
- [ ] Run all tests
- [ ] Update documentation
- [ ] Create release notes
- [ ] Create release tag
- [ ] Push to remote repository
- [ ] Publish to package managers

### Release Workflow

```bash
# Release process
1. git checkout main
2. git pull origin main
3. bump version in pyproject.toml
4. run_tests
5. build_package
6. create_release_notes
7. create_release_tag
8. push_changes
9. publish_package
```

---

## Future Development

### Planned Features

#### Phase 2: Enhanced Features
- **Advanced Visualization**: 3D brain models and activity displays
- **Distributed Computing**: Multi-node simulation support
- **Hardware Acceleration**: GPU support for neural computation
- **Cloud Integration**: Cloud-based simulation environments
- **Web Interface**: Browser-based brain monitoring and control

#### Phase 3: Research Extensions
- **Neuromodulation Studies**: Investigation of multi-system interaction
- **Memory Integration**: Deep integration of multiple memory systems
- **Development Psychology**: Cognitive development modeling
- **Learning Theory**: Testing computational learning theories
- **System Architecture**: Novel brain-inspired computing paradigms

### Research Directions

#### Current Research Areas
1. **Memory Integration**: How memory systems interact with neural dynamics
2. **Neuromodulation**: Role of neuromodulators in learning and behavior
3. **Development**: How developmental stages affect plasticity and learning
4. **Prediction**: How prediction becomes central organizing principle
5. **Self-Organization**: How brain organizes itself through interaction

#### Future Research Opportunities
1. **Cross-species comparison**: Compare NLM with biological brains
2. **Hybrid systems**: Combine NLM with traditional AI methods
3. **Neuromorphic hardware**: Implement NLM on neuromorphic chips
4. **Real-world applications**: Apply NLM to real-world problems
5. **Educational tools**: Use NLM for education and training

---

## Conclusion

This Developers Guide provides comprehensive information for developers who want to contribute to the NLM project. It covers:

- **Development setup** and environment configuration
- **Coding standards** and best practices
- **Testing procedures** and quality assurance
- **Contribution guidelines** and review process
- **Build system** and continuous integration
- **Release process** and version management

The NLM project is a collaborative research platform where developers can contribute to advancing artificial developmental brain research. By following these guidelines, contributors can ensure high-quality code that maintains the project's standards and values.

**Contribute to NLM today and help shape the future of artificial developmental brains!** 🚀

---

## Resources

### Additional Reading

- **Official Documentation**: docs/ directory contains detailed documentation
- **GitHub Repository**: https://github.com/neural-learning-machine/nlm
- **Issues Tracker**: https://github.com/neural-learning-machine/nlm/issues
- **Discussions**: https://github.com/neural-learning-machine/nlm/discussions

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Community**: Ask questions and share experiences
- **Documentation**: Read comprehensive guides and tutorials
- **Slack/Discord**: Join NLM community discussions

### Contribution Guidelines

- **CONTRIBUTING.md**: Detailed contribution guidelines
- **CODE_OF_CONDUCT.md**: Project code of conduct
- **LICENSE.md**: Project license information
- **CHANGELOG.md**: Project changelog and release history

---

## Contact Information

### Development Team

- **Email**: dev@nlm.org
- **GitHub**: https://github.com/neural-learning-machine
- **Slack**: nlm.slack.com
- **Discord**: discord.gg/nlm

### Support

- **Bug Reports**: Open issues on GitHub
- **Feature Requests**: Submit feature requests on GitHub
- **Documentation Requests**: Request documentation updates on GitHub
- **Technical Support**: Contact development team

---

*This Developers Guide is part of the NLM project and is licensed under the MIT License. See LICENSE file for details.*