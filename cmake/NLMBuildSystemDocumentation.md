# NLM Modular Build System Documentation

## Overview

The NLM (Neuromodular Learning Machine) project uses a modular CMake build system designed to provide:

- **Clean separation of concerns** across different components
- **Automated dependency management** to simplify build configuration
- **Cross-platform support** with consistent behavior across Windows, macOS, and Linux
- **Advanced build optimizations** including parallel builds, precompiled headers, and profile-guided optimization
- **CI/CD integration** for automated testing and deployment
- **Comprehensive configuration management** with support for external configuration files
- **Extensive documentation and examples** for users and developers

## Architecture

The modular build system is organized into the following layers:

### 1. Core Build System (cmake/NLMBuildSystem.cmake)

The main entry point that defines project-wide settings, build configurations, and integrates all other modules.

### 2. Core Components (cmake/NLMBuildCore.cmake)

Defines the fundamental libraries:

- **nlm_core**: Core neural computation (neurons, synapses, plasticity, memory systems)
- **nlm_agent**: Agent interface and sensory processing
- **nlm_world**: World simulation and environment
- **nlm_phase3**: Phase 3 experimental components
- **nlm_phase4**: Phase 4 experimental components

### 3. Dependency Management (cmake/NLMDependencyManagement.cmake)

Automates dependency detection and configuration for:

- **Python Integration**: Python interpreter and pybind11 for bindings
- **Boost**: System and thread libraries
- **Eigen**: Optional linear algebra support
- **FFmpeg**: Multimedia support (when using pkg-config)
- **Graphviz**: Dependency graph visualization

### 4. Configuration Management (cmake/NLMBuildOptions.cmake)

Supports external configuration files and provides:

- **User configuration** via `~/.nlmc/config.cmake`
- **Project configuration** via `configs/NLMConfig.cmake`
- **Build options documentation** and validation

### 5. Optimization (cmake/NLMOptimization.cmake)

Provides advanced build optimizations:

- **Parallel builds** with configurable job pools
- **Precompiled headers** for faster compilation
- **Profile-guided optimization** (PGO)
- **Link-time optimization** (LTO)
- **Code coverage** reporting
- **Memory leak detection** via AddressSanitizer
- **Cross-platform compatibility** settings

### 6. Python Bindings (cmake/NLMPythonBindings.cmake)

Integrates Python bindings using pybind11:

- **Automatic module generation** for pynlm
- **Platform-specific compilation** flags
- **Package configuration** for Python installations
- **Documentation generation** for bindings

### 7. CI/CD Support (cmake/NLMCiCd.cmake)

Provides automated integration with:

- **GitHub Actions**: Multi-platform CI/CD pipeline
- **Travis CI**: Open-source CI service
- **Jenkins**: Enterprise CI/CD
- **Automated artifact packaging** and distribution

### 8. Documentation (cmake/NLMDocumentation.cmake)

Generates comprehensive documentation:

- **Doxygen** API reference documentation
- **Sphinx** Python bindings documentation
- **Example configurations** and usage guides
- **Build options reference** and tutorials

## Build Configurations

The modular build system supports multiple build configurations:

### Debug Configuration

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

**Characteristics**:
- Compiler optimizations: -O0
- Debug symbols: -g
- Definitions: DEBUG
- Warnings: Full

### Release Configuration

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Characteristics**:
- Compiler optimizations: -O3
- Debug symbols: None
- Definitions: NDEBUG
- Link-time optimizations: Enabled (when available)

### RelWithDebInfo Configuration

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j$(nproc)
```

**Characteristics**:
- Compiler optimizations: -O2
- Debug symbols: -g
- Definitions: NDEBUG
- Balanced optimization and debugging

### MinSizeRel Configuration

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j$(nproc)
```

**Characteristics**:
- Compiler optimizations: -Os
- Debug symbols: None
- Definitions: NDEBUG
- Optimized for minimal binary size

## Configuration File Support

The modular build system supports external configuration files:

### User Configuration (~/.nlmc/config.cmake)

This file is loaded automatically if it exists:

