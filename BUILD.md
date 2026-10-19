# NLM Build and Development Documentation

# Overview

This document provides comprehensive instructions for building, developing, and maintaining the NLM (Neural Learning Machine) project. It covers both C++ core development and Python bindings, including setup, building, testing, and deployment.

# Quick Start

## Building with CMake

```bash
# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Install (optional)
sudo make install
```

## Building Python Bindings

```bash
# Build Python bindings using scikit-build
cd python
python -m pip install -e .
```

## Running Tests

```bash
# Build and run C++ tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./nlm_test

# Or run with ctest
ctest --output-on-failure
```

```bash
# Run Python tests
cd python
python -m pytest tests/ -v
```

# Building

## Prerequisites

### Linux
```bash
sudo apt-get update
sudo apt-get install \
    build-essential \
    cmake \
    git \
    libpython3-dev \
    python3-pip \
    python3-venv
```

### macOS
```bash
brew install cmake git python3
```

### Windows
```powershell
# Install Chocolatey or Scoop package manager
# Then:
choco install cmake git python
```

## C++ Build System

### Core CMake Configuration

The C++ build system uses modern CMake practices with the following features:

- **Cross-platform support**: Works on Linux, macOS, and Windows
- **Multiple build types**: Debug, Release, RelWithDebInfo, MinSizeRel
- **Compiler optimizations**: Platform-specific optimization flags
- **Install targets**: Support for package installation
- **Testing**: Integrated test framework

### Building the C++ Library

```bash
# Configure with specific options
cmake \
    .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DNLM_ENABLE_PROFILING=OFF \
    -DNLM_ENABLE_ASAN=OFF \
    -DNLM_ENABLE_TSAN=OFF

# Build all targets
make -j$(nproc)

# Build specific targets
make nlm_core
make nlm_agent
make nlm_world
```

### Install Targets

```bash
# Install to system directories (may require sudo)
sudo make install

# Install to custom location
make DESTDIR=/tmp/install install
```

### Configuration Options

```cmake
# Build type (Debug, Release, RelWithDebInfo, MinSizeRel)
-DCMAKE_BUILD_TYPE=Release

# Installation prefix
-DCMAKE_INSTALL_PREFIX=/usr/local

# Enable profiling
-DNLM_ENABLE_PROFILING=ON

# Enable address sanitizer
-DNLM_ENABLE_ASAN=ON

# Enable thread sanitizer
-DNLM_ENABLE_TSAN=ON

# Enable tests
-DBUILD_TESTS=ON

# Enable examples
-DEXAMPLES=ON
```

## Python Bindings

### Building Python Bindings with scikit-build-core

The Python bindings use scikit-build-core for improved build performance and dependency management.

#### Quick Build

```bash
cd python
python -m pip install -e .
```

#### Advanced Configuration

```bash
cd python
# Build with specific options
python -m pip install -e . --config-settings=cmake.define.NLM_ENABLE_PROFILING=ON
```

### Python Package Structure

```
pynlm/
├── __init__.py
├── core/
│   ├── __init__.py
│   └── brain.py
├── memory/
│   ├── __init__.py
│   └── episodic.py
├── neuromodulation/
│   ├── __init__.py
│   └── reward.py
├── sensory/
│   ├── __init__.py
│   └── vision.py
├── motor/
│   └── __init__.py
├── environment/
│   └── __init__.py
├── simulation/
│   └── __init__.py
└── cli.py
```

### Python Installation Options

```bash
# Install from source
pip install -e .

# Install to virtual environment
python -m venv venv
source venv/bin/activate
pip install -e .

# Install system-wide (requires sudo)
sudo pip install -e .
```

### Python Development

```bash
# Install development dependencies
develop-pynlm

# Run type checking
mypy pynlm/

# Run linting
ruff check pynlm/
black --check pynlm/
```

## Testing and Validation

### C++ Testing

The C++ test suite includes comprehensive unit tests, integration tests, and performance benchmarks.

#### Running C++ Tests

```bash
# Build and run all tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./nlm_test

# Or use ctest
ctest --output-on-failure
```

#### Test Categories

- **Unit Tests**: Individual component testing
- **Integration Tests**: Cross-component interactions
- **System Tests**: Complete brain simulation
- **Performance Tests**: Benchmarking and optimization validation
- **Memory Tests**: Resource management and leak detection

#### Python Testing

```bash
# Run Python tests
cd python
pytest tests/ -v --tb=short

# Run with coverage
cd python
pytest --cov=pynlm --cov-report=html

# Run specific test categories
cd python
pytest -m "performance" -v
pytest -m "integration" -v
```

### Test Configuration

The test suite is configured with pytest, supporting:

- **Parameterized testing**: Comprehensive test scenarios
- **Marker support**: Test categorization (slow, integration, performance, memory)
- **Coverage reporting**: Code coverage analysis
- **Test filtering**: Selective test execution
- **Parallel execution**: Multi-threaded test runs

## Development Tools

### Linting and Formatting

#### Code Style

The project uses Black for code formatting and Ruff for linting:

```bash
# Format code
black pynlm/ src/

# Lint code
ruff check pynlm/ src/

# Fix automatic issues
ruff check --fix pynlm/ src/
```

#### Pre-commit Hooks

```bash
# Install pre-commit hooks
pre-commit install

# Run all hooks
pre-commit run --all-files
```

### Type Checking

```bash
# Run mypy type checking
mypy pynlm/

# Generate type stubs
mypy --generate-stub pynlm/ pynlm_stubs/
```

