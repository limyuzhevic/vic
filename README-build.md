# NLM Build System

This document provides comprehensive information about building and installing the Neural Learning Machine (NLM) framework.

## Overview

NLM is a brain simulator that lets you create virtual brains that can learn, remember things, and make decisions. The NLM build system is designed to be modern, unified, and flexible, supporting various build types, package managers, and installation options.

## Quick Start

### Prerequisites

- CMake 3.16 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19.2+)
- Python 3.6+ (for Python bindings)

### Build with Default Settings

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
 cmake --build . -- -j$(nproc)
```

### Build with Build System Script

```bash
python3 build.py --build-type Release
```

### Build with CMakePresets

```bash
mkdir build
cd build
cmake -G "Ninja" -C CMakePresets.json
```

## Build Types

| Build Type | Description | Optimization |
|------------|-------------|--------------|
| Debug | Debug build with symbols and no optimization | None |
| Release | Optimized build for production | Full |
| RelWithDebInfo | Release with debug symbols | Partial |
| MinSizeRel | Optimized for minimal binary size | Full |

## Build Configuration Options

### Using CMake Command Line

```bash
cmake .. \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_BUILD_PYTHON_BINDINGS=ON \
  -DNLM_BUILD_CLI=ON \
  -DNLM_BUILD_DEMOS=ON \
  -DNLM_BUILD_FULL=ON \
  -DNLM_ENABLE_CONAN=ON \
  -DNLM_ENABLE_VCPKG=OFF \
  -DNLM_ENABLE_CPACK=ON
```

### Using Build System Script

```bash
python3 build.py \
  --build-type Release \
  --components all \
  --conan \
  --cpack \
  --install \
  --prefix /usr/local
```

## Package Managers

### Conan

Conan is a C++ package manager that handles dependencies automatically.

#### Install Conan

```bash
curl -L https://raw.githubusercontent.com/conan-io/conan/master/conanfile.py -o conanfile.py
python3 conanfile.py install
```

#### Use Conan with NLM

```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_CONAN=ON
cmake --build .
```

### vcpkg

vcpkg is a package manager for Windows that simplifies dependency management.

#### Install vcpkg

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install nlohmann-json yaml-cpp pybind11
```

#### Use vcpkg with NLM

```bash
set(VCPKG_ROOT=C:\path\to\vcpkg)
mkdir build
cd build
cmake .. -G "Ninja" -A x64 -DVCPKG_ROOT=%VCPKG_ROOT%
cmake --build .
```

## Installation

### Installation Modes

| Mode | Description | Components |
|------|-------------|------------|
| All | Complete installation with all components | Core, Tests, Python, CLI, Demos |
| Core | Minimal installation (core libraries only) | Core |
| Development | Development installation with headers and docs | Core, Headers, Docs |
| Python | Python bindings only | Python bindings |
| Custom | Specific component selection | User-defined |

### Install Core Only

```bash
python3 build.py --components core --install --prefix /usr/local
```

### Install All Components

```bash
python3 build.py --components all --install --prefix /usr/local
```

### Install with Package Manager

```bash
# With Conan
python3 build.py --components all --conan --cpack --install

# With vcpkg
python3 build.py --components all --vcpkg --install
```

## Testing

### Run Tests

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

### Run Tests with Build System

```bash
python3 build.py --components all --test-only
```

## Packaging

### Create DEB Package

```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_CPACK=ON
cmake --build .
cpack --config CPackConfig.cmake
```

### Create RPM Package

```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_CPACK=ON
cmake --build .
cpack --config CPackConfig.cmake
```

### Create TGZ Archive

```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_CPACK=ON -DCPACK_GENERATOR="TGZ"
cmake --build .
cpack --config CPackConfig.cmake
```

## CI/CD Integration

### GitHub Actions

Create `.github/workflows/build.yml`:

```yaml
name: NLM Build and Test

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake ninja-build
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: |
        cd build
        cmake --build . -- -j$(nproc)
    
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Package
      if: matrix.os == 'ubuntu-latest'
      run: |
        cd build
        cpack --config CPackConfig.cmake
```

### GitLab CI

Create `.gitlab-ci.yml`:

```yaml
stages:
  - build
  - test
  - package

variables:
  CMAKE_GENERATOR: "Ninja"
  BUILD_TYPE: "Release"

build_job:
  stage: build
  script:
    - mkdir build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    - cmake --build .
  artifacts:
    paths:
      - build/

 test_job:
  stage: test
  script:
    - cd build
    - ctest --output-on-failure
  needs:
    - build_job

package_job:
  stage: package
  script:
    - cd build
    - cpack --config CPackConfig.cmake
  artifacts:
    paths:
      - package/
  needs:
    - build_job
    - test_job
```

## Troubleshooting

### Common Issues

#### CMake cannot find required packages

**Solution:** Use package managers (Conan or vcpkg) or install dependencies manually:

```bash
# Install dependencies manually
sudo apt-get install libnlohmann-json-dev libyaml-cpp-dev python3-dev
```

#### Build fails with compiler errors

**Solution:** Ensure you have a C++20 compatible compiler:

```bash
# Check compiler version
g++ --version
# Should be GCC 9+ or Clang 10+
```

#### Installation permissions

**Solution:** Use sudo for system-wide installation or specify a user directory:

```bash
# User installation
python3 build.py --install --prefix ~/.local

# System installation (requires sudo)
sudo python3 build.py --install --prefix /usr/local
```

### Build Time Reporting

The build system automatically reports build time. For more detailed information, check the build log file.

## Build Performance Optimization

### Parallel Build

Use parallel build to speed up compilation:

```bash
# With Unix Makefiles
cmake --build . -- -j$(nproc)

# With Ninja
cmake --build . -j$(nproc)
```

### Cache Build Artifacts

Use CMake cache to speed up repeated builds:

```bash
# First build
cmake configure

# Subsequent builds (incremental)
cmake build
```

### Use Pre-built Dependencies

Use package managers to speed up dependency installation:

```bash
# With Conan
curl -L https://raw.githubusercontent.com/conan-io/conan/master/conanfile.py -o conanfile.py
python3 conanfile.py install

# With vcpkg
./vcpkg integrate install
./vcpkg install
```

## Migration Guide

### From Old CMake Structure

The old build system had multiple CMakeLists.txt files:

- `CMakeLists.txt` (main)
- `tests/CMakeLists.txt`
- `python/CMakeLists.txt`

The new unified build system consolidates all configuration into a single `CMakeLists.txt` file with comprehensive configuration options.

### Migration Steps

1. Clean up old build directory:

```bash
rm -rf build
```

2. Create new build directory:

```bash
mkdir build
cd build
```

3. Configure with new build system:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_TESTS=ON -DNLM_BUILD_PYTHON_BINDINGS=ON
```

4. Build:

```bash
cmake --build .
```

## Files Created

- `CMakeLists.txt` - Main CMake configuration
- `CMakePresets.json` - Build presets for different use cases
- `CPackConfig.cmake` - Package configuration for CPack
- `conanfile.py` - Conan package configuration
- `vcpkg.json` - vcpkg port configuration
- `build.py` - Python build automation script
- `install.sh` - Installation script
- `README-build.md` - This documentation file
- `INSTALLATION.md` - Installation documentation
- `CONTRIBUTING.md` - Contribution guide
- `BUILD.md` - Build troubleshooting guide

## Key Features

1. **Unified Build System**: Single CMakeLists.txt with comprehensive configuration
2. **Multiple Build Types**: Debug, Release, RelWithDebInfo, MinSizeRel
3. **Package Manager Support**: Conan, vcpkg, CPack
4. **Flexible Installation**: Multiple installation modes
5. **Build Automation**: Python build script for easy automation
6. **CI/CD Integration**: Templates for GitHub Actions and GitLab CI
7. **Performance Optimization**: Parallel build, caching, dependency management
8. **Cross-Platform**: Support for Windows, Linux, and macOS
9. **Comprehensive Testing**: Integrated test support
10. **Documentation**: Extensive documentation and examples

## Conclusion

The NLM build system provides a modern, unified, and flexible approach to building and installing the Neural Learning Machine framework. It supports multiple build configurations, package managers, and installation modes, making it suitable for development, testing, and production environments.

For more information, refer to the individual documentation files:

- `INSTALLATION.md` - Detailed installation instructions
- `CONTRIBUTING.md` - Contribution guidelines
- `BUILD.md` - Troubleshooting guide

---

*Last updated: $(date -u +%Y-%m-%d)*