```cmake
# Add custom include directories
include_directories(${NLM_SOURCE_DIR}/custom)

# Add custom libraries
link_libraries(custom_lib)

# Override default settings
set(CMAKE_BUILD_TYPE Release)
```

### Project Configuration (configs/NLMConfig.cmake)

Project-level configuration:

```cmake
# Project-specific settings
add_definitions(-DUSE_FEATURE_X)

# Custom dependencies
find_package(CustomLib REQUIRED)

# Override build options
if(USE_FEATURE_X)
    target_compile_definitions(nlm_core PRIVATE FEATURE_X_ENABLED)
endif()
```

## Package Integration

The build system provides integration with external packages:

### Python Integration

```bash
mkdir build && cd build
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j$(nproc)
```

**Results**:
- Builds `pynlm` Python module
- Generates `__init__.py` and configuration files
- Installs Python package with proper metadata

### Optional Dependencies

**Optional dependencies can be enabled/disabled**:

```bash
# Enable code coverage
cmake .. -DNLM_ENABLE_COVERAGE=ON

# Enable memory leak detection
cmake .. -DNLM_ENABLE_SANITIZERS=ON

# Enable performance profiling
cmake .. -DNLM_ENABLE_PERFORMANCE_PROFILING=ON

# Enable distributed builds
cmake .. -DNLM_ENABLE_DISTRIBUTED_BUILD=ON

# Build shared libraries
cmake .. -DNLM_BUILD_SHARED_LIBS=ON
```

## Dependency Management

The dependency management system automates dependency detection and configuration:

### Automated Detection

The system automatically:

1. **Finds Python** with interpreter and development headers
2. **Locates pybind11** for Python bindings
3. **Detects platform-specific requirements** (X11, Accelerate framework, etc.)
4. **Configures system libraries** (Boost, FFmpeg)

### Dependency Graphs

The build system can generate dependency graphs:

```bash
mkdir build && cd build
cmake ..
cmake --build . --target dependency_graph
```

This generates a `dependency_graph.dot` file and provides a Python script to convert it to PNG.

## Build Optimization Features

### Parallel Builds

The build system supports parallel compilation:

```bash
# Basic parallel build
make -j$(nproc)

# Build with specific job count
make -j8

# For CMake generated projects
cmake --build . --parallel
```

### Incremental Builds

CMake automatically tracks dependencies and only rebuilds changed files, saving significant build time.

### Precompiled Headers

When enabled, precompiled headers reduce compilation time:

```bash
cmake .. -DNLM_BUILD_SHARED_LIBS=ON
make -j$(nproc)
```

### Profile-Guided Optimization (PGO)

PGO can be enabled for performance-critical builds:

```bash
cmake .. -DNLM_ENABLE_PERFORMANCE_PROFILING=ON
make pgo_hybrid
```

### Link-Time Optimization (LTO)

LTO can be enabled for optimized linking:

```bash
cmake .. -DNLM_ENABLE_LTO=ON
make -j$(nproc)
```

## Cross-Platform Support

The modular build system provides platform-specific configurations:

### Windows

- **Compiler**: MSVC with specific runtime libraries
- **Security**: `/W4 /WX` warnings
- **Libraries**: ws2_32, iphlpapi
- **Headers**: CRT secure warnings disabled

### macOS

- **Compiler**: Clang with libc++
- **Libraries**: Accelerate framework for math operations
- **Frameworks**: Optional graphics frameworks

### Linux

- **Compiler**: GCC/Clang with pthread support
- **Libraries**: X11 for graphical components
- **Package managers**: Integration with pkg-config

## Continuous Integration Support

The build system includes comprehensive CI/CD configurations:

### GitHub Actions

The `.github/workflows/nlm-ci.yml` file provides:

- **Multi-platform builds**: Ubuntu, Windows, macOS
- **Multiple configurations**: Debug, Release, RelWithDebInfo, MinSizeRel
- **Automated testing**: ctest with failure reporting
- **Artifact uploads**: Build outputs for distribution
- **Static analysis**: clang-analyzer integration

### Travis CI

The `.travis.yml` file provides:

- **Docker-based builds**: Consistent environment across runs
- **Package installation**: All required dependencies
- **Coverage reporting**: Automatic test coverage
- **Documentation generation**: Doxygen and Sphinx

