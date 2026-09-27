# NLM CMake Modernization

This document describes the modernized CMake build system for the Neural Learning Machine (NLM) project.

## Overview

The CMake build system has been completely modernized to follow best practices and provide a clean, maintainable architecture. The new system features:

### Key Improvements

1. **Layered Architecture**: The project is organized into logical layers (Core, Neural, Plasticity, Memory, Neuromodulation, Development, Cognition, Sensory, Prediction, Motor, Environment, Experiment, Agent, Application)

2. **Modern CMake Features**:
   - Target-based dependency management
   - Proper interface library exports
   - Consistent naming conventions
   - Build configuration options for Debug, Release, RelWithDebInfo, MinSizeRel
   - CPack integration for package generation

3. **Enhanced Build System**:
   - Automatic output directory configuration
   - Compiler warning flags
   - Feature options (tests, docs, profiling, etc.)
   - Documentation generation support
   - Profiling support

## Build Configuration

### Build Types

The project supports standard CMake build types:

- **Debug**: With symbols and assertions, slower but safe
- **Release**: Optimized, no debug info
- **RelWithDebInfo**: Optimized with debug info
- **MinSizeRel**: Minimum size, optimized

### Feature Options

The following CMake options can be configured:

```bash
-DNLM_ENABLE_DOCS=ON/OFF        Enable Doxygen documentation generation
-DNLM_ENABLE_PROFILING=ON/OFF    Enable profiling support
-DNLM_ENABLE_TESTS=ON/OFF       Enable unit tests
-DNLM_ENABLE_PERFORMANCE=ON/OFF  Enable performance optimizations
-DNLM_ENABLE_MPI=OFF            Enable MPI support
-DNLM_ENABLE_GPU=OFF            Enable GPU acceleration
```

## Build Instructions

### Standard Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build with Tests

```bash
mkdir build
cd build
cmake -DNLM_ENABLE_TESTS=ON ..
cmake --build .
```

### Install

```bash
mkdir build
cd build
cmake ..
cmake --build .
cmake --install .
```

### Package Generation

```bash
mkdir build
cd build
cmake -DCPACK_GENERATOR="TGZ;DEB;RPM" ..
cmake --build .
cpack
```

## Installation

The project installs the following components:

### Libraries

- `libnlm_core`: Core types and utilities
- `libnlm_neural`: Neural network components
- `libnlm_plasticity`: Synaptic plasticity mechanisms
- `libnlm_memory`: Memory systems
- `libnlm_neuromodulation`: Neuromodulation signals
- `libnlm_development`: Neural development
- `libnlm_cognition`: Cognitive functions
- `libnlm_sensory`: Sensory processing
- `libnlm_prediction`: Prediction and inference
- `libnlm_motor`: Motor control
- `libnlm_environment`: Environment simulation
- `libnlm_experiment`: Experimental systems
- `libnlm_agent`: Agent systems

### Headers

Headers are installed to `include/` with subdirectories for each layer.

### Executables

- `nlm_main`: Main application
- `nlm_phase3_demo`: Phase 3 demonstration
- `nlm_phase4_demo`: Phase 4 demonstration
- `nlm_phase5_demo`: Phase 5 demonstration
- `nlm_phase6_demo`: Phase 6 demonstration
- `nlm_benchmark`: Benchmarking tool

## Testing

Unit tests are built and run using CMake's test framework:

### Run Tests

```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest
```

### Test Outputs

Test results are stored in the `bin/tests/` directory.

## Documentation

### API Documentation

Doxygen documentation can be generated with:

```bash
mkdir build
cd build
cmake ..
cmake --build . docs
```

Generated documentation is available in `build/docs/html/`.

### Profiling

Profiling support is enabled by default. To generate profiling data:

```bash
mkdir build
cd build
cmake ..
cmake --build . nlm_main
./bin/nlm_main
# Run with: gprof nlm_main.out gmon.out > profile.txt
```

## Package Management

The project includes CPack integration for generating packages:

### Package Types

- **TGZ**: Source package
- **DEB**: Debian package
- **RPM**: Red Hat package

### Package Generation

See the "Package Generation" section above.

## Customization

### Adding New Layers

To add a new layer, create a new subdirectory under `src/` with a CMakeLists.txt file that defines the targets for that layer.

### Adding Dependencies

Use `target_link_libraries()` to specify dependencies between targets:

```cmake
target_link_libraries(layer3_target PRIVATE layer2_target layer1_target)
```

### Modifying Build Options

Modify CMake options in the main CMakeLists.txt or pass them to CMake:

```bash
cmake -DNLM_ENABLE_GPU=ON ..
```

## Troubleshooting

### Compiler Errors

If you encounter compiler errors, ensure you're using a C++20 compatible compiler:

```bash
g++ --version
```

### Linker Errors

If you get linker errors about missing symbols, check that all dependent libraries are properly linked in the target_link_libraries() calls.

### Installation Issues

If installation fails due to permissions, try installing to a local directory:

```bash
cmake --install . --prefix ./install
```

## Future Improvements

1. **Modern CMake Features**: Consider using `target_sources()` and `target_compile_features()` for better target management.

2. **Dependency Management**: Add support for external dependencies (BLAS, LAPACK, etc.)

3. **Build Variants**: Support for different build variants (e.g., static/shared libraries)

4. **CUDA Support**: Add support for GPU acceleration with CUDA or HIP.

5. **Cross-compilation**: Add support for cross-compilation targets.

## License

This CMake modernization is licensed under the same terms as the NLM project.
