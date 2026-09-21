# NLM Build System Enhancement

This document provides enhanced CMake build system configuration for the NLM (Neural Learning Machine) project.

## Enhanced CMake Features

### 1. Python Bindings Support
Add options to build Python bindings using pybind11. The Python module can be built as part of the main CMake build or as a separate Python package.

### 2. Build Configurations
Support multiple build types:
- Debug
- Release
- RelWithDebInfo
- MinSizeRel

### 3. Install Targets
Comprehensive install targets for both C++ libraries and Python packages, with proper configuration for package management.

### 4. Python Binding Tests
Integrated test targets for Python bindings using pytest and CTest.

### 5. Documentation Generation
Doxygen-based documentation generation for C++ API documentation.

### 6. CTest Integration
Full CTest integration with test discovery and execution.

### 7. CMake Options
Extensive CMake options for configuring the build, including:
- Enable/disable Python bindings
- Control documentation generation
- Set build configurations
- Specify installation paths

### 8. Project Configuration
Modern CMake project configuration with:
- Package configuration files
- Dependency management
- Cross-platform support
- Compiler feature detection

## Build Commands

### Basic Build
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

### Build with Python Bindings
```bash
mkdir build
cd build
cmake -DBUILD_PYTHON_BINDINGS=ON -DPYTHON_EXECUTABLE=/path/to/python ..
cmake --build . -j$(nproc)
```

### Install
```bash
cmake --build . --target install
```

### Run Tests
```bash
cd build
ctest --output-on-failure
```

### Generate Documentation
```bash
cd build
cmake --build . --target docs
```

## Directory Structure

```
build/
├── bin/              # Executables
├── lib/              # Libraries
├── include/          # Headers
├── pynlm/            # Python module
├── tests/            # Test binaries
└── docs/             # Generated documentation

install/
├── lib/              # C++ libraries
├── bin/              # Executables
├── include/nlm/      # Headers
└── python/           # Python package
```

## CMake Configuration

The enhanced CMake configuration provides:

### Core Library Targets
- `nlm_core` - Main core library
- `nlm_agent` - Agent system
- `nlm_world` - World system
- `nlm_phase3` - Phase 3 experiments
- `nlm_phase4` - Phase 4 experiments

### Executable Targets
- `nlm` - Main executable
- `nlm_phase3_demo` - Phase 3 demo
- `nlm_phase4_demo` - Phase 4 demo
- `nlm_test` - C++ tests

### Python Module Target
- `pynlm` - Python bindings

### Documentation Target
- `docs` - Doxygen documentation

### Install Targets
- `install` - Install all targets
- `install-dev` - Install development files
- `install-python` - Install Python package

## Testing

The build system includes comprehensive testing:

### C++ Tests
- Unit tests for core components
- Integration tests
- Demo tests

### Python Tests
- Unit tests for Python bindings
- Integration tests
- Performance tests

### Test Commands
```bash
# Run C++ tests
cd build
ctest

# Run Python tests
pytest

# Run all tests
ctest --output-on-failure && pytest
```

## Configuration Options

Available CMake options:

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_PYTHON_BINDINGS` | Enable Python bindings | OFF |
| `BUILD_TESTS` | Enable tests | ON |
| `BUILD_DOCS` | Generate documentation | OFF |
| `CMAKE_BUILD_TYPE` | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) | Release |
| `PYTHON_EXECUTABLE` | Python interpreter path | Automatic |
| `INSTALL_PREFIX` | Installation prefix | /usr/local |
| `NLM_INSTALL_PYTHON` | Install Python package | ON |
| `NLM_BUILD_TYPE` | NLM-specific build type | Release |

## Package Management

The build system supports:

### Conan/CMake Integration
- Modern CMake package management
- Dependency resolution
- Cross-platform builds

### Python Package Installation
- Install Python bindings as a package
- Configure package metadata
- Setup Python module structure

## Documentation

Generated documentation includes:
- API reference
- Class documentation
- Usage examples
- Configuration guides
- Build instructions

## Platform Support

The build system supports:
- Linux
- macOS
- Windows

## Compiler Support

Supported compilers:
- GCC >= 7
- Clang >= 5
- MSVC >= 2019

## Future Enhancements

Planned improvements:
- Package configuration files (CMakePackageConfig)
- Export targets for find_package
- Component-based installation
- Installation of third-party dependencies
- Cross-compilation support
- CI/CD integration

## License

MIT License

## Copyright

Copyright (c) 2024 NLM Authors