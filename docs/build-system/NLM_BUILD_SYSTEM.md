# NLM Build System Documentation
# Comprehensive documentation for NLM's modern build system and testing infrastructure

# Table of Contents
# 1. Overview
# 2. Build System Architecture
# 3. Modern CMake Configuration
# 4. Testing Infrastructure
# 5. CI/CD Pipeline
# 6. Build Optimization
# 7. Docker Support
# 8. Performance Monitoring
# 9. Advanced Features
# 10. Troubleshooting
# 11. Migration Guide
# 12. API Reference

# 1. Overview
# ============

# NLM Build System Overview

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The build system supports the compilation and testing of all NLM components, including neural modules, agent systems, world simulations, and advanced experiment frameworks.

## Key Features

- **Modern CMake Build System**: Multi-configuration, platform-aware, dependency-managed
- **Comprehensive Testing**: Google Test-based unit tests, integration tests, performance benchmarks
- **CI/CD Pipeline**: GitHub Actions with multi-platform support
- **Build Optimization**: Advanced caching, incremental compilation, performance analysis
- **Docker Support**: Containerized development and production environments
- **Performance Monitoring**: Build time metrics, memory profiling, resource usage tracking

# 2. Build System Architecture
# ============================

# Directory Structure

```
/workspace/app
├── CMakeLists.txt                    # Main build configuration
├── .github/workflows/                # CI/CD pipeline
├── .dockerignore                     # Docker ignore patterns
├── Dockerfile                        # Development environment
├── setup_build_environment.sh        # Environment setup script
├── analyze_build.sh                  # Build analysis tools
├── manage_build_cache.sh             # Cache management
├── docs/                             # Documentation
├── build/                           # Build directory (generated)
├── tests/                           # Testing framework
│   ├── CMakeLists.txt                # Test build configuration
│   ├── test_*.cpp                    # Test implementations
│   ├── test_config.cmake             # Test configuration
│   └── data/                         # Test data
├── benchmarks/                      # Performance benchmarks
├── scripts/                         # Build scripts
└── configs/                         # Configuration files
```

# Build Components

## Library Modules

### Core Libraries
- **nlm_core**: Core configuration, types, random number generation, logging
- **nlm_brain**: Neural components (neurons, synapses, populations, regions)
- **nlm_dynamics**: Neural dynamics (spike systems, neural networks)
- **nlm_plasticity**: Plasticity rules (Hebbian, STDP, structural)

### Development Libraries
- **nlm_development**: Development system (synaptogenesis, pruning, maturation)
- **nlm_neuromodulation**: Neuromodulators (dopamine, curiosity, novelty)
- **nlm_memory**: Memory systems (working, episodic, semantic, procedural)

### Cognition and Behavior
- **nlm_cognition**: Concept formation, neural planning
- **nlm_sensory**: Sensory processing (vision, audio, internal signals)
- **nlm_prediction**: Prediction systems, neural prediction
- **nlm_motor**: Motor system
- **nlm_environment**: Environment interface

### System Libraries
- **nlm_agent**: Agent system (sensory percept, agent-brain interface)
- **nlm_world**: World system (environment, objects, simple world)

### Experiment Libraries
- **nlm_experiments**: Experiment framework, metrics, advanced experiments
- **nlm_phase3**: Phase 3 experiment implementations
- **nlm_phase4**: Phase 4 experiment implementations

## Executable Modules

### Main Executables
- **nlm**: Main brain simulation
- **nlm_phase3_demo**: Phase 3 demonstration
- **nlm_advanced_experiment_demo**: Advanced experiment demonstration

## Testing Framework

### Test Categories
- **Unit Tests**: Component-level testing (types, config, random, clock, neurons, synapses)
- **Integration Tests**: System component interaction testing
- **Performance Tests**: Benchmarking and performance analysis
- **Regression Tests**: Historical comparison testing

### Test Infrastructure
- Google Test framework
- Custom test data management
- Automated test generation
- Test coverage analysis
- Performance benchmarking

# 3. Modern CMake Configuration
# ================================

# CMake Version Requirements

```cmake
cmake_minimum_required(VERSION 3.19)
```

# Key CMake Features

