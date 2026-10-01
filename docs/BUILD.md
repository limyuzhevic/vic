# Modern CMake Build System for NLM

## Overview
This document provides comprehensive documentation for the modern CMake build system for NLM (熙然), an experimental artificial developmental brain project.

## Project Structure
```
NLM/
├── CMakeLists.txt                    # Main CMake configuration
├── docs/
│   ├── BUILD.md                     # Build documentation
│   ├── CI_CD.md                     # CI/CD integration
│   └── PERFORMANCE.md                # Performance benchmarks
├── component_groups/                 # Interface libraries and component groups
│   ├── core.cmake                   # Core utilities
│   ├── brain.cmake                  # Neural components
│   ├── cognition.cmake              # Cognitive systems
│   ├── memory.cmake                 # Memory systems
│   ├── sensory.cmake                # Sensory systems
│   ├── motor.cmake                  # Motor systems
│   └── visualization.cmake          # Visualization
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── cognition/      # Cognitive systems
│   ├── memory/         # Memory systems
│   ├── sensory/        # Sensory systems
│   ├── motor/          # Motor systems
│   ├── visualization/  # Visualization
│   └── agent/          # Agent systems
├── tests/CMakeLists.txt              # Testing configuration
├── python/CMakeLists.txt              # Python bindings configuration
└── build/                           # Build directory (not committed)
```

## Build Options

### Core Build Options
- `NLM_BUILD_CORE`: Build core utilities (default: ON)
- `NLM_BUILD_BRAIN`: Build neural brain components (default: ON)
- `NLM_BUILD_COGNITION`: Build cognitive systems (default: ON)
- `NLM_BUILD_MEMORY`: Build memory systems (default: ON)
- `NLM_BUILD_SENSORY`: Build sensory systems (default: ON)
- `NLM_BUILD_MOTOR`: Build motor systems (default: ON)
- `NLM_BUILD_VISUALIZATION`: Build visualization components (default: ON)
- `NLM_BUILD_AGENT`: Build agent systems (default: ON)
- `NLM_BUILD_WORLD`: Build world simulation systems (default: ON)
- `NLM_BUILD_EXPERIMENTS`: Build experiment systems (default: ON)

### Python Bindings
- `NLM_BUILD_PYTHON_BINDINGS`: Build Python bindings (default: ON)
- `NLM_PYTHON_EXECUTABLE`: Path to Python interpreter (default: system Python)
- `NLM_PYTHON_VERSION`: Python version requirement (default: 3.8)

### Testing
- `NLM_BUILD_TESTS`: Build test suite (default: ON)
- `NLM_BUILD_BENCHMARKS`: Build performance benchmarks (default: ON)
- `NLM_ENABLE_COVERAGE`: Enable code coverage reporting (default: OFF)
- `NLM_ENABLE_SANITIZERS`: Enable address/memory sanitizers (default: OFF)

### Optimization and Performance
- `NLM_OPTIMIZATION_LEVEL`: Optimization level (0, 1, 2, 3) (default: 2)
- `NLM_ENABLE_PROFILING`: Enable profiling support (default: OFF)
- `NLM_ENABLE_ASSERTIONS`: Enable debug assertions (default: OFF in Release)

## Quick Start

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build with Python Bindings
```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j4
```

### Build with Tests
```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_TESTS=ON -DNLM_ENABLE_COVERAGE=ON
make -j4
```

### Build with Sanitizers
```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_SANITIZERS=ON
make -j4
```

## Installation

### Standard Installation
```bash
cd build
cmake --install . --prefix /usr/local
```

### Installation with Python Bindings
```bash
cd build
cmake --install . --prefix /usr/local \
  -DNLM_BUILD_PYTHON_BINDINGS=ON
```

## Running Tests

### Run Full Test Suite
```bash
cd build
ctest --output-on-failure
```

### Run Specific Test Categories
```bash
cd build
ctest -R "test_*" --output-on-failure
cd build
ctest -R "benchmark" --output-on-failure
```

## Performance Benchmarking

### Run Performance Benchmarks
```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_BENCHMARKS=ON
cmake --build . --target nlm_benchmark
./nlm_benchmark --results results.json
```

### Generate Performance Reports
```bash
# Install required Python packages
pip install matplotlib pandas seaborn

# Run analysis script
cd build
python scripts/analyze_benchmarks.py
```

## Code Coverage

### Generate Coverage Reports
```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_COVERAGE=ON
make -j4
cd build
make coverage_report
```

### Visualize Coverage
```bash
# View HTML coverage report
cd build
cat coverage_html/index.html
```

## Profiling

### Generate Performance Profiles
```bash
mkdir build
cd build
cmake .. -DNLM_ENABLE_PROFILING=ON
make -j4
cd build
./nlm --profile profile.dat

# Generate flame graph
python scripts/generate_flame_graph.py profile.dat
```

## CI/CD Integration

