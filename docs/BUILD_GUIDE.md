# NLM Build Guide

## Overview

This document provides comprehensive build instructions for the NLM (Neural Learning Machine) project, covering development setup, building from source, installation, and configuration.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Building from Source](#building-from-source)
3. [Build Configuration Options](#build-configuration-options)
4. [Python Bindings](#python-bindings)
5. [Installation](#installation)
6. [Testing](#testing)
7. [Performance Tuning](#performance-tuning)
8. [Troubleshooting](#troubleshooting)
9. [Docker Development](#docker-development)
10. [Conda Development](#conda-development)
11. [CI/CD Configuration](#cicd-configuration)

## Prerequisites

### System Requirements

- **Linux/macOS**: GCC 10+, Clang 12+, or MSVC 2019+
- **Windows**: Visual Studio 2019+ with C++ support
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make/ninja, pkg-config

### Platform-Specific Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential cmake libpython3-dev python3-pip \
                     libboost-all-dev libhdf5-dev libeigen3-dev \
                     libgtk-3-dev libgl1-mesa-dev libglu1-mesa-dev
```

#### Fedora/CentOS/RHEL
```bash
sudo dnf install -y gcc gcc-c++ cmake python3-devel python3-pip \
                     boost-devel hdf5-devel eigen3-devel \
                     gtk3-devel mesa-libgl-devel libglvnd-devel
```

#### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Optional: Install Homebrew and dependencies
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake python3 boost hdf5 eigen
```

#### Windows
- Install Visual Studio 2019+ with C++ desktop workload
- Install Windows SDK
- Use Developer Command Prompt for VS

## Building from Source

### Standard Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j$(nproc)
```

### Development Build

For development with debugging and extra features:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DBUILD_TESTS=ON \
         -DBUILD_PYTHON_BINDINGS=ON \
         -DNLM_COVERAGE=ON
make -j$(nproc)
```

### Multi-Configuration Generators

For better control over build types, use CMake's multi-configuration generators:

#### With Makefile (default)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --build . --config Debug
```

#### With Visual Studio (Windows)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cmake --build . --config Debug
```

## Build Configuration Options

The project supports numerous build-time options. Use `-DOPTION=VALUE` when configuring CMake.

### Core Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type: Release, Debug, Coverage, RelWithDebInfo |
| `BUILD_TESTS` | ON | Build test executables |
| `BUILD_PYTHON_BINDINGS` | ON | Build Python bindings |
| `BUILD_SHARED_LIBS` | OFF | Build shared libraries instead of static |
| `NLM_ENABLE_VISUALIZATION` | ON | Enable visualization support |
| `NLM_COVERAGE` | OFF | Enable code coverage reporting |
| `NLM_OPTIMIZE_FOR_PERFORMANCE` | ON | Apply performance optimizations |

### Python Binding Options

The Python bindings are built using scikit-build-core. Use these options to control Python bindings:

```bash
cmake .. -DBUILD_PYTHON_BINDINGS=ON \
         -DPYTHON_EXECUTABLE=/path/to/python
```

### Advanced Options

- `CMAKE_INSTALL_PREFIX`: Installation directory
- `CMAKE_PREFIX_PATH`: Paths to search for dependencies
- `CMAKE_CXX_STANDARD`: C++ standard (20 is minimum)
- `CMAKE_CXX_COMPILER`: Specific compiler to use

### Example Build Script

Create a build script `build.sh`:

```bash
#!/bin/bash

# Parse command line arguments
BUILD_TYPE="Release"
BUILD_TESTS=ON
BUILD_PYTHON=ON

done

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Configuring NLM build...${NC}"

cmake .. \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DBUILD_TESTS=${BUILD_TESTS} \
    -DBUILD_PYTHON_BINDINGS=${BUILD_PYTHON} \
    -DCMAKE_INSTALL_PREFIX=/usr/local

echo -e "${GREEN}Building NLM...${NC}"
make -j$(nproc)

echo -e "${YELLOW}Build complete!${NC}"
echo "To run tests: ctest --output-on-failure"
echo "To install: sudo make install"
```

## Python Bindings

### Installation from Source

#### Using pip (Recommended)

```bash
# Install with pip (builds bindings automatically)
pip install .

# Install in development mode
pip install -e .
```

#### Using Build System

```bash
# Install Python build dependencies
pip install scikit-build-core pybind11 pytest numpy build

# Build and install
pip install .

# Or build only (without installing)
python -m build
```

### Building Python Package

The Python bindings are built using `pyproject.toml` and `scikit-build-core`:

```bash
# Create build directory and configure
mkdir build
cd build
cmake .. -DBUILD_PYTHON_BINDINGS=ON

# Build Python extensions
cmake --build . --target pynlm
```

### Testing Python Bindings

```bash
# Run Python tests
python -m pytest tests/ -v

# Run with coverage
python -m pytest --cov=pynlm tests/

# Run specific test module
python -m pytest tests/test_python_bindings.py -v
```

### Verification

Verify the Python installation:

```python
import pynlm

print(f"NLM version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

## Installation

### System Installation

```bash
# Configure build with installation prefix
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=/usr/local

# Build and install
make -j4
make install
```

### Installation to Custom Prefix

```bash
# Install to custom directory
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=$HOME/local

make -j4
make install

# Add to PATH and LD_LIBRARY_PATH
export PATH=$HOME/local/bin:$PATH
export LD_LIBRARY_PATH=$HOME/local/lib:$LD_LIBRARY_PATH
```

### Python Installation

```bash
# Install Python package globally
pip install .

# Or install to user directory
pip install --user .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Package Manager Installation

For systems with package managers:

#### Ubuntu/Debian (with dependencies)
```bash
sudo apt install -y build-essential cmake python3-dev python3-pip
pip install scikit-build-core pybind11 pytest numpy

# Build and install
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
sudo make install
```

#### Using Conan (if available)
```bash
# Create conanfile.py
# Install conan if not present
pip install conan

# Create conanfile.py in project root
# Configure and install with conan
conan create . nlm/0.1.0
```

## Testing

### C++ Tests

Build and run C++ tests:

```bash
# Build with tests enabled
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
cmake --build . --target nlm_test

# Run tests
./nlm_test

# Or use CTest
ctest --output-on-failure
```

### Python Tests

```bash
# Install pytest if not present
pip install pytest pytest-cov

# Run all Python tests
pytest tests/ -v

# Run with coverage report
pytest --cov=pynlm --cov-report=html --cov-report=term tests/

# Run specific test suite
pytest tests/test_brain.py tests/test_memory.py -v
```

### Integration Tests

Run integration tests that test the complete system:

```bash
# Build with integration tests
cmake .. -DBUILD_TESTS=ON -DBUILD_PYTHON_BINDINGS=ON
cmake --build . --target nlm_phase6_demo

# Run integration demo
./nlm_phase6_demo
```

### Test Coverage

Generate code coverage reports:

```bash
# Configure with coverage
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Coverage -DNLM_COVERAGE=ON
cmake --build .
./nlm_test

# Generate reports
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage-report
```

## Performance Tuning

### Compiler Optimizations

For maximum performance, use these optimization flags:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_OPTIMIZE_FOR_PERFORMANCE=ON \
         -DBUILD_SHARED_LIBS=ON \
         -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"
```

### Runtime Optimization

Environment variables for runtime tuning:

- `NLM_MAX_THREADS`: Maximum number of threads for parallel processing
- `NLM_SIMULATION_STEPS`: Maximum number of simulation steps before auto-save
- `NLM_PLACEMENT_THRESHOLD`: Memory allocation threshold
- `NLM_SPIKE_PRECISION`: Precision for spike timing calculations

Example for high-performance simulation:

```bash
export NLM_MAX_THREADS=8
export NLM_SIMULATION_STEPS=1000000
export NLM_PLACEMENT_THRESHOLD=0.01
export NLM_SPIKE_PRECISION=double
```

### Memory Management

- Use `--enable-memory-profiling` for debugging memory issues
- Monitor memory usage with `valgrind` or `massif`
- Use efficient data structures for large-scale simulations

## Troubleshooting

### Common Build Issues

**"CMake cannot find Python"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"Compilation errors"**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Install build dependencies: `build-essential`, `cmake`, `libpython3-dev`

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Use debug build: `cmake .. -DCMAKE_BUILD_TYPE=Debug`

### Platform-Specific Issues

**Windows**
```bash
# Using Visual Studio Generator
cmake .. -G "Visual Studio 16 2019" -A x64

# Build with MSBuild
msbuild ALL_BUILD.vcxproj

# Install with MSBuild
msbuild INSTALL.vcxproj
```

**macOS**
```bash
# Ensure Xcode Command Line Tools are installed
xcode-select --install

# Build with proper permissions
make install

# Fix DYLD library path
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

**Linux**
```bash
# Install required packages (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake libpython3-dev python3-pip python3-dev

# Install required packages (Fedora/CentOS)
sudo dnf install gcc gcc-c++ cmake python3-devel python3-pip

# Fix library path
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

### Python Binding Issues

**"pybind11 not found"**
```bash
pip install pybind11
```

**"scikit-build-core not found"**
```bash
pip install scikit-build-core
```

**"Python version mismatch"**
```bash
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

### Dependency Issues

If dependencies are missing:

1. **Check for system packages**
   - Ubuntu: `sudo apt list --installed | grep [package]`
   - macOS: `brew list | grep [package]`
   - Windows: `pip list` or `conda list`

2. **Install missing dependencies**
   - Ubuntu: `sudo apt install [package]`
   - macOS: `brew install [package]`
   - Using package manager

## Docker Development

### Using Docker

#### Build Docker Image

Create `Dockerfile`:

```dockerfile
FROM ubuntu:22.04

LABEL maintainer="NLM Team"
LABEL description="Development environment for NLM project"

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    libboost-all-dev \
    libhdf5-dev \
    libeigen3-dev

# Install Python dependencies
RUN pip3 install --upgrade pip
RUN pip3 install scikit-build-core pybind11 pytest numpy

# Set working directory
WORKDIR /workspace/nlm

# Copy source code
COPY . /workspace/nlm

# Create build directory
RUN mkdir -p build
WORKDIR /workspace/nlm/build

# Configure and build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j$(nproc)

# Expose ports if needed
EXPOSE 8080

# Default command
CMD ["make", "install"]
```

#### Build and Run

```bash
# Build Docker image
docker build -t nlm-dev .

# Run container
docker run -it --rm -v $(pwd):/workspace/nlm nlm-dev

# Or run interactive shell
docker run -it --rm -v $(pwd):/workspace/nlm nlm-dev bash

# Build and run tests inside container
docker run --rm -v $(pwd):/workspace/nlm nlm-dev \
    bash -c "cd /workspace/nlm/build && cmake .. && make -j4 && ctest --output-on-failure"
```

### Docker Compose

Create `docker-compose.yml`:

```yaml
version: '3.8'

services:
  nlm:
    build: .
    volumes:
      - .:/workspace/nlm
    ports:
      - "8888:8888"
    command: >
      bash -c "
      cd /workspace/nlm/build
      cmake ..
      make -j4
      ./nlm
      "
```

Run with:

```bash
docker-compose up --build
```

## Conda Development

### Create Conda Environment

```bash
# Create conda environment
conda create -n nlm python=3.11
conda activate nlm

# Install conda-forge packages
conda install -c conda-forge cmake boost-python hdf5 eigen

# Install pip packages
pip install scikit-build-core pybind11 pytest numpy
```

### Build with Conda

```bash
# Create build directory
cd nlm
mkdir build && cd build

# Configure with conda Python
cmake .. -DPYTHON_EXECUTABLE=$(which python)

# Build
cmake --build . --parallel $(nproc)
```

### Conda Environment File

Create `environment.yml`:

```yaml
name: nlm-dev
channels:
  - conda-forge
  - defaults
dependencies:
  - python=3.11
  - cmake>=3.16
  - boost-cpp
  - hdf5
  - eigen
  - scikit-build-core
  - pybind11
  - pytest
  - numpy
  - pip
  - pip:
    - build
```

Install with:

```bash
conda env create -f environment.yml
conda activate nlm-dev
```

## CI/CD Configuration

### GitHub Actions

The project includes GitHub Actions workflows for CI/CD:

- **`.github/workflows/build.yml`**: Build and test the C++ core
- **`.github/workflows/python.yml`**: Test Python bindings
- **`.github/workflows/coverage.yml`**: Generate coverage reports

### Continuous Integration

#### Build Matrix

```yaml
test_matrix:
  os: [ubuntu-latest, macos-latest, windows-latest]
  build_type: [Release, Debug]
  python_version: ['3.8', '3.9', '3.10', '3.11', '3.12']
```

#### Code Quality Checks

```yaml
test_code_quality:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v3
    - name: Check code formatting
      run: clang-format --style=LLVM --dry-run src/**/*.cpp src/**/*.hpp
    - name: Check for trailing whitespace
      run: git diff --check HEAD
    - name: Run static analysis
      run: cppcheck src/ --enable=all
```

#### Documentation Build

```yaml
test_documentation:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v3
    - name: Build documentation
      run: doxygen Doxyfile
    - name: Check documentation links
      run: markdownlint docs/
```

### Pre-commit Hooks

Set up pre-commit hooks for code quality:

```bash
# Install pre-commit
curl https://pre-commit.com/install.py | python
pre-commit install

# Install git hooks
pre-commit install-hooks
```

Create `.pre-commit-config.yaml`:

```yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-yaml
      - id: check-added-large-files

  - repo: https://github.com/llvm/llvm-project
    rev: '15.0.0'
    hooks:
      - id: clang-format

  - repo: https://github.com/cppcheck/cppcheck
    rev: '2.10'
    hooks:
      - id: cppcheck
        args: [--enable=all]
```

## Advanced Topics

### Building with Dependencies

#### Using External Libraries

```bash
# Find external packages
cmake .. \
    -DCMAKE_PREFIX_PATH=/path/to/deps \
    -DBOOST_ROOT=/path/to/boost \
    -DHDF5_ROOT=/path/to/hdf5

# Or use package managers
conan install . --build=missing
```

#### Cross-Compilation

For cross-compilation:

```bash
cmake .. \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
```

### Performance Profiling

#### gprof

```bash
# Build with profiling support
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run program to generate profile
./nlm --profiler-file=gmon.out

# Generate profile report
gprof ./nlm gmon.out > profile.txt
```

#### Valgrind

```bash
# Build with debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with valgrind
valgrind --tool=memcheck --log-file=valgrind.log ./nlm_test

# Performance profiling
valgrind --tool=callgrind ./nlm_test
callgrind_control --display-block-lines=yes
```

### Memory Profiling

```bash
# Build with AddressSanitizer
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address"
make -j4

# Run with memory leak detection
./nlm_test
```

## Migration Guide

### From Previous Versions

**From version 0.0.x to 0.1.0:**

1. Update CMakeLists.txt build options:
   - New build system with multiple configuration options
   - Changed library naming (nlm_core, nlm_agent, nlm_world)

2. Update build scripts:
   - New build.sh script with comprehensive options
   - Support for Docker and Conda environments

3. Update Python bindings:
   - Using scikit-build-core instead of setuptools
   - New pyproject.toml configuration

## License

MIT

## Authors

NLM Research Team

## Acknowledgments

- TensorFlow, PyTorch, and other ML frameworks for inspiration
- Python community for excellent ecosystem
- CMake community for build system
- Contributors for testing and development