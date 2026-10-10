# NLM Build System Documentation

## Overview

This document provides comprehensive information about the NLM (Neural Learning Machine) build system, including build configurations, packaging options, and deployment strategies.

## Table of Contents

1. [Quick Start](build_docs.md#quick-start)
2. [Build Types](build_docs.md#build-types)
3. [CMake Configuration](build_docs.md#cmake-configuration)
4. [Python Bindings](build_docs.md#python-bindings)
5. [Packaging](build_docs.md#packaging)
6. [Installation](build_docs.md#installation)
7. [Troubleshooting](build_docs.md#troubleshooting)
8. [Advanced Configuration](build_docs.md#advanced-configuration)

## Quick Start

### Basic Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Building with Python Bindings

```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j4
```

### Building Tests

```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_TESTS=ON
make -j4
ctest
```

## Build Types

NLM supports the following CMake build types:

- **Debug**: Full debug information, no optimizations, assertions enabled
- **Release**: Optimized build with full features enabled
- **RelWithDebInfo**: Optimized build with debug information
- **MinSizeRel**: Optimized for minimum binary size

### Build Type Configuration

Each build type has different warning levels and compiler optimizations:

- **Debug**: Enables `-Werror` on compilers (GNU/Clang) for strict checking
- **Release**: Standard optimization and warning levels
- **RelWithDebInfo**: Optimized with debug symbols for development
- **MinSizeRel**: Optimized for smallest binary size

## CMake Configuration

### Core CMake Settings

```cmake
cmake_minimum_required(VERSION 3.19)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

### Build Options

NLM provides the following build configuration options:

| Option | Default | Description |
|--------|---------|-------------|
| `NLM_ENABLE_TESTS` | ON | Enable/disable test building |
| `NLM_ENABLE_PYTHON_BINDINGS` | ON | Enable/disable Python bindings |
| `NLM_ENABLE_PERFORMANCE` | OFF | Enable performance optimizations |
| `NLM_ENABLE_COVERAGE` | OFF | Enable code coverage reporting |
| `NLM_ENABLE_ASAN` | OFF | Enable AddressSanitizer |
| `NLM_ENABLE_TSAN` | OFF | Enable ThreadSanitizer |

### Compiler Warnings

NLM implements comprehensive warning systems based on build type and compiler:

**MSVC Warnings:**
```cmake
target_compile_options(target PRIVATE /W4)
```

**GCC/Clang Warnings:**
```cmake
target_compile_options(target PRIVATE -Wall -Wextra -Wpedantic)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(target PRIVATE -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
endif()
```

**Debug Build Additional Warnings:**
```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(target PRIVATE -Werror)  # Strict checking in debug
endif()
```

### Platform-Specific Optimizations

#### Linux
```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(nlm_core PRIVATE -march=native)
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_options(nlm_core PRIVATE -O3 -flto)
        target_link_options(nlm_core PRIVATE -flto)
    endif()
endif()
```

#### macOS
```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_options(nlm_core PRIVATE -O3 -flto)
        target_link_options(nlm_core PRIVATE -flto)
    endif()
endif()
```

#### Windows
```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_options(nlm_core PRIVATE /O2 /GL)
        target_link_options(nlm_core PRIVATE /LTCG)
    endif()
endif()
```

### Sanitizer Support

#### AddressSanitizer
```cmake
if(NLM_ENABLE_ASAN)
    target_compile_options(nlm_core PRIVATE -fsanitize=address)
    target_link_options(nlm_core PRIVATE -fsanitize=address)
endif()
```

#### ThreadSanitizer
```cmake
if(NLM_ENABLE_TSAN)
    target_compile_options(nlm_core PRIVATE -fsanitize=thread)
    target_link_options(nlm_core PRIVATE -fsanitize=thread)
endif()
```

### Code Coverage
```cmake
if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_core PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(nlm_core PRIVATE --coverage)
endif()
```

## Python Bindings

### Python Module Installation

The Python bindings support comprehensive installation with multiple formats:

#### Build Configuration
```cmake
cmake_minimum_required(VERSION 3.19)
project(pynlm_bindings LANGUAGES CXX)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)
```

#### Module Structure
```cmake
pybind11_add_module(pynlm bindings.cpp)

# Standardized include directories
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Compiler-specific configurations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive- /bigobj)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pynlm PRIVATE -fPIC -fstack-protector-strong)
endif()
```

#### Installation Targets
```cmake
# Main module installation
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}/pynlm
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Python package structure
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${Python_SITEARCH}/pynlm
    PATTERN "*.py"
    PATTERN "*.pyi"
)

# Package metadata
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml"
    COPYONLY
)
```

### Python Package Components

The Python package includes:

- **Core module**: `pynlm` - Main NLM bindings
- **Submodules**: Configuration, performance, debugging, export, analysis utilities
- **Package data**: JSON configuration files, documentation
- **Metadata**: setup.py, pyproject.toml, requirements.txt

## Packaging

### CPack Configuration

NLM uses CPack for comprehensive package generation:

```cmake
include(CPack)

# Package metadata
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - An Experimental Artificial Developmental Brain")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NLM Neural Simulation Framework")
set(CPACK_PACKAGE_CONTACT "contact@nlm.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")

# Dependencies
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgomp1")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc, libstdc++")

# Package components
include(CPackComponent)

cpack_component(core
    DISPLAY_NAME "Core Library"
    DESCRIPTION "Core neural simulation library"
    REQUIRED
)

cpack_component(python_bindings
    DISPLAY_NAME "Python Bindings"
    DESCRIPTION "Python bindings for NLM"
    OPTIONAL
)
```

### Supported Package Formats

#### DEB Packages (Debian/Ubuntu)

```bash
cmake --build . --config Release -- -j4
cpack --config Debug/Deb
```

#### RPM Packages (Fedora/CentOS)

```bash
cmake --build . --config Release -- -j4
cpack --config Debug/RPM
```

#### TGZ Archives (Tarballs)

```bash
cmake --build . --config Release -- -j4
cpack --config Debug/TGZ
```

#### NSIS Installers (Windows)

```bash
cmake --build . --config Release -- -j4
cpack --config Debug/NSIS
```

### Package Components

1. **Core Library**
   - Static and shared libraries (`libnlm_core.a`, `libnlm_core.so`)
   - Header files (`include/nlm/*.hpp`)
   - Executable binaries (`bin/nlm_*`)

2. **Python Bindings**
   - Python extension module (`pynlm*.so`/`pynlm*.pyd`)
   - Python package files
   - Python documentation

3. **Documentation**
   - Build documentation
   - API documentation
   - Examples and tutorials

## Installation

### System Installation

```bash
# Install system-wide (requires sudo)
cmake --build . --config Release -- install
```

### Local Development Installation

```bash
# Install to local prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Python Package Installation

```bash
# Install Python bindings to Python site-packages
cmake --build . --config Release -- install
pip install -e ./python
```

### Package Manager Installation

```bash
# Using package managers (platform-specific)
# Debian/Ubuntu
apt install nlm

# Fedora/CentOS
yum install nlm

# For development versions
pip install ./python
```

## Troubleshooting

### Common Issues

#### Python Bindings Not Found

**Symptom:** Python cannot import pynlm

**Solution:**

1. Ensure Python development packages are installed:

   ```bash
   # Ubuntu/Debian
   sudo apt-get install python3-dev python3-pip

   # Fedora/CentOS
   sudo dnf install python3-devel python3-pip

   # macOS
   brew install python
   ```

2. Install pybind11 dependencies:

   ```bash
   pip install pybind11
   ```

3. Rebuild Python bindings:

   ```bash
   cd build
   cmake --build . --target pynlm
   ```

#### Build Configuration Issues

**Symptom:** CMake configuration errors

**Solution:**

1. Ensure minimum CMake version:

   ```bash
   cmake --version
   # Version should be 3.19 or higher
   ```

2. Check for conflicting packages:

   ```bash
   # Clean and rebuild
   rm -rf build
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

3. Verify Python installation:

   ```bash
   python3 --version
   python3-config --includes
   ```

#### Performance Issues

**Symptom:** Slow build times or runtime performance

**Solution:**

1. Enable performance optimizations:

   ```bash
   mkdir build && cd build
   cmake .. -DNLM_ENABLE_PERFORMANCE=ON
   ```

2. Use parallel builds:

   ```bash
   make -j$(nproc)
   ```

3. Consider AddressSanitizer for debugging:

   ```bash
   mkdir build && cd build
   cmake .. -DNLM_ENABLE_ASAN=ON
   ```

### Advanced Troubleshooting

#### Compiler-Specific Issues

**MSVC Issues:**

- Ensure Visual Studio is properly configured
- Use `/permissive-` flag in Python bindings
- Check Windows SDK availability

**GCC/Clang Issues:**

- Ensure C++17/20 support
- Check for missing compiler features
- Verify pthread library availability

#### Cross-Platform Issues

**Windows:**

- Use CMake generators appropriate for your Visual Studio version
- Ensure correct path separators
- Check for Windows-specific dependencies

**macOS:**

- Ensure Xcode command line tools are installed
- Check for Homebrew installations of dependencies
- Verify framework paths

**Linux:**

- Ensure pthread and math libraries are available
- Check for OpenMP support
- Verify system package dependencies

## Advanced Configuration

### Custom CMake Options

Add custom CMake options to the main CMakeLists.txt:

```cmake
# Add custom build options
option(NLM_CUSTOM_FEATURE "Enable custom feature" ON)
option(NLM_DEBUG_OUTPUT "Enable debug output" OFF)

# Use custom options
if(NLM_DEBUG_OUTPUT)
    add_definitions(-DNLM_DEBUG_OUTPUT)
endif()
```

### Dependency Management

#### External Dependencies

NLM supports integration with external dependency managers:

**Conan:**

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/conan_toolchain.cmake
```

**vcpkg:**

```bash
cmake .. -A x64 -T vcpkg
```

**FetchContent:**

```cmake
include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.10.0
)

FetchContent_MakeAvailable(pybind11)
```

### Continuous Integration

#### GitHub Actions Example

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

    - name: Set up CMake
      uses: lukka/get-cmake-action@v1.0

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y libpython3-dev python3-pip
        pip install pybind11 pytest

    - name: Configure
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}

    - name: Build
      run: |
        cmake --build . --config ${{ matrix.build_type }} -- -j4

    - name: Test
      run: |
        ctest --output-on-failure

    - name: Package (optional)
      if: matrix.build_type == 'Release'
      run: |
        cpack --config Release
        cpack --output output.tar.gz

    - uses: actions/upload-artifact@v3
      if: matrix.build_type == 'Release'
      with:
        name: nlm-packages
        path: output.tar.gz
```

#### GitLab CI Example

```yaml
stages:
  - build
  - test
  - package

variables:
  CMAKE_BUILD_TYPE: Release

build_job:
  stage: build
  script:
    - mkdir build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
    - make -j4
  artifacts:
    paths:
      - build/
    expire_in: 1 week

test_job:
  stage: test
  script:
    - cd build
    - ctest --output-on-failure
  dependencies:
    - build_job

package_job:
  stage: package
  script:
    - cd build
    - cpack
  artifacts:
    paths:
      - *.deb
      - *.rpm
      - *.tar.gz
  dependencies:
    - build_job
  only:
    - master
```

## Build Best Practices

### Recommended Build Workflow

1. **Always clean before building:**

   ```bash
   rm -rf build
   ```

2. **Use build types appropriately:**

   - `Debug` for development and debugging
   - `Release` for production builds
   - `RelWithDebInfo` for development with optimizations

3. **Leverage parallel builds:**

   ```bash
   make -j$(nproc)
   ```

4. **Validate after build:**

   ```bash
   # Run tests
   ctest --output-on-failure

   # Check installation
   ./bin/nlm --help
   ```

### Performance Optimization

1. **Enable performance optimizations:**

   ```bash
   cmake .. -DNLM_ENABLE_PERFORMANCE=ON
   ```

2. **Use native optimizations on Linux:**

   ```bash
   cmake .. -DCMAKE_CXX_FLAGS="-march=native"
   ```

3. **Consider link-time optimization (LTO):**

   ```bash
   cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
   ```

### Development Workflow

1. **Debug builds for development:**

   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_ASAN=ON
   ```

2. **Coverage builds for testing:**

   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_COVERAGE=ON
   ```

3. **Multi-configuration generators:**

   ```bash
   cmake -G "Ninja Multi-Config" ..
   cmake --build . --config Debug
   cmake --build . --config Release
   ```

## Conclusion

The NLM build system provides comprehensive configuration options for development, testing, and production environments. With support for multiple build types, comprehensive packaging options, and extensive troubleshooting guidance, developers can efficiently build and deploy NLM across various platforms.

Key improvements implemented:

- **Standardized CMake configuration** with modern CMake practices
- **Enhanced Python bindings** with comprehensive installation support
- **Comprehensive CPack integration** for multiple package formats
- **Robust warning systems** and build type support
- **Platform-specific optimizations** for performance
- **Extensive documentation** and troubleshooting guides
- **Continuous integration** examples and best practices

These improvements make the NLM build system more robust, maintainable, and user-friendly, supporting the full development lifecycle from initial build to production deployment.