### GitHub Actions Configuration
```yaml
name: NLM CI/CD
on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
        include:
          - build_type: Debug
            optimizations: -O0
          - build_type: Release
            optimizations: -O3

    steps:
      - uses: actions/checkout@v3
      - name: Set up CMake
        uses: lukka/get-cmake@v3.5

      - name: Configure
        run: |
          mkdir build
          cd build
          cmake .. \
            -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
            -DNLM_BUILD_TESTS=ON \
            -DNLM_ENABLE_COVERAGE=ON \
            -DNLM_BUILD_PYTHON_BINDINGS=ON

      - name: Build
        run: cd build && make -j4

      - name: Test
        run: cd build && ctest --output-on-failure

      - name: Coverage Report
        run: cd build && make coverage_report
        if: matrix.build_type == 'Debug'

      - name: Python Bindings Test
        run: |
          cd build
          python -c "import pynlm"
        if: always()
```

### GitLab CI Configuration
```yaml
variables:
  CMAKE_GENERATOR: "Unix Makefiles"
  CMAKE_BUILD_TYPE: "Release"
  NLM_BUILD_TESTS: "ON"
  NLM_ENABLE_COVERAGE: "OFF"
  NLM_BUILD_PYTHON_BINDINGS: "ON"

stages:
  - build
  - test
  - coverage

build_job:
  stage: build
  script:
    - mkdir build
    - cd build
    - cmake .. \
        -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
        -DNLM_BUILD_TESTS=$NLM_BUILD_TESTS \
        -DNLM_ENABLE_COVERAGE=$NLM_ENABLE_COVERAGE \
        -DNLM_BUILD_PYTHON_BINDINGS=$NLM_BUILD_PYTHON_BINDINGS
    - make -j$(nproc)
  artifacts:
    paths:
      - build/

.test_job:
  stage: test
  script:
    - cd build
    - ctest --output-on-failure
  needs:
    - build_job

coverage_job:
  stage: coverage
  script:
    - cd build
    - make coverage_report
    - cp coverage_html/* $CI_PROJECT_DIR/coverage/
  artifacts:
    paths:
      - coverage/
  needs:
    - build_job
    - .test_job
  only:
    - main
```

## Advanced Configuration

### Cross-Compilation
```bash
mkdir build
cd build
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
  -DCMAKE_C_COMPILER=arm-none-eabi-gcc \
  -DCMAKE_CXX_COMPILER=arm-none-eabi-g++
```

### External Dependencies
```bash
mkdir build
cd build
cmake .. \
  -DNLM_USE_EXTERNAL_PYBIND11=ON \
  -DPYBIND11_ROOT=/custom/path \
  -DNLM_USE_EXTERNAL_GOOGLETEST=ON \
  -DGTEST_ROOT=/custom/path
```

### Multiconfig Generators (Visual Studio, Xcode)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Troubleshooting

### Common Issues

**Issue: Python bindings not found**
```bash
# Install Python development packages
apt-get install python3-dev python3-pip

# Install pybind11
cd build
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3
```

**Issue: CMake version too old**
```bash
# Update CMake to version 3.16 or higher
cd build
cmake .. -DCMAKE_POLICY_DEFAULT_CMP0074=NEW
```

**Issue: Memory errors during build**
```bash
# Clean build
cd ..
rm -rf build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

## Performance Tips

### Parallel Build
```bash
# Optimal thread count (adjust based on your system)
export MAKEFLAGS="-j$(nproc)"
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Incremental Builds
```bash
# Only rebuild changed components
cd build
cmake --build . --target nlm_core
```

### Cache Management
```bash
# Clean CMake cache
cd build
cmake --build . --target clean
cd ..
rm -rf build
```

## Migration Guide

### From Old CMakeLists.txt

The new CMake build system introduces several improvements over the old basic configuration:

1. **Target-based dependencies**: Instead of global properties, use `target_link_libraries()` for clean dependency management
2. **Component libraries**: Each subsystem is a separate target for better modularity
3. **Generator expressions**: Platform-specific configurations using `$<...>` expressions
4. **Better test integration**: Using `enable_testing()` and `add_test()` for proper test management
5. **Enhanced Python bindings**: Using `pybind11_add_module()` for cleaner bindings
6. **Configuration options**: Comprehensive build options for feature selection
7. **Documentation**: Comprehensive build documentation and CI/CD integration

## Resources

### Documentation
- [CMake Best Practices](https://cmake.org/cmake/help/latest/guide/index.html)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/index.html)
- [pybind11 Documentation](https://pybind11.readthedocs.io/)

### Community
- GitHub Discussions
- Stack Overflow (tagged with [cmake] [pybind11])
- Project Wiki

### Additional Tools
- [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) - Test runner
- [CPack](https://cmake.org/cmake/help/latest/module/CPack.html) - Packaging
- [Doxygen](https://www.doxygen.org/) - Documentation generation
- [Valgrind](https://valgrind.org/) - Memory debugging
- [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) - Memory error detection

## License
This build system documentation is provided under the same license as the NLM project.
