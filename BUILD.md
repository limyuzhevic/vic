# Building NLM

This document describes how to build the NLM (熙然) project with various build-time configuration options.

## Quick Build

The simplest way to build NLM:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Build Configuration Options

NLM supports several build-time configuration options to customize the build:

### Standard CMake Options

```bash
# Configure with different options
cmake .. -DCMAKE_BUILD_TYPE=Debug     # Debug build with symbols and assertions
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo  # Release build with debug info
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel  # Optimized for size
```

### NLM-Specific Features

```bash
# Enable/disable specific features
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON    # Enable Python bindings (default)
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=OFF   # Disable Python bindings
cmake .. -DNLM_ENABLE_VISUALIZATION=ON      # Enable visualization module
cmake .. -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON  # Enable performance optimizations (default)
cmake .. -DNLM_ENABLE_LOGGING=ON            # Enable logging (default)
```

### Performance-Focused Build

For maximum performance:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
         -DNLM_ENABLE_LOGGING=OFF \
         -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"
make -j$(nproc)
```

### Development Build

For development and debugging:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=OFF \
         -DNLM_ENABLE_LOGGING=ON \
         -DCMAKE_CXX_FLAGS="-g -O0 -Wall -Wextra"
make -j$(nproc)
```

## Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test (NEW) |

## Python Bindings

The project includes Python bindings via pybind11. To build Python bindings:

```bash
# Install Python dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python module
pip install -e .

# Or build as wheel
python -m build
pip install dist/*.whl
```

## Running Tests

After building, you can run the test suite:

```bash
# Run unit tests
./nlm_test

# Or run with more verbose output
cd build
ctest --output-on-failure
```

## Phase 6 Integration Demo

The Phase 6 integration demo is now included as a separate executable:

```bash
# Run the Phase 6 integration demo
./nlm_phase6_demo

# The demo tests:
# - Integration verification of all systems
# - Memory systems integration
# - Neuromodulation integration
# - Checkpoint save/load functionality
# - Replay system functionality
```

## Performance Testing

For performance testing and benchmarking:

```bash
# Build with performance monitoring
cmake .. -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON -DNLM_ENABLE_LOGGING=ON
make nlm_phase6_demo

# Run with different configurations
./nlm_phase6_demo --help
```

## Installation

To install NLM system-wide:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

This will install:
- Libraries to `/usr/local/lib`
- Headers to `/usr/local/include`
- Executables to `/usr/local/bin`

## Build Script Examples

### Complete Build Script

```bash
#!/bin/bash
set -e

echo "Building NLM with all optimizations..."
mkdir -p build
cd build

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native"

make -j$(nproc)

echo "Build complete!"
echo "Available executables:"
ls -la *.nlm* 2>/dev/null || echo "No NLM executables found"
```

### Python Development Build

```bash
#!/bin/bash
set -e

echo "Building NLM Python bindings..."
pip install scikit-build-core pybind11 pytest numpy
pip install -e .

echo "Python bindings built successfully!"
```

## Troubleshooting

### "CMake cannot find Python"

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

### "Compilation errors"

- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

### "Segmentation faults"

- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify all required configuration parameters are set

## Build Time Optimization

### For CI/CD Pipelines

```bash
# Minimal build for CI
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=ON
make -j4
```

### For Embedded Systems

```bash
# Reduced build for embedded
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel \
         -DNLM_ENABLE_PERFORMANCE_OPTIMIZATIONS=OFF \
         -DNLM_ENABLE_VISUALIZATION=OFF \
         -DNLM_ENABLE_PYTHON_BINDINGS=OFF
make -j2
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build-type: [Release, Debug]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build-type }}
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run Tests
      run: |
        cd build
        ./nlm_test
```

## Advanced Build Configuration

### Custom Build Directory

```bash
mkdir custom_build
cd custom_build
cmake ../src -DCMAKE_BUILD_TYPE=Release
make
```

### Cross-Compilation

For cross-compilation to other platforms:

```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake
make
```

## Conclusion

This document provides comprehensive guidance for building the NLM project with various configuration options. Choose the build configuration that best fits your use case, whether it's maximum performance, development debugging, or integration with Python bindings.