### Code Generation

NLM includes code generation tools for creating new components and examples:

```bash
# Generate new component template
cd python
pynlm generate-component --name="new_component" --path="pynlm/core/"

# Generate example script
cd python
pynlm generate-example --type="basic" --output="examples/new_example.py"
```

### Debugging and Profiling

#### Debug Tools

```bash
# Build with debug symbols and sanitizers
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ASAN=ON

# Use GDB for debugging
gdb --args ./nlm --simulate

# Use Visual Studio Code with C++ extension
code .
```

#### Performance Profiling

```bash
# Build with profiling enabled
cmake .. -DNLM_ENABLE_PROFILING=ON

# Run performance benchmark
./nlm_phase6_demo --benchmark

# Profile Python bindings
python -m memory_profiler pynlm/core/brain.py
python -m cProfile -o profile.stats pynlm/core/brain.py
```

### Development Environment Setup

#### Scripts

The project includes development environment setup scripts:

```bash
# Setup development environment
develop.sh

# Setup Python development environment
develop-pynlm

# Setup CI/CD pipeline
cd python
ci/setup-ci.sh
```

#### Environment Variables

```bash
# NLM environment variables
export NLM_LOG_LEVEL=INFO
export NLM_DATA_DIR=~/.nlm/data
export NLM_CHECKPOINT_DIR=~/.nlm/checkpoints
export NLM_PERF_THRESHOLD=1000.0

# Python environment variables
export PYNLB_BINDINGS_PATH=/usr/local/lib/python3.8/site-packages/pynlm
export PYTHONPATH=$PYTHONPATH:$(pwd)/python
```

## Continuous Integration

### CI Configuration

The project supports multiple CI platforms:

#### GitHub Actions

```yaml
# .github/workflows/ci.yml
name: NLM CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        python-version: [3.8, 3.9, "3.10", "3.11", "3.12"]

    steps:
    - uses: actions/checkout@v3
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}

    - name: Install dependencies
      run: |
        pip install -e .[dev]

    - name: Run C++ tests
      run: |
        mkdir build
        cd build
        cmake ..
        make -j$(nproc)
        ./nlm_test

    - name: Run Python tests
      run: |
        cd python
        pytest tests/ -v

    - name: Run linting and type checking
      run: |
        black --check pynlm/ src/
        ruff check pynlm/ src/
        mypy pynlm/
```

#### GitLab CI

```yaml
# .gitlab-ci.yml
development:
  stage: test
  script:
    - cmake setup
    - make build
    - ctest --output-on-failure
    - python -m pytest tests/
  only:
    - main
    - develop
```

### Performance Testing

```bash
# Performance benchmarking script
#!/bin/bash

cd python

# Install benchmarking tools
pip install pytest-benchmark

# Run performance tests
pytest --benchmark-only --benchmark-save=benchmarks.json

# Generate performance report
pytest --benchmark-compare
```

## Deployment and Packaging

### Packaging

#### Python Package

```bash
# Build Python package
dev-pynlm build

# Create source distribution
python setup.py sdist

# Create wheel distribution
python setup.py bdist_wheel

# Upload to PyPI
python -m twine upload dist/*
```

#### C++ Library

```bash
# Build packaging scripts
ci/build-package.sh

# Create tarball
make DESTDIR=/tmp/package install
```

### Deployment

#### System Deployment

```bash
# Install system service
sudo cp nlm.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable nlm
sudo systemctl start nlm
```

#### Python Deployment

```bash
# Install Python package globally
pip install pynlm

# Create user configuration
mkdir -p ~/.config/pynlm
sudo cp config/pynlm.conf /etc/pynlm/

# Set up environment
source ~/.bashrc
export PYTHONPATH=$PYTHONPATH:/usr/local/lib/python3.8/site-packages
```

## Migration Guide

### From CMake 3.16 to CMake 3.16+

- Updated minimum CMake version to 3.16
- Added support for CMake 3.19+ features
- Improved target-based dependencies

### From setup.py to pyproject.toml

- Migrated to scikit-build-core for better Python build integration
- Added comprehensive pyproject.toml configuration
- Improved dependency management and packaging

## Troubleshooting

### Common Issues

#### Build Errors

**Symptom**: Compiler errors during build
**Solution**: Update compiler and build tools
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
```

**Symptom**: Python bindings fail to build
**Solution**: Install pybind11 and dependencies
```bash
pip install pybind11 numpy
```

**Symptom**: Test failures
**Solution**: Check environment variables and dependencies
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

#### Installation Problems

**Symptom**: Cannot install system-wide
**Solution**: Use virtual environment or install with sudo
```bash
python -m venv venv
source venv/bin/activate
pip install -e .
```

**Symptom**: Python module not found
**Solution**: Check PYTHONPATH and install path
```bash
export PYTHONPATH=$PYTHONPATH:$(pwd)/python
pip install -e .
```

### Getting Help

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Slack/Discord**: Contact project maintainers for development support

## Changelog

### Latest Changes

- Enhanced CMake configuration with cross-platform support
- Improved Python bindings with scikit-build-core
- Added comprehensive testing infrastructure
- Enhanced documentation and development tools
- Added linting, formatting, and type checking support

### Previous Versions

See git history for previous release notes and migration guides.

## License

MIT License. See LICENSE file for details.

---

*For more information or questions about development, please refer to the NLM documentation website or contact the project maintainers.*