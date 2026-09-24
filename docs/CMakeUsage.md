# Modern CMake Configuration for NLM

This document describes the modern CMake build system used for the NLM (Neural Learning Machine) project.

## Overview

NLM uses a modern target-based CMake architecture that follows best practices and provides comprehensive build options for development, testing, and distribution.

## Key Features

### 1. Target-Based Architecture

Instead of compiling source files directly, NLM uses CMake targets to represent:

- **nlm_core**: Core neural computation library
- **nlm_agent**: Agent brain interface and behavior
- **nlm_world**: Simulation world and environment
- **nlm_phase3**: Phase 3 experimental features
- **nlm_phase4**: Phase 4 experimental features

Each target has proper:
- Dependency management
- Include directory propagation
- Compile feature requirements (C++20)
- Platform-specific optimizations

### 2. Build Configuration Options

The following CMake options control the build:

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_SHARED_LIBS` | Build shared instead of static libraries | OFF |
| `NLM_BUILD_TESTS` | Build test executables | ON |
| `NLM_BUILD_PYTHON` | Build Python bindings | OFF |
| `NLM_ENABLE_SANITIZERS` | Enable memory sanitizers | ON |
| `NLM_ENABLE_PROFILING` | Enable performance profiling | OFF |
| `NLM_ENABLE_EXPERIMENTS` | Build experimental features | ON |
| `NLM_ENABLE_CCACHE` | Use CCache for faster rebuilds | ON |

## Building from Source

### Basic Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build with Tests

```bash
cmake .. -DNLM_BUILD_TESTS=ON
cmake --build .
ctest
```

### Build with Python Bindings

```bash
cmake .. -DNLM_BUILD_PYTHON=ON
cmake --build .
```

### Build with All Features

```bash
cmake .. \
  -DBUILD_SHARED_LIBS=ON \
  -DNLM_BUILD_TESTS=ON \
  -DNLM_BUILD_PYTHON=ON \
  -DNLM_ENABLE_SANITIZERS=ON \
  -DNLM_ENABLE_PROFILING=ON \
  -DNLM_ENABLE_EXPERIMENTS=ON
cmake --build .
```

### Build Types

| Build Type | Description |
|------------|-------------|
| `Debug` | Debug symbols, no optimizations |
| `Release` | Optimized, no debug symbols (default) |
| `RelWithDebInfo` | Optimized with debug symbols |
| `MinSizeRel` | Optimized for minimal size |

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Platform Support

### Linux/macOS

- Uses RPATH for library discovery
- Comprehensive warning flags
- Modern compiler optimizations

### Windows

- Dynamic linking support via `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS`
- Different warning flags for MSVC
- Unicode support enabled

## Installation

```bash
cmake --install . --prefix /usr/local
```

### Custom Installation Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
cmake --install . --prefix=/opt/nlm
```

## Package Management

NLM uses CPack to generate distribution packages:

### Linux Packages

```bash
cpack --config CPackConfig.cmake
```

Generates:
- `.deb` packages for Debian/Ubuntu
- `.rpm` packages for RedHat/CentOS

### macOS Packages

```bash
cpack --config CPackConfig.cmake
```

Generates:
- `.dmg` disk images

### Generic Packages

```bash
cpack --config CPackConfig.cmake
```

Generates:
- `.tar.gz` source tarballs
- `.zip` archives

## Running Tests

### Basic Test Suite

```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest
```

### Verbose Test Output

```bash
ctest -V
```

### Test Filtering

```bash
ctest -R "test_type"  # Run tests matching pattern
```

## Python Bindings

### Building Python Bindings

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON=ON
cmake --build .
```

### Installing Python Module

```bash
cmake --install . --prefix=/usr/local
```

### Using Python Bindings

```python
import pynlm

# Create a brain with configuration
from pynlm import createBrain, createDefaultConfig
config = createDefaultConfig()
brain = createBrain(config)

