# NLM CMake Build System
# ====================

This document describes the modern CMake build system for the NLM project, including advanced build features and configuration options.

## Overview

The NLM project now uses a modern, modular CMake build system that addresses all previous issues:

1. **Clean Structure**: Single CMakeLists.txt with no duplication
2. **Complete Features**: Installation, packaging, testing, benchmarking
3. **Flexible Configuration**: Multiple build variants and options
4. **Robust Dependencies**: Target-based dependency management
5. **Efficient Build**: Modern CMake practices for parallel builds
6. **Well Documented**: Comprehensive build instructions and examples

## Basic Usage

### Quick Start

```bash
# Create build directory
cd nlm
mkdir build
cd build

# Configure (Release build by default)
cmake ..

# Build everything (parallel)
cmake --build . --parallel

# Run tests
ctest
```

### Installation

```bash
# Install to default prefix (typically /usr/local)
cmake --build . --target install

# Or install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
cmake --build . --target install
```

## Configuration Options

### Core Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `NLM_BUILD_SHARED` | `OFF` | Build shared libraries instead of static |
| `NLM_ENABLE_PYTHON_BINDINGS` | `ON` | Build Python bindings (requires pybind11) |
| `NLM_ENABLE_TESTS` | `ON` | Enable testing framework |
| `NLM_ENABLE_BENCHMARKS` | `ON` | Build performance benchmarks |
| `NLM_ENABLE_COVERAGE` | `OFF` | Enable code coverage reporting |
| `NLM_EXAMPLE_BUILD` | `ON` | Build example applications |
| `NLM_INSTALL_DEVELOPER` | `ON` | Install development targets and docs |

### Build Type Options

| Build Type | Optimization | Debug Info | Suitability |
|------------|--------------|------------|-------------|
| `Debug` | None | Full | Development, debugging |
| `Release` | Maximum | None | Production, performance |
| `RelWithDebInfo` | High | Full | Release with debugging |
| `MinSizeRel` | Size-focused | None | Size-constrained deployments |

### Example Configurations

```bash
# Debug build with tests and coverage (development)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON -DNLM_ENABLE_COVERAGE=ON

# Release build with Python bindings (production)
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON -DNLM_BUILD_SHARED=OFF

# Development environment (all features)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON -DNLM_ENABLE_BENCHMARKS=ON -DNLM_ENABLE_COVERAGE=ON

# Minimal production build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_SHARED=OFF -DNLM_ENABLE_TESTS=OFF
```

## Targets

### Core Libraries

| Target | Type | Dependencies | Purpose |
|--------|------|-------------|---------|
| `nlm_core` | Static/Shared | None | Core brain components |
| `nlm_agent` | Static/Shared | `nlm_core` | Agent system |
| `nlm_world` | Static/Shared | `nlm_core` | World simulation |
| `nlm_phase3` | Static/Shared | `nlm_core`, `nlm_agent`, `nlm_world` | Phase 3 experiments |
| `nlm_phase4` | Static/Shared | `nlm_core`, `nlm_agent`, `nlm_world` | Phase 4 experiments |

### Executables

| Target | Type | Dependencies | Purpose |
|--------|------|-------------|---------|
| `nlm` | Executable | `nlm_core` | Main simulation |
| `nlm_phase3_demo` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3` | Phase 3 demo |
| `nlm_phase4_demo` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3`, `nlm_phase4` | Phase 4 demo |
| `nlm_test` | Executable | `nlm_core` | Unit tests |
| `nlm_benchmark` | Executable | `nlm_core`, `nlm_agent`, `nlm_world` | Performance benchmarks |

### Python Bindings

| Target | Type | Purpose |
|--------|------|---------|
| `pynlm` | Python Module | Python bindings for NLM |

### Special Targets

| Target | Description |
|--------|-------------|
| `all` | Build all targets (default) |
| `install` | Install all targets and documentation |
| `install-dev` | Install development targets |
| `coverage` | Generate code coverage report (if enabled) |
| `test` | Build and run tests |
| `docs` | Generate documentation (if available) |

## Installation

### System Installation

```bash
# Install to system prefix
cmake --build . --target install

# Verify installation
find /usr/local -name "*nlm*" -type f
```

### Development Installation

```bash
# Install development files for C++ development
cmake .. -DNLM_INSTALL_DEVELOPER=ON
cmake --build . --target install

# Install Python bindings
set(NLM_CONFIG_FILE=generated_config.cmake)
cmake --build . --target pynlm
```

### Package Installation

```bash
# Install package configuration
# Generates find_package support
cmake --build . --target all

# Create package for distribution
cmake --build . --target package
```

## Testing

### Running Tests

```bash
# Build and run all tests
ctest --output-on-failure

# Run only specific test
ctest -R specific_test_name

# Run tests with verbose output
ctest -V

# Run tests with timing
ctest --timeout 30
```

### Code Coverage