### Jenkins

The `Jenkinsfile` provides:

- **Parameterized builds**: Build type, features, options
- **Stage-based pipeline**: Build → Test → Package
- **Email notifications**: Build status reporting
- **Artifact archiving**: Build outputs and logs

## Advanced Build Options

### Feature Toggles

The build system supports extensive feature toggling:

```bash
# Core features
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON/OFF
cmake .. -DNLM_ENABLE_TESTS=ON/OFF

# Optimization features
cmake .. -DNLM_ENABLE_COVERAGE=ON/OFF
cmake .. -DNLM_ENABLE_SANITIZERS=ON/OFF
cmake .. -DNLM_ENABLE_PERFORMANCE_PROFILING=ON/OFF

# Build features
cmake .. -DNLM_ENABLE_DISTRIBUTED_BUILD=ON/OFF
cmake .. -DNLM_BUILD_SHARED_LIBS=ON/OFF
```

### Custom Build Types

Additional build types can be defined:

```bash
cmake .. -DCMAKE_CONFIGURATION_TYPES="Debug;Release;RelWithDebInfo;MinSizeRel;CustomOpt"
```

### Installation

Standard CMake installation:

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

**Installation outputs**:

- **Libraries**: libnlm_core.so, libnlm_agent.so, etc.
- **Executables**: nlm, nlm_benchmark, demo executables
- **Headers**: NLM API headers
- **Python modules**: pynlm Python bindings
- **Documentation**: HTML and PDF documentation

## Examples and Tutorials

### Example 1: Basic Debug Build

```bash
# Create build directory
mkdir build
d cd build

# Configure with debug options
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Example 2: Release Build with Python Bindings

```bash
# Create build directory
mkdir build
d cd build

# Configure with release and Python bindings
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON

# Build all targets
make -j$(nproc)

# Generate dependency graph
make dependency_graph
```

### Example 3: Custom Configuration

Create a `configs/NLMConfig.cmake` file:

```cmake
# Custom project configuration

# Enable additional features
add_definitions(-DEXPERIMENTAL_FEATURES)

# Add custom include paths
include_directories(${NLM_SOURCE_DIR}/experimental)

# Link with custom libraries
link_libraries(experimental_lib)

# Override compiler options
if(MSVC)
    target_compile_options(nlm_core PRIVATE /permissive-)
endif()
```

## Troubleshooting

### Common Issues

**1. Python not found**

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core

# Specify Python path
cmake .. -DPYTHON_EXECUTABLE=/path/to/python
```

**2. CMake version too old**

```bash
# Install newer CMake
cmake --version
# Upgrade to >= 3.20
```

**3. Missing dependencies**

```bash
# On Ubuntu/Debian
sudo apt-get install libboost-all-dev libopencv-dev libgtk-3-dev graphviz doxygen

# On macOS
brew install boost opencv gtk graphviz doxygen
```

**4. Build errors**

```bash
# Clear build directory and reconfigure
rm -rf build
mkdir build
d cd build
cmake ..
make clean
make -j$(nproc)
```

**5. Python bindings fail to build**

```bash
# Ensure pybind11 is available
pip install pybind11

# Try building with verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
make -j$(nproc)
```

### Getting Help

1. **Documentation**: Check the `docs/` directory for comprehensive documentation
2. **Examples**: Review the examples in the `examples/` directory
3. **Build options**: Use `make nlm_build_options` to display current configuration
4. **Dependency graphs**: Use `make dependency_graph` to visualize dependencies

## Build System Summary

The NLM modular build system provides:

- **15+ configuration options** for extensive customization
- **8+ CMake modules** for clean separation of concerns
- **5+ build configurations** for different optimization levels
- **3+ CI/CD platforms** for automated testing
- **2+ optimization modes** for performance-critical builds
- **Cross-platform support** for Windows, macOS, and Linux
- **Comprehensive documentation** with examples and tutorials
- **Dependency management** for automated configuration
- **Configuration file support** for external customization
- **Advanced features** like PGO, LTO, and coverage reporting

This modular approach makes the NLM project easier to build, maintain, and extend while providing better dependency management and configuration options.
