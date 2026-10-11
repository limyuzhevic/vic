# NLM Build System - Modern CMake Enhancement

## Overview

This document outlines the enhanced CMake build system for the NLM (Neural Learning Machine) framework, implementing modern CMake practices with comprehensive test suites and performance benchmarking.

## Build System Improvements

### 1. Modern Target-Based CMake Structure

The build system now uses modern CMake practices with proper target-based dependencies:

```cmake
# Core library (static)
add_library(nlm_core STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    # ... other core sources
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Optional libraries (conditionally built)
if(NLM_PERFORMANCE_ENABLED)
    add_library(nlm_performance STATIC ${NLM_PERFORMANCE_SOURCES})
    target_link_libraries(nlm_core PUBLIC nlm_performance)
endif()

# Python bindings (pybind11)
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
```

### 2. Build Options

The build system provides the following configuration options:

```cmake
# Core build options
option(NLM_PERFORMANCE "Enable performance optimizations (SIMD, multithreading)" ON)
option(NLM_VISUALIZATION "Enable visualization support" ON)
option(NLM_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_TESTS "Build test suite" ON)
option(NLM_BENCHMARKS "Build performance benchmarks" ON)
option(NLM_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ASAN "Enable AddressSanitizer" OFF)
option(NLM_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# Performance tuning
option(NLM_USE_SIMD "Enable SIMD optimizations" ON)
option(NLM_THREAD_COUNT "Number of threads for parallel processing" AUTO)

# Platform-specific options
option(NLM_PLATFORM_LINUX "Linux-specific optimizations" ON)
option(NLM_PLATFORM_MACOS "macOS-specific optimizations" OFF)
option(NLM_PLATFORM_WINDOWS "Windows-specific optimizations" OFF)
```

### 3. Installation Targets

The build system provides comprehensive installation support:

```cmake
# Install libraries
install(TARGETS nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_performance
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION bin
)

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Install configuration
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    DESTINATION lib/cmake/nlm
)
```

### 4. Configuration Export

The build system provides CMake package configuration for find_package support:

```cmake
# Create nlm-config.cmake
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

# Create nlm-config-version.cmake
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${NLM_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)
```

## Testing Framework

### 1. Google Test Integration

The test suite uses Google Test framework:

```cmake
# Find Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# Create test target
gtest_discover_tests(nlm_test
    TEST_PREFIX "nlm_"
    PROPERTY working_directory ${CMAKE_CURRENT_BINARY_DIR}
)
```

### 2. Test Categories

The test suite is organized into categories:

#### Unit Tests
- **Brain tests**: Brain creation, initialization, step execution, reset functionality
- **Config tests**: Configuration loading, parameter validation, serialization
- **Performance tests**: Performance monitoring, benchmarking
- **Visualization tests**: Visualization interface, rendering, update rates

#### Integration Tests
- **System integration**: Component interaction, data flow
- **Cross-platform**: Platform-specific behavior
- **Memory management**: Memory leak detection, allocation tracking

### 3. Performance Benchmarking

Performance benchmarks are implemented as standalone executables:

```bash
# Run all benchmarks
./benchmark --runs 100 --size medium

# Specific benchmark types
./benchmark --benchmark-type memory_pool
./benchmark --benchmark-type event_queue
./benchmark --benchmark-type simd_vectorization
```

### 4. Memory Leak Detection

Memory leak detection is enabled with sanitizers:

```bash
# Build with AddressSanitizer
cmake .. -DNLM_ASAN=ON
make -j$(nproc)

# Run tests with memory leak detection
./nlm_test --gtest_filter=*Leak* --leaks_report
```

### 5. Cross-Platform Testing

Cross-platform testing support:

```cmake
# Platform-specific test configuration
if(UNIX AND NOT APPLE)
    # Linux-specific tests
    target_compile_definitions(nlm_test PRIVATE NLM_PLATFORM_LINUX)
endif()

if(APPLE)
    # macOS-specific tests
    target_compile_definitions(nlm_test PRIVATE NLM_PLATFORM_MACOS)
endif()

if(WIN32)
    # Windows-specific tests
    target_compile_definitions(nlm_test PRIVATE NLM_PLATFORM_WINDOWS)
endif()
```

## Python Bindings Enhancement

### 1. Modern pybind11 Integration

Enhanced Python bindings with better documentation:

```cmake
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Install Python package
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Create Python package metadata
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python/setup.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py @ONLY)
```

### 2. Python Test Suite

```python
# tests/python_test_suite.py
import pynlm
import pytest
import tempfile
import json
import os

class TestPythonBindings:
    def test_config_creation(self):
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        config.set("neuron_count", 100)
        assert config.get("neuron_count") == 100
        
    def test_brain_creation(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        
    def test_visualization(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        vis = pynlm.createVisualization()
        vis.initialize()
        vis.setUpdateRate(30.0)
        
        assert vis.isActive()
        assert vis.getUpdateRate() == 30.0
        
        vis.close()
```

### 3. Performance Monitoring