## Multi-Configuration Support

The build system supports four build configurations:

- **Debug**: `-g -O0 -DDEBUG`
- **Release**: `-O3 -DNDEBUG`
- **RelWithDebInfo**: `-O2 -g -DNDEBUG`
- **MinSizeRel**: `-Os -DNDEBUG`

```cmake
# Configure CMake for multi-configuration builds
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()

# Set configuration-specific flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Wpedantic -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -Wall -Wextra -Wpedantic -DNDEBUG")
```

## Platform Detection and Configuration

Automatic detection and configuration based on the target platform:

```cmake
# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Platform: Linux")
    set(PLATFORM_LINUX 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Platform: macOS")
    set(PLATFORM_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Platform: Windows")
    set(PLATFORM_WINDOWS 1)
endif()
```

## Dependency Management

Automatic discovery and management of dependencies:

```cmake
# Find required packages
find_package(Threads REQUIRED)

# Check for compiler features
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMPILER_SUPPORTS_CXX17 1)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0)
        set(COMPILER_SUPPORTS_CXX20 1)
    endif()
endif()
```

## Build Options

Configurable build options via CMake GUI or command line:

```cmake
option(NLM_BUILD_SHARED "Build shared libraries instead of static" OFF)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_EXAMPLES "Build examples" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory/debug sanitizers" OFF)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_PERF_MEASUREMENTS "Enable build-time performance measurements" ON)
```

# 4. Testing Infrastructure
# ============================

# Test Framework Overview

## Google Test Integration

The NLM testing framework is built on Google Test (gtest) for comprehensive unit testing:

```cmake
# Fetch Google Test
include(FetchContent)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)
```

## Test Categories

### Unit Tests
- **test_types.cpp**: Core type testing (NeuronId, SynapseId, RegionId, etc.)
- **test_config.cpp**: Configuration system testing
- **test_random.cpp**: Random number generation testing
- **test_clock.cpp**: Simulation clock testing
- **test_neuron.cpp**: Neuron dynamics testing
- **test_synapse.cpp**: Synapse transmission testing
- **test_stdp.cpp**: STDP plasticity testing
- **test_brain.cpp**: Brain system integration testing

### Test Categories Structure

```cpp
// Test namespace organization
namespace test_types {
    void testNeuronId();
    void testSynapseId();
    void testRegionId();
    void testPopulationId();
    void testNeuronType();
    void testSynapseType();
    void testPlasticityFlags();
    void testSpikeEvent();
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

// ... other test namespaces
```

## Performance Testing

### Benchmark Framework

```cmake
# Performance benchmark executable
add_executable(nlm_perf_benchmark
    src/agent/benchmark.cpp
)

target_link_libraries(nlm_perf_benchmark
    PRIVATE gtest_main nlm_core nlm_brain nlm_memory
)
```

### Benchmark Categories

- **CPU Performance**: Neural computation speed
- **Memory Usage**: Memory allocation patterns
- **I/O Performance**: File system operations
- **Network Performance**: Inter-component communication
- **Stress Testing**: System limits and capacity

## Test Data Management

### Automated Test Generation

```cpp
// Test configuration for different scenarios
test_config {
    "framework": "google_test",
    "language": "C++",
    "standard": "C++20",
    "performance": {
        "benchmark_enabled": true,
        "benchmark_iterations": 1000,
        "benchmark_warmup": 100
    },
    "coverage": {
        "enabled": true,
        "report_format": "json",
        "threshold": 90.0
    }
}
```

## Test Coverage Analysis

### Coverage Configuration

```cmake
# Enable code coverage
if(NLM_ENABLE_COVERAGE)
    target_compile_options(nlm_test PRIVATE --coverage)
    target_link_options(nlm_test PRIVATE --coverage)
endif()
```

### Coverage Metrics

- **Line Coverage**: Code execution paths
- **Branch Coverage**: Decision outcomes
- **Function Coverage**: Function calls
- **File Coverage**: File-level testing

# 5. CI/CD Pipeline
# =================

# Pipeline Configuration

## GitHub Actions Workflow