# Initialize and run
brain.initialize()
brain.step()
```

## Build Optimizations

### CCache Integration

NLM automatically integrates CCache if available:

```bash
cmake .. -DNLM_ENABLE_CCACHE=ON
```

### Compiler Optimizations

Modern compilers receive:

- **GCC/Clang**: -Wall -Wextra -Wpedantic
- **MSVC**: /W4
- **Platform-specific** optimization flags

### Sanitizers

Memory sanitizers are enabled by default for development:

```bash
cmake .. -DNLM_ENABLE_SANITIZERS=ON
```

Enables:
- AddressSanitizer (ASAN)
- LeakSanitizer (LSAN)
- UndefinedBehaviorSanitizer (UBSAN)
- ThreadSanitizer (TSAN)

## Development Features

### Experimental Features

Enable experimental functionality:

```bash
cmake .. -DNLM_ENABLE_EXPERIMENTS=ON
```

This builds:
- Phase 3: World interaction and behavior
- Phase 4: Advanced learning mechanisms

### Profiling

Enable performance profiling:

```bash
cmake .. -DNLM_ENABLE_PROFILING=ON
```

### Debug Information

Enable comprehensive debug information:

```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## Cross-Platform Configuration

### Windows-Specific Settings

```bash
cmake .. -G "Visual Studio 16 2019"
```

### macOS-Specific Settings

```bash
cmake .. -G "Xcode"
```

### Linux-Specific Settings

```bash
cmake .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON
```

## CMake Utility Targets

NLM provides several utility targets:

| Target | Description |
|--------|-------------|
| `print_config` | Display current configuration |
| `nlm_build_info` | Generate build information |
| `clean_all` | Clean all build artifacts |

Example usage:

```bash
cmake --build . --target print_config
cmake --build . --target nlm_build_info
cmake --build . --target clean_all
```

## Dependency Management

NLM uses CMake's modern export/import mechanism:

### Package Configuration

Generated files:
- `NLMConfig.cmake` - Package configuration
- `NLMConfigVersion.cmake` - Version checking
- `NLMTargets.cmake` - Target definitions

### Finding NLM

```cmake
find_package(NLM REQUIRED)

# Use targets
NLM::nlm_core
NLM::nlm_agent
NLM::nlm_world
```

## Troubleshooting

### Common Issues

**Issue**: CMake not found
```
sudo apt-get install cmake
# or
brew install cmake
```

**Issue**: Compiler not found
```
sudo apt-get install g++
# or
brew install gcc
```

**Issue**: Python bindings not working
```bash
cmake .. -DNLM_BUILD_PYTHON=ON
# Ensure pybind11 is available
```

**Issue**: Installation permissions
```bash
cmake --install . --prefix=/usr/local
sudo cmake --install . --prefix=/usr/local
```

### Build Logs

For detailed build information:

```bash
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
cmake --build . VERBOSE=1
```

## Migration Guide

### From Previous CMake Version

This version includes significant improvements:

1. **Target-based architecture**: No more flat CMakeLists.txt
2. **Modern CMake practices**: Proper export/import, generator expressions
3. **Enhanced options**: More build configuration options
4. **Better packaging**: Full CPack integration
5. **Cross-platform support**: Comprehensive platform-specific configurations

### Key Changes

- CMake version: 3.16 → 3.20
- Static library targets → Modular library targets
- Single build file → Modular structure with subdirectory builds
- Basic testing → Comprehensive testing with modern CMake testing

## Best Practices

### Using NLM in Your Projects

```cmake
# Find NLM
find_package(NLM REQUIRED)

# Use targets with proper dependencies
target_link_libraries(your_project NLM::nlm_core NLM::nlm_agent)
```

### Contributing

1. Follow the existing code style
2. Use the target-based approach
3. Add new targets for new modules
4. Update documentation when adding new options
5. Test on multiple platforms

## Further Reading

- [CMake Best Practices](https://cmake.org/cmake/help/latest/guide/best-practices/index.html)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [pybind11 Documentation](https://pybind11.readthedocs.io/)
- [CPack Documentation](https://cmake.org/cmake/help/latest/cpack.html)

---

*Last Updated: $(date +%Y-%m-%d)*
