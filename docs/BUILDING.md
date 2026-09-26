# BUILDING.md

# NLM Build Instructions

This document provides detailed instructions for building the NLM project from source.

## Prerequisites

### C++ Development Environment
- **Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Make**: Version 4.1 or higher (Unix-like systems)
- **Ninja**: Alternative build system (recommended for faster builds)

### Python Environment (for Python bindings)
- **Python**: 3.8+ (3.10+ recommended)
- **pip**: Python package installer
- **build tools**: setuptools, wheel, scikit-build-core

### Optional Dependencies
- **Doxygen**: For API documentation generation
- **Valgrind**: For memory leak detection (Linux/macOS)
- **AddressSanitizer**: For memory debugging

## Building the C++ Project

### Standard CMake Build

```bash
# Navigate to the project root
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Recommended Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Fast parallel build using Ninja
cmake .. -GNinja
cmake --build . --config Release --parallel

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Enable memory debugging (Linux/macOS)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Outputs

After successful compilation, the following targets will be created:

| Target | Type | Location | Description |
|--------|------|----------|-------------|
| `nlm` | Executable | `build/nlm` | Main simulation executable |
| `nlm_core` | Static Library | `build/libnlm_core.a` | Core brain components |
| `nlm_agent` | Static Library | `build/libnlm_agent.a` | Agent system |
| `nlm_world` | Static Library | `build/libnlm_world.a` | World simulation |
| `nlm_phase3_demo` | Executable | `build/nlm_phase3_demo` | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | `build/nlm_phase4_demo` | Phase 4 demonstration |
| `nlm_test` | Executable | `build/nlm_test` | Unit test suite |
| `nlm_phase6_demo` | Executable | `build/nlm_phase6_demo` | Phase 6 integration demo |
| `pynlm` | Python Module | `pynlm/` | Python bindings |

## Building Python Bindings

### From Source (Development Setup)

```bash
# Install Python build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build Python module in development mode
cd /path/to/nlm
pip install -e .

# Or build and install normally
pip install .

# Build only (without installing)
pip install build
python -m build

# Install from wheel
pip install dist/*.whl
```

### Using pyproject.toml

The project uses a modern `pyproject.toml` configuration:

```toml
[build-system]
requires = ["scikit-build-core", "pybind11>=2.11.0", "numpy>=1.18"]
build-backend = "scikit-build-core"

[tool.scikit-build]
cmake.minimum-version = "3.16"
wheel.cmake = true
wheel.cmake.args = ["-DCMAKE_BUILD_TYPE=Release"]
```

## Common Build Issues and Solutions

### CMake Cannot Find Python

```bash
# Install Python development packages
# Ubuntu/Debian:
sudo apt-get update
sudo apt-get install python3-dev python3-pip python3-venv

# macOS:
brew install python3

# Reconfigure with explicit Python path
cmake .. -DPython_EXECUTABLE=$(which python3)
```

### Missing Dependencies

```bash
# For Ubuntu/Debian:
sudo apt-get install build-essential cmake ninja-build \
    libstdc++-11-dev python3-dev python3-pip \
    python3-venv valgrind doxygen

# For macOS:
brew install cmake ninja python3 doxygen
```

### Compilation Errors

**Common causes and solutions:**

1. **C++20 feature not supported:**
   ```bash
   # Use a newer compiler version
   cmake .. -DCMAKE_CXX_COMPILER=g++13
   ```

2. **Memory allocation issues:**
   ```bash
   # Use AddressSanitizer for debugging
   cmake .. -DENABLE_ASAN=ON
   ```

3. **Thread-related issues:**
   ```bash
   # Check for available CPU cores
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

## Build Performance Optimization

### Recommended Toolchain

```bash
# Use Ninja for faster builds
cmake .. -GNinja
cmake --build . --parallel

# Use ccache to speed up incremental builds
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Build Scripts

Create a `build.sh` script for quick builds:

```bash
#!/bin/bash

cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja

# Build
if command -v cmake >/dev/null 2>&1; then
    cmake --build . --parallel
else
    make -j$(nproc)
fi

# Run tests (optional)
# ctest --output-on-failure
```

## Troubleshooting

### Build Fails with Out-of-Memory Errors

```bash
# Increase swap space or use a machine with more RAM
# Or build with less parallelism
make -j2
```

### CMake Configuration Issues

```bash
# Clean build
cd build
rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Python Bindings Not Working

```bash
# Check Python path
python -c "import sys; print(sys.path)"

# Reinstall Python module
pip uninstall pynlm
pip install -e .

# Check for symbol conflicts
ldd $(python -c "import pynlm; print(pynlm.__file__.replace('__init__.py', 'nlm') or '')") 2>/dev/null || echo "Could not find native module"
```

## Continuous Integration Setup

### GitHub Actions Workflow

Create `.github/workflows/build.yml`:

```yaml
name: NLM Build and Test

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        build_type: [Release, Debug]

    steps:
    - uses: actions/checkout@v3

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.10'

    - name: Install dependencies
      run: |
        pip install pybind11 scikit-build-core pytest numpy

    - name: Configure CMake
      run: cmake -B ${{ github.workspace }}/build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}

    - name: Build
      run: cmake --build ${{ github.workspace }}/build --parallel

    - name: Run tests
      run: ctest --output-on-failure
      working-directory: ${{ github.workspace }}/build
```

## Quick Reference

### Build Commands

| Goal | Command |
|------|---------|
| Quick debug build | `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j$(nproc)` |
| Quick release build | `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)` |
| Build Python bindings | `pip install -e .` |
| Run tests | `cd build && ctest --output-on-failure` |
| Install system-wide | `cd build && sudo make install` |

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |
| `PYTHON_EXECUTABLE` | system default | Python interpreter to use for bindings |
| `ENABLE_TESTS` | `ON` | Enable/disable unit tests |
| `ENABLE_ASAN` | `OFF` | Enable AddressSanitizer (debug builds only) |
| `NUMPY_INCLUDE_DIR` | auto-detected | Path to NumPy headers |

## Support

For build issues, refer to the project's issue tracker or consult the existing documentation. The build system is designed to be straightforward, but complex environments may require custom configurations.

---

*Last updated: $(date +%Y-%m-%d)*