```yaml
# .github/workflows/nlm-ci-cd.yml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  workflow_dispatch:

jobs:
  build-and-test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        build_type: [Debug, Release, RelWithDebInfo]
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4
    
    - name: Configure CMake
      run: |
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build project
      run: |
        cd build
        cmake --build . --parallel $(nproc)
    
    - name: Run unit tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Generate coverage report
      run: |
        cd build
        if [ -f ./coverage_report ]; then
          ./coverage_report --generate --format=html
        fi
```

## Pipeline Stages

1. **Build Stage**: Compile code for multiple platforms
2. **Test Stage**: Run unit tests, integration tests, performance benchmarks
3. **Coverage Stage**: Generate and analyze test coverage
4. **Security Stage**: Vulnerability scanning
5. **Deployment Stage**: Package and deploy artifacts

## Artifact Management

- **Build Artifacts**: Compiled binaries, libraries, documentation
- **Test Artifacts**: Test results, coverage reports, performance metrics
- **Cache Artifacts**: Build dependencies and intermediate files

# 6. Build Optimization
# ====================

# Optimization Techniques

## Build Caching

### ccache Integration

```cmake
# Enable compiler cache
option(USE_CCACHE "Use ccache for compiler caching" ON)

if(USE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    endif()
endif()
```

### CMake Build Caching

```bash
# Configure with cache
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..

# Build with parallel jobs
make -j$(nproc)

# Incremental build
make target_name
```

## Performance Monitoring

### Build Time Analysis

```bash
# Analyze build performance
./analyze_build.sh

# Monitor build resources
./scripts/monitor_perf.sh
```

### Resource Usage Tracking

```bash
# Monitor CPU, memory, I/O during build
perf stat -e cpu-clock,cache-references,cache-misses,instructions,cycles make -j8

# Track memory usage
valgrind --tool=massif ./nlm_test
```

## Dependency Optimization

### Dependency Graph Analysis

```bash
# Generate dependency graph
generate_dependencies.py --output=dependency_graph.dot

# Visualize dependencies
dot -Tpng dependency_graph.dot -o dependency_graph.png
```

### Incremental Build

CMake automatically handles incremental builds:

```bash
# Only rebuild modified targets
make target_name

# Rebuild only when dependencies change
make all
```

# 7. Docker Support
# =================

# Development Environment

## Dockerfile Overview

```dockerfile
FROM ubuntu:22.04 AS base

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    curl \
    wget \
    python3 \
    python3-pip

# Install Python dependencies
RUN pip3 install --no-cache-dir \
    numpy \
    scipy \
    matplotlib \
    pytest \
    pytest-cov

# Create NLM directory structure
WORKDIR /opt/nlm
COPY . src/
```

## Docker Commands

```bash
# Build development image
docker build -t nlm-dev .

# Run development environment
docker run -it --rm -v $(pwd):/opt/nlm nlm-dev /bin/bash

# Build and test
docker exec nlm-dev /opt/nlm/scripts/dev.sh build Release
```

## Production Environment

```dockerfile
FROM base AS production

# Remove build tools
RUN apt-get remove -y build-essential cmake git curl wget

# Copy built binaries
COPY --from=build-env /opt/nlm/bin ./bin
COPY --from=build-env /opt/nlm/lib ./lib

# Create runtime configuration
COPY config/ ./config/

# Run NLM
CMD ["./bin/nlm", "--config", "./config/production.cfg"]
```

# 8. Performance Monitoring
# ============================

# Performance Metrics Collection

## Build Time Metrics

```cmake
# Enable build time measurement
if(NLM_ENABLE_PERF_MEASUREMENTS)
    add_custom_target(measure_build_time
        COMMAND ${CMAKE_COMMAND} -E time cmake --build ${CMAKE_BINARY_DIR} --target nlm_core
        COMMENT "Measuring build time for nlm_core"
    )
endif()
```

## Resource Monitoring

### System Monitoring

```bash
# Monitor system resources during build
script -c "perf stat -e cpu-clock,cache-references,cache-misses make -j8" build.log

# Track memory usage
/usr/bin/time -v make -j8
```

### Build Performance Analysis

```bash
# Analyze build performance
./analyze_build.sh

# Generate build report
cmake --build . --target build_metrics
```

