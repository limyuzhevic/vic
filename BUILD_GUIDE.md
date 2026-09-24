# NLM Build System Documentation

## Overview

This document provides comprehensive documentation for the NLM (熙然) build system. The NLM project is an experimental artificial developmental brain implemented in C++ with Python bindings, requiring a sophisticated build system to support its complex architecture.

## Table of Contents

1. [Build System Overview](#build-system-overview)
2. [Build Scripts and Automation](#build-scripts-and-automation)
3. [CI/CD Configuration](#cicd-configuration)
4. [Development Environment Setup](#development-environment-setup)
5. [Build Optimization and Caching](#build-optimization-and-caching)
6. [Quality Assurance](#quality-assurance)
7. [Testing Integration](#testing-integration)
8. [Performance Testing](#performance-testing)
9. [Documentation Generation](#documentation-generation)
10. [Release Management](#release-management)
11. [Troubleshooting](#troubleshooting)
12. [Configuration Reference](#configuration-reference)
13. [API Documentation](#api-documentation)

## Build System Overview

### Project Structure

The NLM project has a modular architecture designed to support complex neural simulation:

```
NLM/
├── CMakeLists.txt              # C++ build configuration
├── pyproject.toml              # Python package configuration
├── Makefile                    # Makefile targets
├── build/                      # Build artifacts
├── src/                        # Source code
│   ├── core/                  # Core utilities
│   ├── brain/                 # Neural components
│   ├── memory/                # Memory systems
│   ├── ...                    # Other modules
├── python/                     # Python bindings
├── docs/                       # Documentation
├── tests/                      # Tests
├── build_scripts/              # Build scripts
├── .github/workflows/          # GitHub Actions
└── .gitlab-ci.yml              # GitLab CI
```

### Build Requirements

- **CMake**: Version 3.16 or higher
- **C++ Compiler**: C++20 compatible (GCC 9+, Clang 10+, MSVC 19.0+)
- **Python**: Version 3.8 or higher
- **scikit-build-core**: Version 0.5.0 or higher
- **pybind11**: Version 2.11.0 or higher

### Build Options

| Option | Description | Default | Values |
|--------|-------------|---------|--------|
| `CMAKE_BUILD_TYPE` | Build configuration | `Release` | `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel` |
| `CMAKE_CXX_STANDARD` | C++ standard version | `20` | `11`, `14`, `17`, `20`, `23` |
| `ENABLE_MPI` | Enable MPI support | `OFF` | `ON`, `OFF` |
| `ENABLE_CUDA` | Enable CUDA support | `OFF` | `ON`, `OFF` |
| `ENABLE_SIMD` | Enable SIMD optimizations | `OFF` | `ON`, `OFF` |
| `BUILD_SHARED_LIBS` | Build shared libraries | `OFF` | `ON`, `OFF` |
| `BUILD_TESTS` | Build test executables | `ON` | `ON`, `OFF` |
| `BUILD_EXAMPLES` | Build example programs | `ON` | `ON`, `OFF` |
| `NLM_PYTHON_BINDINGS` | Build Python bindings | `ON` | `ON`, `OFF` |
| `ENABLE_VALIDATION` | Enable runtime validation | `OFF` | `ON`, `OFF` |

## Build Scripts and Automation

### Core Build Scripts

#### 1. build.sh

A comprehensive build script that provides:

- **Configurable build options** via command-line arguments
- **Parallel build support** with optimal job detection
- **Timeout protection** for long-running builds
- **Test execution** after successful build
- **Installation support** with custom prefix
- **Code style checks** and validation

**Key Features:**
- Supports multiple build configurations (Debug, Release, etc.)
- Feature flags for MPI, CUDA, SIMD, validation
- Python virtual environment management
- Automatic dependency installation
- Coverage report generation
- Documentation building

**Usage Examples:**

```bash
# Basic release build
./build_scripts/build.sh

# Debug build with all features
./build_scripts/build.sh --debug --features all

# Build with installation
./build_scripts/build.sh --install-prefix /usr/local

# Build with coverage and documentation
./build_scripts/build.sh --coverage --docs

# Build with specific number of jobs
./build_scripts/build.sh --jobs 8
```

#### 2. test.sh

Comprehensive testing script that includes:

- **C++ unit tests** via CTest
- **Python unit tests** via pytest
- **Code coverage** analysis
- **Benchmark execution**
- **Performance testing**
- **Configuration validation**

**Key Features:**
- Parallel test execution
- Configurable timeout
- Coverage threshold enforcement
- HTML and XML report generation
- Support for different test directories
- Verbose output options

**Usage Examples:**

```bash
# Run all tests with defaults
./build_scripts/test.sh

# Run tests with verbose output and coverage
./build_scripts/test.sh --verbose --coverage

# Skip C++ tests
./build_scripts/test.sh --no-cpp

# Run specific Python tests
./build_scripts/test.sh --python-tests tests/unit
```

#### 3. benchmark.sh

Performance benchmarking tool that includes:

- **Comprehensive benchmark suite** for NLM
- **Configurable iterations** and warmup
- **Multiple output formats** (JSON, CSV, HTML)
- **Memory and CPU profiling** support
- **Detailed result analysis**
- **HTML report generation**

**Key Features:**
- Supports benchmark filtering by name
- Configurable output formats
- Performance comparison capabilities
- Memory and CPU profiling
- Automatic report generation

**Usage Examples:**

```bash
# Run default benchmarks
./build_scripts/benchmark.sh

# Run with memory profiling
./build_scripts/benchmark.sh --memory-profile

# Run with specific iterations
./build_scripts/benchmark.sh --iterations 100

# Generate HTML report
./build_scripts/benchmark.sh --report-format html
```

#### 4. setup_env.sh

Development environment setup script that includes:

- **Python virtual environment** creation
- **Development dependencies** installation
- **Git hooks** configuration
- **Workspace configuration**
- **IDE support** setup

**Key Features:**
- Automatic virtual environment creation
- Pre-commit and commit-msg hooks
- GitLab CI configuration
- Comprehensive .gitignore generation
- Environment file setup

**Usage Examples:**

```bash
# Setup basic development environment
./build_scripts/setup_env.sh

# Use custom virtual environment name
./build_scripts/setup_env.sh --venv-name myenv

# Skip dependencies and tools
./build_scripts/setup_env.sh --no-deps --no-tools
```

#### 5. release.sh

Release management tool that includes:

- **Version management** and bumping
- **Distribution package** creation
- **GitHub/GitLab releases** integration
- **Docker image** building and deployment
- **Checksum generation**
- **Documentation generation**

**Key Features:**
- Semantic version support
- Multi-platform distribution
- Automated release notes generation
- Docker and Python package support
- Comprehensive artifact management

**Usage Examples:**

```bash
# Create release from current version
./build_scripts/release.sh

# Bump minor version and create release
./build_scripts/release.sh --bump-level minor

# Create GitHub release with token
./build_scripts/release.sh --github-token $GITHUB_TOKEN
```

### Makefile

A comprehensive Makefile that provides:

- **Standard make targets** for common operations
- **Quick start** commands for new users
- **Project validation** and requirement checking
- **Integration** with build scripts
- **Documentation generation**

**Key Targets:**

| Target | Description |
|--------|-------------|
| `all` | Build the project with Python bindings (default) |
| `setup` | Set up development environment (basic) |
| `setup-full` | Set up development environment (comprehensive) |
| `build` | Build the C++ library and executables |
| `build-script` | Build using comprehensive build script |
| `test` | Run all tests (C++ and Python) |
| `test-script` | Run tests using comprehensive test script |
| `clean` | Clean build artifacts |
| `coverage` | Generate code coverage report |
| `docs` | Build documentation |
| `release` | Create release distribution |
| `docker` | Build Docker image |
| `python-benchmark` | Run Python benchmarks |
| `validate` | Validate project structure |
| `check-requirements` | Check build requirements |
| `quick-start` | Show quick start guide |

## CI/CD Configuration

### GitHub Actions

**File:** `.github/workflows/ci.yml`

A comprehensive GitHub Actions workflow that includes:

- **Multi-platform builds** (Linux, macOS, Windows)
- **Parallel job execution** for faster CI
- **Docker image** building and caching
- **Code quality checks** and static analysis
- **Test execution** with coverage reporting
- **Documentation generation**
- **Release automation**

**Key Features:**

- **Matrix strategy** for multiple platforms
- **Docker buildx** integration for multi-architecture support
- **Artifact caching** to speed up subsequent runs
- **Code quality gates** with linting and security scanning
- **Automated documentation** deployment
- **Nightly build** schedule for continuous integration

### GitLab CI

**File:** `.gitlab-ci.yml`

A comprehensive GitLab CI configuration that includes:

- **Multi-stage pipeline** (dependencies, build, test, quality, docs, release)
- **System-specific configurations** for Ubuntu, Alpine, macOS, and Windows
- **Artifact caching** for faster pipeline execution
- **Quality gates** with coverage and linting requirements
- **Security scanning** and dependency checking
- **Manual deployment** for production

**Key Features:**

- **Stage-based pipeline** for better organization
- **System optimization** for each platform
- **Comprehensive testing** including C++ and Python tests
- **Security-first approach** with automated scanning
- **Manual approval** for production deployments

### Jenkins Pipeline

**File:** `Jenkinsfile`

A declarative Jenkins pipeline that includes:

- **Modern Jenkins pipeline** syntax
- **Multi-stage process** for complex workflows
- **Docker integration** for isolated builds
- **Artifact management** and publication
- **Automated notifications** and deployment
- **Quality gates** with failure handling

**Key Features:**

- **Declarative pipeline** for better readability
- **Modern Jenkins features** (parallel stages, dynamic agents)
- **Comprehensive error handling** and recovery
- **Artifact publishing** to multiple repositories
- **Automated notifications** for team members

## Development Environment Setup

### Prerequisites

#### System Requirements

- **Operating System**: Linux, macOS, Windows (or WSL)
- **Memory**: 8 GB minimum, 16+ GB recommended
- **Storage**: 2 GB free space (50+ GB for full development)
- **Network**: For downloading dependencies and Docker images

#### Software Dependencies

##### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    libpython3-dev \
    pkg-config \
    libopenmpi-dev \
    liblapack-dev \
    libatlas-base-dev \
    valgrind \
    gprof \
    clang-tidy \
    clang-format \
    doxygen
```

##### macOS

```bash
# Install Homebrew if not available
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git python3 openmpi lapack doxygen
```

##### Windows

```powershell
# Install from Microsoft Store or chocolatey
# Install Git for Windows
# Install Python from python.org
# Install Visual Studio with C++ workload
# Install CMake from cmake.org
```

### Environment Setup

#### 1. Virtual Environment Setup

```bash
# Create and activate virtual environment
python3 -m venv nlm-venv
source nlm-venv/bin/activate

# Upgrade pip and setuptools
pip install --no-cache-dir --upgrade pip setuptools wheel

# Install core dependencies
pip install --no-cache-dir scikit-build-core>=0.5.0 pybind11>=2.11.0
```

#### 2. Project Installation

```bash
# Install the project in development mode
source venv/bin/activate
pip install --no-cache-dir -e .

# Install development dependencies
pip install --no-cache-dir -e .[dev]
```

#### 3. IDE Configuration

##### Visual Studio Code

Create `.vscode/settings.json`:

```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureOnOpen": false,
    "cmake.generator": "Ninja",
    "cmake.cBuildType": "Release",
    "files.exclude": {
        "**/.git": true,
        "**/.github": true,
        "**/build": true,
        "**/dist": true,
        "**/__pycache__": true,
        "**/*.pyc": true,
        "**/*.pyo": true
    },
    "python.pythonPath": "${workspaceFolder}/venv/bin/python",
    "python.formatting.provider": "black",
    "python.linting.enabled": true,
    "python.linting.flake8Enabled": true,
    "python.linting.isortEnabled": true,
    "python.linting.mypyEnabled": true
}
```

Create `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build NLM",
            "type": "shell",
            "command": "bash",
            "args": [
                "build_scripts/build.sh"
            ],
            "problemMatcher": "$cmake"
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "bash",
            "args": [
                "build_scripts/test.sh"
            ],
            "dependsOn": "Build NLM",
            "problemMatcher": []
        }
    ]
}
```

##### CLion

Create `cmake-toolchain.cmake` for cross-compilation:

```cmake
# Toolchain file for cross-compilation

# Cross-compilation settings
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Compiler toolchain files
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)

# Sysroot if needed
set(CMAKE_SYSROOT /path/to/sysroot)

# Additional system libraries
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

#### 4. Docker Development Environment

**Dockerfile**:

```dockerfile
FROM ubuntu:22.04

LABEL maintainer="NLM Team"
LABEL description="NLM Development Environment"

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    libpython3-dev \
    wget \
    curl \
    vim \
    nano

# Install Python dependencies
RUN pip3 install --no-cache-dir \
    scikit-build-core \
    pybind11 \
    numpy \
    pytest \
    pytest-cov \
    pytest-html \
    matplotlib \
    seaborn

# Create workspace directory
WORKDIR /workspace

# Clone NLM repository
RUN git clone https://github.com/your-org/nlm.git .

# Set up build directory
RUN mkdir -p build

# Configure and build
WORKDIR /workspace/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Clean up build artifacts for final image
WORKDIR /workspace
RUN rm -rf build

# Create non-root user
RUN useradd -m -s /bin/bash developer
USER developer

# Set up development environment
WORKDIR /workspace/nlm

# Install the Python bindings in development mode
RUN pip install -e .

# Default command
CMD ["bash"]
```

**Docker Compose**:

```yaml
version: '3.8'

services:
  nlm-dev:
    build: .
    volumes:
      - .:/workspace/nlm
      - ./build:/workspace/nlm/build
      - ~/.cache/pip:/root/.cache/pip
    ports:
      - "8888:8888"  # Jupyter notebook
    environment:
      - DISPLAY=${DISPLAY}
      - QT_X11_NO_MITSHM=1
    command: bash

  nlm-test:
    build: .
    volumes:
      - .:/workspace/nlm
      - ./build:/workspace/nlm/build
    command: build_scripts/test.sh

  nlm-build:
    build: .
    volumes:
      - .:/workspace/nlm
      - ./build:/workspace/nlm/build
    command: build_scripts/build.sh
```

## Build Optimization and Caching

### Parallel Build Optimization

#### 1. CMake Parallel Build

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -j$(nproc)
cmake --build . --parallel $(nproc)
```

#### 2. Environment Variables for Parallelism

```bash
export OMP_NUM_THREADS=$(nproc)
export MKL_NUM_THREADS=$(nproc)
export OPENBLAS_NUM_THREADS=$(nproc)
export CUDA_VISIBLE_DEVICES=0,1,2,3

# Build with optimal parallelization
make -j$(nproc) -l$(nproc)
```

### Incremental Build Support

#### 1. CMake Build Caching

CMake supports incremental builds through:

- **Generated files**: Source code generation is cached
- **Object files**: Per-source object file caching
- **Dependency tracking**: Automatic header dependency tracking
- **Compiler cache**: Support for ccache

#### 2. Using ccache

```bash
# Install ccache
sudo apt install -y ccache

# Configure CMake with ccache
cmake .. -DCMAKE_CXX_COMPILER=ccache-g++

# Use ccache for better build performance
make -j$(nproc)
```

#### 3. Build Caching Strategies

##### Local Build Cache

```bash
# Set environment variables for build caching
export CMAKE_BUILD_PARALLEL_LEVEL=$(nproc)
export CCACHE_DIR=~/.cache/ccache
export CCACHE_COMPRESS=1

# Build with caching enabled
make -j$(nproc)
```

##### Remote Build Cache (CI/CD)

GitHub Actions build cache can be used:

```yaml
- name: Cache CMake build
  uses: actions/cache@v3
  with:
    path: build/
    key: ${{ runner.os }}-build-${{ hashFiles('**/CMakeLists.txt', '**/CMakeLists.txt') }}
    restore-keys: |
      ${{ runner.os }}-build-
```

### Space-Efficient Build Strategies

#### 1. Stripped Builds

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-s -O3"
make -j$(nproc)

# Strip debug symbols from binary
strip nlm
```

#### 2. Out-of-Tree Builds

```bash
# Build outside source tree
mkdir -p ~/nlm-build
cd ~/nlm-build
cmake /path/to/nlm/source
make -j$(nproc)
```

#### 3. Minimal Build Configuration

For minimal builds, disable unnecessary features:

```cmake
# In CMakeLists.txt
option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_TESTS "Build test executables" OFF)
option(BUILD_DOCUMENTATION "Build documentation" OFF)
```

## Quality Assurance

### Automated Quality Checks

#### 1. Code Style Enforcement

```bash
# Install clang-format and clang-tidy
sudo apt install -y clang-format-12 clang-tidy-12

# Run clang-format
find src -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format-12 -i

# Run clang-tidy
find src -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-tidy-12 -p build/
```

#### 2. Pre-commit Hooks

Create `.pre-commit-config.yaml`:

```yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: trailing-whitespace
      - id: end-of-file-fixer
      - id: check-yaml
      - id: check-json
      - id: check-added-large-files

  - repo: https://github.com/psf/black
    rev: 23.3.0
    hooks:
      - id: black
        types: [python]

  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort
        types: [python]

  - repo: https://github.com/clang-dictionary/clang-format
    rev: 13.0.0
    hooks:
      - id: clang-format
        types: [c++]
```

#### 3. Static Analysis Integration

##### SonarQube Integration

```xml
<!-- pom.xml for Java projects or CMake configuration for C++ projects -->
<!-- For CMake, add sonar target -->
find_package(SonarQube REQUIRED)

sonarqube_scan(
    PROJECT_KEY "nlm"
    PROJECT_NAME "NLM - Experimental Artificial Developmental Brain"
    PROJECT_VERSION "${PROJECT_VERSION}"
    ANALYSIS_MODE CI
    SCANNER_HOME "${SONAR_SCANNER_HOME}"
)
```

##### Cppcheck Integration

```bash
# Install cppcheck
sudo apt install -y cppcheck

# Run cppcheck on source code
cppcheck --enable=all --insecure --warnings-as-errors src/

# Generate HTML report
cppcheck --enable=all --insecure --output-file=report.html src/
```

### Security Scanning

#### 1. AddressSanitizer (ASAN)

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
make -j$(nproc)

# Run tests with AddressSanitizer
ctest --output-on-failure
```

#### 2. Static Analysis Security Scanner

```bash
# Install and run static analysis security scanner
pip install semgrep
semgrep --config=auto src/
```

## Testing Integration

### Automated Test Execution

#### 1. C++ Tests

```bash
# Run all C++ tests
mkdir -p build
cd build
ctest --output-on-failure --timeout 300

# Run specific test suites
ctest -R "test_brain"
ctest -R "test_memory"
ctest -R "test_plasticity"

# Run tests with verbose output
ctest -V

# Run tests with custom timeout
ctest --timeout 300
```

#### 2. Python Tests

```bash
# Run Python tests
pytest tests/

# Run Python tests with coverage
pytest --cov=pynlm --cov-report=html tests/

# Run specific Python tests
pytest tests/test_neuron.py
pytest tests/test_memory.py

# Run tests with verbose output
pytest -v

# Run tests with custom timeout
pytest --timeout 300
```

### Performance Test Integration

#### 1. Benchmark Framework

```cpp
#include <benchmark/benchmark.h>
#include <nlm/core/brain/Neuron.h>

static void BM_NeuronSimulation(benchmark::State& state) {
    for (auto _ : state) {
        Neuron neuron;
        neuron.simulate(0.1);
    }
}
BENCHMARK(BM_NeuronSimulation);

BENCHMARK_MAIN();
```

Run benchmarks:

```bash
# Build with benchmarks
mkdir -p build
cd build
cmake .. -DBUILD_BENCHMARKS=ON
cmake --build . --target benchmark

# Run benchmarks
./benchmark --benchmark_out=benchmark_results.json \
                          --benchmark_out_format=json
```

### Test Result Reporting

#### 1. Automated Test Reporting

Create `scripts/generate_test_report.py`:

```python
import xml.etree.ElementTree as ET
import json
import os

def generate_test_report():
    # Parse CTest results
    xml_path = "test_results/ctest_results.xml"
    if os.path.exists(xml_path):
        tree = ET.parse(xml_path)
        root = tree.getroot()
        
        # Generate summary
        total_tests = 0
        passed_tests = 0
        failed_tests = 0
        
        for test in root.findall('.//testcase'):
            total_tests += 1
            if 'failure' not in test.attrib:
                passed_tests += 1
            else:
                failed_tests += 1
        
        summary = {
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': failed_tests,
            'pass_rate': (passed_tests / total_tests * 100) if total_tests > 0 else 0
        }
        
        with open('test_results/test_summary.json', 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Test Summary: {summary}")

if __name__ == "__main__":
    generate_test_report()
```

#### 2. Continuous Integration Reporting

GitHub Actions example for test reporting:

```yaml
tasks:
  - name: Run C++ tests
    run: |
      mkdir -p build
      cd build
      ctest --output-on-failure --output-junit test-results.xml
    shell: bash

  - name: Run Python tests
    run: |
      pytest tests/ \
        --cov=pynlm \
        --cov-report=xml \
        --cov-report=html \
        --cov-report=term-missing \
        --cov-config=.coveragerc
    shell: bash

  - name: Generate test report
    run: |
      python scripts/generate_test_report.py
    shell: python

  - name: Upload test results
    uses: actions/upload-artifact@v3
    with:
      name: test-results
      path: test_results/
```

## Performance Testing

### Automated Performance Testing

#### 1. Performance Test Framework

```bash
# Build with performance tests
mkdir -p build
cd build
cmake .. -DBUILD_PERFORMANCE_TESTS=ON
cmake --build . --target performance_tests

# Run performance tests
./performance_tests --output performance_metrics.json
```

#### 2. Performance Monitoring

```cpp
// In performance test files
#include <nlm/experiments/Metrics.h>

void benchmark_neural_dynamics() {
    Metrics metrics("neural_dynamics");
    
    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (int i = 0; i < 1000; ++i) {
        // Neural dynamics computation
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    metrics.record(duration.count(), "simulation_time");
    metrics.log();
}
```

### Memory Usage Testing

```bash
# Monitor memory usage during build
valgrind --tool=massif build/nlm configs/default.cfg

# Generate memory profiles
ms_print massif.out.12345 > memory_report.html
```

## Documentation Generation

### Automatic Documentation Generation

#### 1. Doxygen Configuration

Create `Doxyfile` or generate it:

```bash
doxygen Doxyfile
```

Doxygen configuration options:

```cmake
# In CMakeLists.txt
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
```

#### 2. Sphinx Documentation

Create `docs/conf.py`:

```python
project = 'NLM'
copyright = '2024, NLM Authors'
author = 'NLM Authors'
release = '0.1.0'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.intersphinx',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
```

Build documentation:

```bash
sphinx-build docs/ docs/_build/html/
```

## Release Management

### Package Creation

#### 1. Debian Package

```bash
# Install packaging tools
apt-get install -y devscripts dpkg-dev

# Create source package
dpkg-buildpackage -us -uc -b

# Create binary package
make deb
```

#### 2. RPM Package

```bash
# Install rpm-build
yum install -y rpm-build

# Create RPM package
rpmbuild -ta nlm-0.1.0.tar.gz
```

#### 3. Python Package

```bash
# Build Python package
python setup.py sdist bdist_wheel

# Upload to PyPI
pip install twine
twine upload dist/*
```

### Distribution Channels

#### 1. GitHub Releases

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build
        run: bash build.sh
      - name: Create Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            build/nlm
            dist/*
```

#### 2. Docker Hub

```dockerfile
# Dockerfile for distribution
FROM ubuntu:22.04

LABEL maintainer="NLM Team"
LABEL description="NLM Runtime Environment"

# Install required runtime dependencies
RUN apt-get update && apt-get install -y \
    # Required runtime dependencies

COPY build/nlm /usr/local/bin/nlm
RUN chmod +x /usr/local/bin/nlm

ENTRYPOINT ["nlm"]
```

### Version Management

#### 1. Semantic Versioning

Use semantic versioning with:
- **Major version**: Breaking changes
- **Minor version**: New features, backward compatible
- **Patch version**: Bug fixes, backward compatible

Version bump scripts:

```bash
#!/bin/bash

# Bump major version
VERSION=$(grep -E '^version=' pyproject.toml | cut -d'=' -f2 | tr -d '"')
major=$(echo $VERSION | cut -d'.' -f1)
major=$((major + 1))
new_version="$major.0.0"

# Update version in files
sed -i "s/version = .*/version = \"$new_version\"/" pyproject.toml
sed -i "s/version = .*/version = $new_version/" CMakeLists.txt

sed -i "s/v[0-9]*\.[0-9]*\.[0-9]*/v$new_version/" CHANGELOG.md

echo "Bumped version to $new_version"
```

### Release Automation

#### 1. Release Script

```bash
#!/bin/bash

# Automated release script
set -e

VERSION=$(grep -E '^version=' pyproject.toml | cut -d'=' -f2 | tr -d '"')

# Run tests
make test

# Build artifacts
make dist

# Create GitHub release tag
git tag -a "v${VERSION}" -m "Release v${VERSION}"

# Push to remote
git push origin "v${VERSION}"

# Build and publish to PyPI
python -m build
scikit-build-core bdist_wheel
pip upload dist/*

echo "Release v${VERSION} completed"
```

## Troubleshooting

### Common Build Issues

#### 1. CMake Configuration Errors

**Issue**: CMake version is too old

**Solution**:

```bash
sudo apt update
sudo apt install -y cmake
```

**Issue**: Compiler does not support C++20

**Solution**:

```bash
# Install newer GCC
sudo apt install -y gcc-11 g++-11

# Use newer compiler
cd build
cmake .. -DCMAKE_CXX_COMPILER=g++-11
```

#### 2. Missing Dependencies

**Issue**: OpenMPI not found

**Solution**:

```bash
sudo apt install -y libopenmpi-dev
mkdir -p build
cd build
cmake .. -DENABLE_MPI=ON
```

#### 3. Build Memory Issues

**Issue**: Out of memory during build

**Solution**:

```bash
# Reduce parallelism
make -j4

# Use more memory
export MAKEFLAGS="-j4"

# Build in stages
cmake .. -DBUILD_IN_STAGES=ON
```

#### 4. Python Bindings Issues

**Issue**: Python bindings fail to build

**Solution**:

```bash
# Check Python installation
python3-config --includes

# Install required Python packages
pip install scikit-build-core pybind11 numpy

# Build with verbose output
SKBUILD_VERBOSE=1 pip install -e .
```

### Debug Information

#### 1. Generate Debug Information

```bash
# Enable debug symbols
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O2"

# Generate core dump for debugging
ulimit -c unlimited
make -j$(nproc)

# Run with GDB
start: gdb --args ./nlm configs/default.cfg
```

#### 2. Performance Profiling

```bash
# Profile with gprof
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-pg -O2"
make -j$(nproc)

# Run and generate profile
./nlm configs/default.cfg

gprof nlm gmon.out > profile.txt
```

#### 3. Memory Profiling

```bash
# Profile with valgrind
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
make -j$(nproc)

# Run with memory check
valgrind --leak-check=full --show-leak-kinds=all ./nlm configs/default.cfg
```

### Advanced Build Configurations

#### 1. Cross-Compilation Setup

```bash
# Set up cross-compilation
mkdir -p build
cd build
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
    -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
    -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++

make -j$(nproc)
```

#### 2. Docker-Based Development

```bash
# Use Docker for development
docker build -t nlm-dev .

# Run container
docker run -it --rm -v $(pwd):/workspace/nlm nlm-dev

# Build within container
docker exec -it <container_id> bash -c "cd /workspace/nlm && bash build_scripts/build.sh"
```

## Configuration Reference

### Environment Variables

| Variable | Description | Default | Example |
|----------|-------------|---------|---------|
| `NLM_CONFIG_FILE` | Path to configuration file | `./configs/default.cfg` | `/path/to/config.cfg` |
| `NLM_LOG_LEVEL` | Logging verbosity | `INFO` | `DEBUG`, `WARN`, `ERROR` |
| `NLM_WORKING_DIR` | Working directory for runtime | `.nlm` | `/home/user/nlm_workspace` |
| `NLM_CHECKPOINT_INTERVAL` | Checkpoint save interval | `1000` | `500`, `10000` |
| `NLM_PYTHON_VERSION` | Python version requirement | `3.8` | `3.8`, `3.9`, `3.10`, `3.11` |
| `NLM_BUILD_TYPE` | Build type | `Release` | `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel` |

### CMake Options

| Option | Description | Default | Possible Values |
|--------|-------------|---------|-----------------|
| `CMAKE_BUILD_TYPE` | Build configuration | `Release` | `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel` |
| `CMAKE_CXX_STANDARD` | C++ standard version | `20` | `11`, `14`, `17`, `20`, `23` |
| `ENABLE_MPI` | Enable MPI support | `OFF` | `ON`, `OFF` |
| `ENABLE_CUDA` | Enable CUDA support | `OFF` | `ON`, `OFF` |
| `ENABLE_SIMD` | Enable SIMD optimizations | `OFF` | `ON`, `OFF` |
| `BUILD_SHARED_LIBS` | Build shared libraries | `OFF` | `ON`, `OFF` |
| `BUILD_TESTS` | Build test executables | `ON` | `ON`, `OFF` |
| `BUILD_EXAMPLES` | Build example programs | `ON` | `ON`, `OFF` |
| `NLM_PYTHON_BINDINGS` | Build Python bindings | `ON` | `ON`, `OFF` |
| `ENABLE_VALIDATION` | Enable runtime validation | `OFF` | `ON`, `OFF` |

## API Documentation

### Python API

```python
import pynlm

# Create neural network
brain = pynlm.Brain()

# Add neurons
neuron = pynlm.Neuron()
brain.add_neuron(neuron)

# Run simulation
brain.simulate(1.0)

# Get results
spikes = brain.get_spikes()
```

### C++ API

```cpp
#include <nlm/brain/Brain.h>
#include <nlm/memory/WorkingMemory.h>

// Create neural network
NLM::Brain brain;

// Add neurons
NLM::Neuron neuron;
brain.addNeuron(neuron);

// Run simulation
brain.simulate(1.0);

// Get results
auto spikes = brain.getSpikes();
```

## Conclusion

The NLM build system provides a comprehensive, professional-grade solution for building, testing, and deploying the NLM project. It includes:

- **Multiple build configurations** for different environments
- **Parallel builds** with optimal resource utilization
- **Cross-compilation** support for ARM and other architectures
- **Performance optimization** with SIMD and CUDA support
- **Quality assurance** with automated testing and static analysis
- **Continuous integration** with GitHub Actions, GitLab CI, and Jenkins
- **Comprehensive documentation** with API reference and user guides
- **Release automation** for multiple distribution channels

The build system is designed to be flexible, efficient, and maintainable, supporting both development and production deployments across multiple platforms.
EOF