Performance monitoring from Python:

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
config.set("performance.monitoring", True)
config.set("performance.profiling", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with performance tracking
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        stats = brain.getPerformanceStats()
        print(f"Step {step}: {stats.neuronsPerSecond} neurons/sec")
        brain.logPerformanceReport()
```

## Build Commands

### Standard Build

```bash
# Configure with all options
mkdir build && cd build
cmake .. \
    -DNLM_PERFORMANCE=ON \
    -DNLM_VISUALIZATION=ON \
    -DNLM_PYTHON_BINDINGS=ON \
    -DNLM_TESTS=ON \
    -DNLM_BENCHMARKS=ON \
    -DNLM_THREAD_COUNT=8 \
    -DNLM_USE_SIMD=ON

# Build
make -j$(nproc)
```

### Installation

```bash
# Install to system
cmake --build . --target install

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
cmake --build . --target install
```

### Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test categories
ctest -R "test_brain"
ctest -R "test_config"
ctest -R "test_performance"

# Run performance benchmarks
./benchmark --help
./benchmark --runs 100
```

### Python Package Installation

```bash
# Install Python bindings
cd python
pip install -e .

# Test Python bindings
python -m pytest python_tests.py -v
```

### Performance Testing

```bash
# Build with optimizations
cmake .. -DNLM_PERFORMANCE=ON -DNLM_USE_SIMD=AVX2 -DNLM_THREAD_COUNT=8
make -j$(nproc)

# Run performance benchmarks
./benchmark --benchmark-type all --runs 50
```

## Documentation

### Advanced Usage Documentation

The advanced usage documentation (`docs/USAGE_ADVANCED.md`) provides:

- Configuration optimization guides
- Performance tuning techniques
- Cross-platform compatibility information
- Debugging and troubleshooting
- Advanced usage examples

### API Documentation

Python bindings include comprehensive documentation:

```python
import pynlm
help(pynlm)
```

### Examples

Example usage from Python:

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        stats = brain.getPerformanceStats()
        print(f"Step {step}: {stats.neuronsPerSecond:.0f} neurons/sec")

# Get final statistics
brain.logPerformanceReport()
```

## CI/CD Integration

### GitHub Actions Example

```yaml
name: NLM Build and Test

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: |
        cmake -B build -S . 
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
          -DNLM_TESTS=ON
          -DNLM_PYTHON_BINDINGS=ON
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Run Tests
      run: cd build && ctest --output-on-failure
    
    - name: Run Python Tests
      run: cd python && pip install -e . && python -m pytest
```

### Azure Pipelines Example

```yaml
stages:
- stage: Build
  jobs:
  - job: Linux
    pool:
      vmImage: 'ubuntu-latest'
    steps:
    - script: |
        mkdir build
        cd build
        cmake .. -DNLM_TESTS=ON -DNLM_PYTHON_BINDINGS=ON
        make -j$(nproc)
        ctest --output-on-failure
      displayName: 'Build and Test'
  
  - job: macOS
    pool:
      vmImage: 'macOS-latest'
    steps:
    - script: |
        mkdir build
        cd build
        cmake .. -DNLM_TESTS=ON -DNLM_PYTHON_BINDINGS=ON
        make -j$(sysctl -n hw.ncpu)
        ctest --output-on-failure
      displayName: 'Build and Test'
```

## Troubleshooting

### Common Issues and Solutions

1. **Build fails with SIMD errors**
   ```bash
   # Disable SIMD optimizations
cmake .. -DNLM_USE_SIMD=OFF
   ```

2. **Python bindings not found**
   ```bash
   # Install Python dependencies
   pip install pybind11 numpy matplotlib
   
   # Rebuild Python bindings
   cd python && rm -rf build && mkdir build && cd build && cmake .. && make
   ```

3. **Tests fail with memory errors**
   ```bash
   # Build with AddressSanitizer
cmake .. -DNLM_ASAN=ON
make -j$(nproc)

   # Run with memory leak detection
   ./nlm_test --leaks_report
   ```

4. **Cross-platform compilation issues**
   ```bash
   # Check platform-specific options
   cmake .. -DNLM_PLATFORM_LINUX=ON
   cmake .. -DNLM_PLATFORM_MACOS=ON
   cmake .. -DNLM_PLATFORM_WINDOWS=ON
   ```

## Future Enhancements

### Planned Improvements

1. **Export Configuration**
   - Support for JSON configuration export
   - External configuration management

2. **Distributed Computing**
   - Multi-node simulation support
   - MPI integration for large-scale simulations

3. **Advanced Visualization**
   - 3D visualization support
   - Web-based visualization interface

4. **Plugin Architecture**
   - Extensible plugin system
   - Dynamic module loading

5. **Advanced Profiling**
   - Detailed performance analysis
   - Visualization of performance bottlenecks

6. **Machine Learning Integration**
   - Integration with ML frameworks (TensorFlow, PyTorch)
   - Neural network transfer learning

7. **Real-time Data Streaming**
   - Real-time data ingestion
   - Live visualization updates

8. **Export Capabilities**
   - Multiple output formats (JSON, HDF5, Parquet)
   - Simulation result archiving

## License

This enhanced build system is licensed under the NLM License.

## Acknowledgments

Special thanks to the open-source community for Google Test, pybind11, and other contributing projects.
