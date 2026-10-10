# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source with cross-platform support
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Cross-Platform Build Configuration

The build system automatically detects the platform and configures itself accordingly:

#### Platform Detection
- **Linux**: GCC/Clang with SIMD optimizations
- **macOS**: Clang with libc++ and SIMD optimizations
- **Windows**: MSVC with AVX2 optimizations

#### Build Options

Available CMake options:

- `-DNLM_ENABLE_OPENMP=ON/OFF` - Enable/disable OpenMP parallelism (default: ON)
- `-DNLM_ENABLE_SIMD=ON/OFF` - Enable/disable SIMD optimizations (default: ON)
- `-DNLM_ENABLE_AVX2=ON/OFF` - Enable/disable AVX2 instruction set (default: ON)
- `-DNLM_ENABLE_AVX512=ON/OFF` - Enable/disable AVX512 instruction set (default: OFF)
- `-DNLM_ENABLE_THREADS=ON/OFF` - Use C++ threads instead of OpenMP (default: OFF)
- `-DNLM_BUILD_TESTS=ON/OFF` - Build test executables (default: ON)
- `-DNLM_BUILD_BENCHMARKS=ON/OFF` - Build performance benchmarks (default: ON)
- `-DNLM_BUILD_OPTIMIZATION_TESTS=ON/OFF` - Build optimization verification tests (default: ON)
- `-DNLM_BUILD_PACKAGE=ON/OFF` - Build package distribution files (default: OFF)

#### Compiler-Specific Configuration

**GCC/Clang:**
- Auto-detect best available SIMD instructions
- Use `-march=native` for maximum performance
- Support for LTO (Link Time Optimization)

**MSVC:**
- Auto-enable AVX2 optimizations
- Use `/O2 /Oi /Gy /GF` for Release builds
- Windows-specific linking with export-all-symbols

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `nlm_benchmark` | Executable | Performance benchmark |
| `nlm_optimization_tests` | Executable | Optimization verification |

### Build Options (Advanced)

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_LOG_LEVEL_DEBUG=ON

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build with minimal features (no SIMD, no OpenMP)
cmake .. -DNLM_ENABLE_SIMD=OFF -DNLM_ENABLE_OPENMP=OFF

# Build for production (maximum optimizations)
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNLM_ENABLE_SIMD=ON -DNLM_ENABLE_AVX2=ON -DNLM_ENABLE_AVX512=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build package distribution (RPM/DEB)
cmake .. -DNLM_BUILD_PACKAGE=ON
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

[Content from original file continues here...]

---

## Part 4: Cross-Platform Configuration Options

### Platform-Specific Python Package Configuration

The Python package automatically adapts to the platform:

#### Linux/macOS
- Uses system Python with standard library
- Compiles with `-march=native` for maximum performance
- Uses pthreads for threading

#### Windows
- Uses MSVC compiler with AVX2 optimizations
- Uses Windows native threading model
- Enables `/permissive-` for stricter C++ compliance

#### Cross-Platform Features

The build system detects platform capabilities and enables:

- **SIMD Support**: Automatic detection and utilization of available instruction sets (SSE2, AVX2, AVX512)
- **Threading**: OpenMP on Linux/macOS, Windows native threading on Windows
- **Memory Management**: Platform-specific optimizations for memory allocation and management
- **Linker Flags**: Platform-specific optimization flags for executables and libraries

### Build Environment Variables

```bash
# Set compiler flags for specific platforms
export CXXFLAGS="-O3 -march=native"

# Enable verbose build output
export CMAKE_VERBOSE_MAKEFILE=1

# Set Python executable path
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3

# Build with development dependencies
pip install -e .[dev]
```

---

## Part 5: Testing and Validation

### Running Tests

```bash
# Run all tests
make test_all

# Run CTest directly
ctest --output-on-failure

# Run optimization tests specifically
./nlm_optimization_tests

# Run benchmarks
./nlm_benchmark
```

### Cross-Platform Testing

The test system automatically adapts to the platform:

- **Unit Tests**: Compile and run on all platforms
- **Integration Tests**: Platform-aware environment configuration
- **Performance Benchmarks**: Platform-optimized for fair comparison
- **Coverage Analysis**: GCC/Clang supported (lldb on macOS)

### CI/CD Integration

The build system is designed for continuous integration:

```yaml
# Example GitHub Actions workflow
name: Build and Test
on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
        python-version: ["3.8", "3.9", "3.10", "3.11", "3.12"]
    
    steps:
    - uses: actions/checkout@v3
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install pybind11 scikit-build-core pytest numpy
    
    - name: Build and install
      run: |
        pip install .
    
    - name: Run tests
      run: |
        ctest --output-on-failure
```

---

## Part 6: Advanced Configuration

### Environment Configuration

The NLM project supports multiple configuration files:

#### config.json
- Default configuration for the brain simulation
- Can be overridden via command line

#### .env (for development)
- Environment variables for build configuration
- Compiler flags and optimization settings

#### CMake options in CMakeLists.txt
- Comprehensive build configuration
- Platform-specific adjustments

### Performance Tuning

#### SIMD Configuration

The build system automatically detects available SIMD instructions:

- **SSE2**: Basic x86 support (always enabled on modern processors)
- **AVX2**: Advanced Vector Extensions 2 (default on Linux/macOS)
- **AVX512**: Advanced Vector Extensions 512 (optional, disabled by default)

#### Threading Configuration

Choose the best threading model for your platform:

- **OpenMP**: Multi-threading standard (default on Linux/macOS)
- **C++ Threads**: Standard library threading (alternative)
- **No Threading**: Single-threaded for deterministic behavior

### Troubleshooting

#### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Check for missing dependencies

**Platform-specific issues:**
- macOS: Use `-stdlib=libc++` flag
- Windows: Ensure MSVC compiler version compatibility
- Linux: Install required development packages

#### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