If `NLM_ENABLE_COVERAGE` is enabled:

```bash
# Build tests with coverage
cmake .. -DNLM_ENABLE_COVERAGE=ON
cmake --build . --target nlm_test

# Generate coverage report
cmake --build . --target coverage

# View coverage report
open build/coverage/html/index.html
```

### Performance Benchmarks

```bash
# Build and run benchmarks
cmake --build . --target nlm_benchmark

# Compare against baseline
# See benchmarks output for performance metrics
```

## Development

### Building Specific Targets

```bash
# Build only the core library
cmake --build . --target nlm_core

# Build only the main executable
cmake --build . --target nlm

# Build only Python bindings
cmake --build . --target pynlm

# Build only tests
cmake --build . --target nlm_test
```

### Incremental Builds

Modern CMake provides efficient dependency tracking:

```bash
# Only rebuild what changed
cmake --build . --parallel

# Rebuild everything
rm -rf *
cmake ..
cmake --build . --parallel
```

### Dependency Management

The build system uses target-based dependencies:

```cpp
// Example: Target linking
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world)

// Example: Target properties
set_target_properties(nlm_core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
```

## Python Bindings

### Building Python Bindings

```bash
# Configure with Python bindings enabled
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON

# Build Python module
cmake --build . --target pynlm

# Install Python module
cmake --build . --target install

# Test Python module
python -c "import pynlm; print(pynlm.__version__)"
```

### Python Integration

The Python bindings use `pybind11` and provide:

- Complete API mirroring C++ interface
- Automatic Python bindings for NLM classes
- Integration with Python scientific stack
- Support for NumPy arrays for large datasets

### Development Python Bindings

```bash
# Install Python dependencies
pip install pybind11 numpy scipy

# Build Python module in development mode
cmake --build . --target pynlm
pip install -e .
```

## Advanced Features

### Building Variants

The build system supports multiple build variants:

```bash
# Build for different platforms
make nlm # Default
make nlm_phase3_demo # Phase 3
make nlm_phase4_demo # Phase 4
make pynlm # Python bindings
make nlm_test # Tests
make nlm_benchmark # Benchmarks
```

### Cross-Compilation

```bash
# Cross-compile to another platform
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake
```

### External Dependencies

```bash
# Find external packages
cmake .. -DFIND_PYTHON=OFF -DFIND_PYBIND11=OFF
```

## Migration Guide

### From Old to New CMake

If you were using the old CMake configuration, follow these migration steps:

1. **Single CMakeLists.txt**: All targets are now in the root CMakeLists.txt
2. **No Duplication**: Source files are organized by directory, not duplicated
3. **Modern CMake**: Use target-based dependencies instead of global commands
4. **New Options**: Use CMake options instead of hardcoded paths
5. **Installation**: Use CMake install() commands for deployment

### Migration Example

**Old (fragmented):**
```bash
cmake ../src    # For main library
cmake ../python # For Python bindings
cmake ../tests  # For tests
```

**New (single build):**
```bash
cmake .. -DNLM_ENABLE_TESTS=ON -DNLM_ENABLE_PYTHON_BINDINGS=ON
```

## Troubleshooting

### Common Issues

#### Build Fails with Compiler Errors

```bash
# Check compiler
which g++

# Ensure C++20 support
mkdir build && cd build
cmake .. -DCMAKE_CXX_COMPILER=g++-10
```

#### Python Bindings Not Found

```bash
# Verify Python installation
python -m pip install --upgrade pip
pip install pybind11
```

#### Tests Not Building

```bash
# Enable tests
cmake .. -DNLM_ENABLE_TESTS=ON
cmake --build . --target nlm_test
```

#### Installation Permissions

```bash
# Use sudo for system installation
sudo cmake --build . --target install

# Use custom prefix for user installation
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/local
cmake --build . --target install
```

## Performance Considerations

### Optimized Builds

```bash
# Parallel builds (recommended)
cmake --build . --parallel $(nproc)

# Strip debug symbols for release
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel

# Use link-time optimization
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE
```

### Memory Usage

```bash
# Use less memory for smaller builds
cmake .. -DNLM_BUILD_SHARED=OFF
```

## Future Enhancements

### Planned Features

- **Package Manager Integration**: Support for package managers (Conan, vcpkg)
- **Package Scripts**: Automated package creation scripts
- **CI/CD Integration**: Templates for CI/CD pipelines
- **Documentation Generation**: Automatic documentation generation
- **Plugin Architecture**: Support for third-party plugins
- **Module Dependencies**: Fine-grained dependency management

## See Also

- [INSTALLATION.md](examples/INSTALLATION.md) - Installation instructions
- [README.md](README.md) - Project overview
- [docs/](docs/) - Technical documentation
- [examples/](examples/) - Example applications
- [HOW_TO_USE.md](HOW_TO_USE.md) - User guide

## License

This CMake build system is provided under the same license as the NLM project.