# 9. Advanced Features
# ====================

# Cross-Compilation Support

## Configure Cross-Compilation

```cmake
# Find cross-compiler
find_program(CROSS_COMPILER_NAME aarch64-linux-gnu-g++ REQUIRED)

# Configure cross-compilation toolchain
set(CMAKE_CXX_COMPILER ${CROSS_COMPILER_NAME})

# Set system name and processor
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Additional cross-compilation settings
add_compile_options(--sysroot=/usr/aarch64-linux-gnu)
```

## Toolchain Files

```cmake
# Toolchain file for cross-compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
```

# Package Configuration

## Install Targets

```cmake
# Install libraries and executables
install(TARGETS nlm nlm_core nlm_agent nlm_world
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)
```

# 10. Troubleshooting
# ====================

# Common Issues and Solutions

## CMake Configuration Errors

### Error: CMake version too low
**Solution**: Update CMake to at least version 3.19:

```bash
sudo apt-get install cmake
```

### Error: Compiler not found
**Solution**: Install required compilers:

```bash
# Linux
sudo apt-get install g++

# macOS
xcode-select --install

# Windows
choco install mingw
```

## Build Failures

### Error: Missing dependencies
**Solution**: Install development packages:

```bash
sudo apt-get install libgtest-dev libbenchmark-dev libjsoncpp-dev
```

### Error: Test failures
**Solution**: Run tests with verbose output:

```bash
cd build
./nlm_test --gtest_list_tests
./nlm_test --gtest_filter=test_types.*
```

## Performance Issues

### Slow build times
**Solutions**:
1. Enable ccache: `ccache -M 10G`
2. Use parallel builds: `make -j$(nproc)`
3. Clean cache: `./manage_build_cache.sh clean`
4. Enable incremental builds

### Memory usage during build
**Solutions**:
1. Limit parallel jobs: `make -j4`
2. Increase swap space
3. Monitor memory usage: `./scripts/monitor_perf.sh`

# 11. Migration Guide
# ====================

# From Old to New Build System

## Migration Steps

### Step 1: Update CMake Configuration

Replace old CMake configuration:

```cmake
# Old
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)

# New
cmake_minimum_required(VERSION 3.19)
project(NLM VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "An Experimental Artificial Developmental Brain")
if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
endif()
```

### Step 2: Update Test Framework

Replace custom test framework with Google Test:

```cpp
// Old
namespace test_types {
    void runAll() {
        testNeuronId();
        testSynapseId();
        // ... other tests
    }
}

// New with Google Test
TEST(TestTypes, NeuronId) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    EXPECT_EQ(id1.index(), 1);
    EXPECT_NE(id1, id2);
}
```

### Step 3: Add Build Options

Add new CMake options:

```cmake
option(NLM_ENABLE_COVERAGE "Enable code coverage" OFF)
option(NLM_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
```

# 12. API Reference
# ====================

# Build System API

## CMake Functions

### `setup_target_cxx_features(target)`
Set C++20 standard features for target.

### `print_build_summary()`
Print build configuration summary.

## Command Line Tools

### `./setup_build_environment.sh`
Set up development environment.

### `./analyze_build.sh`
Analyze build performance and provide recommendations.

### `./manage_build_cache.sh`
Manage build cache for optimization.

### `./scripts/dev.sh`
Development workflow script.

## Configuration Files

### `tests/test_config.cmake`
Test framework configuration.

### `configs/default.cfg`
Default NLM configuration.

### `Dockerfile`
Docker development environment.

# Conclusion
# ============

The NLM build system provides a modern, robust, and production-ready foundation for the NLM project. It includes:

1. **Modern CMake**: Multi-configuration, platform-aware, dependency-managed
2. **Comprehensive Testing**: Google Test-based with coverage analysis
3. **CI/CD Pipeline**: Automated testing and deployment
4. **Build Optimization**: Caching, incremental compilation, performance monitoring
5. **Docker Support**: Containerized development and production
6. **Advanced Features**: Cross-compilation, package configuration
7. **Documentation**: Comprehensive guides and references

This build system ensures NLM can be developed, tested, and deployed efficiently across different platforms and environments while maintaining high code quality and performance standards.
