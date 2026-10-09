# Examples and Documentation for NLM Modular Build System

## Directory Structure

```
workspace/
├── cmake/                    # Core CMake modules
│   ├── NLMBuildSystem.cmake          # Main build system
│   ├── NLMBuildCore.cmake            # Core library definitions
│   ├── NLMDependencyManagement.cmake  # Dependency management
│   ├── NLMBuildOptions.cmake         # Build options
│   ├── NLMOptimization.cmake         # Optimization features
│   ├── NLMPythonBindings.cmake       # Python bindings
│   ├── NLMCiCd.cmake                 # CI/CD integration
│   ├── NLMDocumentation.cmake        # Documentation generation
│   └── NLMBuildSystemDocumentation.md # Documentation
├── configs/                    # Configuration files
│   └── NLMConfig.cmake          # Project configuration
├── examples/                   # Build examples
│   ├── basic-debug.md          # Basic debug build
│   ├── release-pybind11.md      # Release with Python bindings
│   ├── custom-config.md        # Custom configuration
│   └── advanced-options.md     # Advanced options
├── build-examples/             # Example build directories
│   ├── debug-build/             # Debug build example
│   ├── release-build/           # Release build example
│   └── optimized-build/         # Optimized build example
└── docs/                       # Generated documentation
    ├── api-reference/          # Doxygen generated
    ├── python-api/             # Python API docs
    └── build-system/           # Build system docs
```

## Configuration File Examples

### 1. User Configuration (configs/NLMConfig.cmake)

```cmake
# User-specific NLM configuration
# Place in ~/.nlmc/ or project configs/ directory

# Enable experimental features
add_definitions(-DEXPERIMENTAL_FEATURES)

# Add custom include directories
include_directories(
    ${NLM_SOURCE_DIR}/custom
    ${NLM_SOURCE_DIR}/experimental
)

# Link with custom libraries
link_libraries(
    custom_lib
    experimental_lib
)

# Override compiler settings
if(MSVC)
    target_compile_options(nlm_core PRIVATE /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_core PRIVATE -Wno-unused-variable -Wno-unused-function)
endif()

# Customize build options
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")

# Configure dependencies
find_package(CustomLib REQUIRED)
target_link_libraries(nlm_core PRIVATE CustomLib::Custom)

# Platform-specific settings
if(NLM_PLATFORM_WINDOWS)
    target_link_libraries(nlm_core PRIVATE ws2_32 iphlpapi)
elseif(NLM_PLATFORM_MACOS)
    find_library(ACCELERATE_LIBRARY Accelerate)
    if(ACCELERATE_LIBRARY)
        target_link_libraries(nlm_core PRIVATE ${ACCELERATE_LIBRARY})
    endif()
elseif(NLM_PLATFORM_LINUX)
    find_package(X11 REQUIRED)
    target_link_libraries(nlm_core PRIVATE ${X11_LIBRARIES})
endif()

# Optimization settings
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native")

# Installation preferences
set(CMAKE_INSTALL_PREFIX ${HOME}/nlminstall CACHE PATH "Installation prefix")

# Testing options
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_test(NAME nlm_test COMMAND nlm_test)
endif()
```

### 2. Project Configuration (configs/NLMConfig.cmake)

```cmake
# Project-level NLM configuration
# Override project-wide settings

# Core build configuration
set(NLM_BUILD_SHARED_LIBS ON)
set(NLM_ENABLE_PYTHON_BINDINGS ON)
set(NLM_ENABLE_TESTS ON)
set(NLM_ENABLE_DOCS ON)
set(NLM_ENABLE_COVERAGE ON)
set(NLM_ENABLE_SANITIZERS ON)

# Python configuration
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Dependency management
find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(Eigen3 REQUIRED)

# Platform-specific dependencies
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
endif()

# Compiler-specific settings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion)
endif()

# Optimization configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g -DNDEBUG")

# Installation directories
set(NLM_INCLUDE_INSTALL_DIR include)
set(NLM_LIBRARY_INSTALL_DIR lib)
set(NLM_BIN_INSTALL_DIR bin)

# Documentation configuration
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_PROJECT_NAME "NLM")
    set(DOXYGEN_PROJECT_NUMBER ${PROJECT_VERSION})
endif()

# Python bindings configuration
set(PYTHON_BINDINGS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/python)
set(PYTHON_MODULE_NAME "pynlm")
```

## Build Examples

### Example 1: Basic Debug Build

**File**: `examples/basic-debug.md`

```bash
#!/bin/bash
# Basic Debug Build Example

BUILD_DIR="${1:-debug-build}"
cd "$(dirname "$0")/.."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with debug options
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=OFF \
    -DNLM_ENABLE_DOCS=OFF \
    -DNLM_ENABLE_COVERAGE=OFF \
    -DNLM_ENABLE_SANITIZERS=OFF

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Display build information
make nlm_build_options

# Generate dependency graph
make dependency_graph

echo "Debug build completed successfully!"
echo "Build directory: $(pwd)"
echo "Targets created:"
echo "  - nlm (main executable)"
echo "  - nlm_core (core library)"
echo "  - nlm_agent (agent library)"
echo "  - nlm_world (world library)"
echo "  - nlm_test (test suite)"
```

### Example 2: Release Build with Python Bindings

**File**: `examples/release-pybind11.md`

```bash
#!/bin/bash
# Release Build with Python Bindings Example

BUILD_DIR="${1:-release-build}"
cd "$(dirname "$0")/.."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with release and Python bindings
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_DOCS=ON \
    -DNLM_ENABLE_COVERAGE=ON \
    -DNLM_ENABLE_SANITIZERS=OFF \
    -DNLM_ENABLE_PERFORMANCE_PROFILING=ON

# Build all targets
make -j$(nproc)

# Generate documentation
make docs

# Generate Python bindings documentation
make pynlm_docs

# Create package
make package_all

# Display build information
make nlm_build_options

# Generate dependency graph
make dependency_graph

echo "Release build with Python bindings completed successfully!"
echo "Build directory: $(pwd)"
echo "Python module: $(pwd)/pynlm.so"
echo "Documentation: $(pwd)/docs/"
```

### Example 3: Custom Configuration

**File**: `examples/custom-config.md`

```bash
#!/bin/bash
# Custom Configuration Example

BUILD_DIR="${1:-custom-build}"
CONFIG_FILE="${2:-configs/NLMConfig.cmake}"
cd "$(dirname "$0")/.."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with custom configuration
cmake .. \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_DOCS=ON \
    -DNLM_ENABLE_PERFORMANCE_PROFILING=ON \
    -DNLM_BUILD_SHARED_LIBS=ON \
    -DNLM_ENABLE_DISTRIBUTED_BUILD=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_VERBOSE_MAKEFILE=OFF

# Build with custom job count
CPU_COUNT=$(nproc)
make -j$((CPU_COUNT - 1))

# Install to custom prefix
make install

# Verify installation
if [ -f "${CMAKE_INSTALL_PREFIX}/bin/nlm" ]; then
    echo "Installation successful!"
    echo "Main executable: ${CMAKE_INSTALL_PREFIX}/bin/nlm"
    echo "Libraries: ${CMAKE_INSTALL_PREFIX}/lib/"
    echo "Headers: ${CMAKE_INSTALL_PREFIX}/include/"
else
    echo "Installation failed!"
    exit 1
fi

echo "Custom configuration build completed successfully!"
```

### Example 4: Advanced Options

**File**: `examples/advanced-options.md`

```bash
#!/bin/bash
# Advanced Options Example

BUILD_DIR="${1:-advanced-build}"
cd "$(dirname "$0")/.."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with advanced options
cmake .. \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_DOCS=ON \
    -DNLM_ENABLE_COVERAGE=ON \
    -DNLM_ENABLE_SANITIZERS=ON \
    -DNLM_ENABLE_PERFORMANCE_PROFILING=ON \
    -DNLM_ENABLE_LTO=ON \
    -DNLM_ENABLE_DISTRIBUTED_BUILD=ON \
    -DNLM_BUILD_SHARED_LIBS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_EXTENSIONS=OFF \
    -DNLM_PLATFORM_SPECIFIC=ON

# Build with optimizations
make -j$(nproc)

# Generate all documentation and reports
make docs
make dependency_graph
make coverage
make package_all

# Create compile_commands.json for tooling
cat compile_commands.json | jq '.[] | .file' > compile_commands.txt

# Create build summary
{
    echo "Build Summary:"
    echo "==============="
    echo "Build Type: $(make -p | grep "^CMAKE_BUILD_TYPE" | head -1 | cut -d= -f2)"
    echo "Compiler: $(make -p | grep "^CMAKE_CXX_COMPILER_ID" | head -1 | cut -d= -f2)"
    echo "Platform: $(uname -s)"
    echo "Python Bindings: $(test -f pynlm.so && echo "Yes" || echo "No")"
    echo "Tests: $(test -f nlm_test && echo "Yes" || echo "No")"
    echo "Documentation: $(test -d docs/html && echo "Yes" || echo "No")"
    echo "Coverage Report: $(test -f coverage.info && echo "Yes" || echo "No")"
    echo "Packages: $(find . -name "*.tar.gz" -o -name "*.zip" | wc -l)"
} > build_summary.txt

# Display build information
make nlm_build_options

echo "Advanced options build completed successfully!"
echo "Build directory: $(pwd)"
echo "Build summary: $(pwd)/build_summary.txt"
```

## Build System Test Suite

### Test 1: Configuration Validation

```bash
#!/bin/bash
# Test configuration validation

cd "$(dirname "$0")/.."
mkdir -p test-build
cd test-build

# Test basic configuration
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON
if [ $? -eq 0 ]; then
    echo "✓ Basic configuration test passed"
else
    echo "✗ Basic configuration test failed"
    exit 1
fi

# Test Python bindings configuration
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON_BINDINGS=ON
if [ $? -eq 0 ]; then
    echo "✓ Python bindings configuration test passed"
else
    echo "✗ Python bindings configuration test failed"
    exit 1
fi

# Test custom configuration
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNLM_ENABLE_COVERAGE=ON
if [ $? -eq 0 ]; then
    echo "✓ Custom configuration test passed"
else
    echo "✗ Custom configuration test failed"
    exit 1
fi

cd ..
rm -rf test-build
echo "All configuration tests passed!"
```

### Test 2: Build Target Validation

```bash
#!/bin/bash
# Test build target validation

BUILD_DIR="test-build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
cmake ..

# Check if all expected targets are built
TARGETS_TO_CHECK=(
    "nlm"
    "nlm_core"
    "nlm_agent"
    "nlm_world"
    "nlm_phase3"
    "nlm_phase4"
    "nlm_phase3_demo"
    "nlm_phase4_demo"
    "nlm_benchmark"
)

ALL_TARGETS_FOUND=true
for target in "${TARGETS_TO_CHECK[@]}"; do
    if make --dry-run "$target" >/dev/null 2>&1; then
        echo "✓ Target '$target' found"
    else
        echo "✗ Target '$target' not found"
        ALL_TARGETS_FOUND=false
    fi
    
    if make --dry-run "$target" >/dev/null 2>&1 && [ -f "$target" ]; then
        echo "✓ Target '$target' executable exists"
    elif [ -d "$target" ]; then
        echo "✓ Target '$target' directory exists"
    fi
    
    if ! make --dry-run "$target" >/dev/null 2>&1; then
        echo "✗ Target '$target' build definition broken"
        ALL_TARGETS_FOUND=false
    fi
    
done

if [ "$ALL_TARGETS_FOUND" = true ]; then
    echo "All build targets validated successfully!"
else
    echo "Some build targets are missing!"
    exit 1
fi

cd ..
rm -rf "$BUILD_DIR"
```

### Test 3: Dependency Management

```bash
#!/bin/bash
# Test dependency management

cd "$(dirname "$0")/.."
mkdir -p test-dep-build
cd test-dep-build

# Configure with dependency management
cmake .. -DNLM_ENABLE_TESTS=ON -DNLM_ENABLE_PYTHON_BINDINGS=ON

# Check if dependency scripts are generated
if [ -f "generate_graph.py" ]; then
    echo "✓ Dependency graph generation script found"
else
    echo "✗ Dependency graph generation script missing"
    exit 1
fi

# Check if custom targets exist
if make --dry-run dependency_graph >/dev/null 2>&1; then
    echo "✓ dependency_graph target found"
else
    echo "✗ dependency_graph target missing"
    exit 1
fi

if make --dry-run pynlm_docs >/dev/null 2>&1; then
    echo "✓ pynlm_docs target found"
else
    echo "✗ pynlm_docs target missing"
    exit 1
fi

cd ..
rm -rf test-dep-build
echo "Dependency management tests passed!"
```

## Documentation Examples

### CMake Configuration Examples

#### Minimal Configuration

```cmake
# Minimal NLM configuration
cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

include(cmake/NLMBuildSystem.cmake)
```

#### Feature-Rich Configuration

```cmake
# Feature-rich NLM configuration
cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "Neuromodular Learning Machine with full feature set"
)

# Enable all features
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_TESTS "Enable tests" ON)
option(NLM_ENABLE_DOCS "Enable documentation" ON)
option(NLM_ENABLE_COVERAGE "Enable coverage reporting" ON)
option(NLM_ENABLE_SANITIZERS "Enable memory leak detection" OFF)
option(NLM_ENABLE_PERFORMANCE_PROFILING "Enable PGO" ON)
option(NLM_ENABLE_DISTRIBUTED_BUILD "Enable distributed builds" ON)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries" ON)

# Include core modules
include(cmake/NLMBuildSystem.cmake)
include(cmake/NLMDependencyManagement.cmake)
include(cmake/NLMOptimization.cmake)
include(cmake/NLMDocumentation.cmake)

# Set build type
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")

# Configure compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include system modules
include(FindPackageHandleStandardArgs)
```

#### Cross-Platform Configuration

```cmake
# Cross-platform NLM configuration
cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Platform detection
if(WIN32)
    set(NLM_PLATFORM "Windows")
    set(NLM_COMPILER_RUNTIME "/MD")
elseif(APPLE)
    set(NLM_PLATFORM "macOS")
    set(NLM_COMPILER_RUNTIME "")
elseif(UNIX)
    set(NLM_PLATFORM "Linux")
    set(NLM_COMPILER_RUNTIME "")
endif()

# Include platform-specific modules
include(cmake/NLMPlatform.cmake)

# Set platform-specific compile options
if(NLM_PLATFORM STREQUAL "Windows")
    add_compile_options(/W4)
elseif(NLM_PLATFORM STREQUAL "macOS")
    add_compile_options(-stdlib=libc++)
elseif(NLM_PLATFORM STREQUAL "Linux")
    add_compile_options(-pthread)
endif()

# Configure build system
include(cmake/NLMBuildSystem.cmake)
```

## Usage Examples

### 1. Interactive Build Configuration

```bash
# Create build directory
mkdir build
cd build

# Configure with interactive prompts (requires CMake 3.19+)
cmake -DNLM_INTERACTIVE_MODE=ON ..

# Set build type interactively
cmake -DNLM_BUILD_TYPE="Debug" ..

# Enable specific features
cmake -DNLM_ENABLE_PYTHON_BINDINGS=ON -DNLM_ENABLE_TESTS=ON ..

# Build
cmake --build . --parallel
```

### 2. Programmatic Configuration

```bash
#!/bin/bash
# Programmatic build configuration

CONFIG_FILE="custom-config.cmake"
cd "$(dirname "$0")/.."
mkdir -p build
cd build

# Generate configuration file
cat > "$CONFIG_FILE" << EOF
cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

option(NLM_FEATURE_X "Enable feature X" ON)
option(NLM_FEATURE_Y "Enable feature Y" OFF)

include(cmake/NLMBuildSystem.cmake)

if(NLM_FEATURE_X)
    add_definitions(-DFEATURE_X_ENABLED)
endif()

if(NLM_FEATURE_Y)
    add_definitions(-DFEATURE_Y_ENABLED)
endif()
EOF

# Configure with custom configuration
cmake -C "$CONFIG_FILE" ..

# Build
cmake --build . --parallel
```

### 3. Batch Processing

```bash
#!/bin/bash
# Batch processing for multiple build configurations

cd "$(dirname "$0")/.."

CONFIGS=(
    "debug;Debug;OFF;OFF;OFF"
    "release;Release;ON;ON;ON"
    "optimized;RelWithDebInfo;ON;ON;ON"
)

for config in "${CONFIGS[@]}"; do
    IFS=';' read -r name build_type python tests docs <<<"$config"
    
    echo "Building $name configuration..."
    
    mkdir -p "build-$name"
    cd "build-$name"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DNLM_ENABLE_PYTHON_BINDINGS="$python" \
        -DNLM_ENABLE_TESTS="$tests" \
        -DNLM_ENABLE_DOCS="$docs"
    
    make -j$(nproc)
    
    ctest --output-on-failure
    
    cd ..
    
    echo "$name configuration completed successfully!"
    echo ""
done
```

### 4. Development Workflow

```bash
#!/bin/bash
# Development workflow example

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_ROOT"

# Create development build directory
DEV_DIR="dev-build"
if [ -d "$DEV_DIR" ]; then
    echo "Removing existing $DEV_DIR..."
    rm -rf "$DEV_DIR"
fi

mkdir -p "$DEV_DIR"
cd "$DEV_DIR"

# Configure for development
echo "Configuring development build..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_DOCS=ON \
    -DNLM_ENABLE_COVERAGE=ON \
    -DNLM_ENABLE_SANITIZERS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_VERBOSE_MAKEFILE=OFF

# Build all targets
echo "Building development targets..."
make -j$(nproc)

# Run tests to verify build
echo "Running tests..."
ctest --output-on-failure

# Generate documentation
echo "Generating documentation..."
make docs

# Generate dependency graph
echo "Generating dependency graph..."
make dependency_graph

# Generate build information
echo "Generating build information..."
make nlm_build_options

# List all targets
echo "Development build completed!"
echo "Available targets:"
echo "  nlm (main executable)"
echo "  nlm_core, nlm_agent, nlm_world (libraries)"
echo "  nlm_test (test suite)"
echo "  nlm_phase3_demo, nlm_phase4_demo (demo executables)"
echo "  nlm_benchmark (benchmark)"
echo "  docs (documentation)"
echo "  dependency_graph (dependency visualization)"
echo "  nlm_build_options (build information)"
echo "  pynlm_docs (Python bindings documentation)"

cd "$PROJECT_ROOT"
echo "Development environment ready at: $DEV_DIR"
```

## Quick Reference

### Common Commands

```bash
# Create and configure build directory
mkdir build && cd build
cmake ..

# Build all targets
make -j$(nproc)

# Build specific target
make nlm

# Build with parallel jobs
make -j8

# Run tests
ctest --output-on-failure

# Display build options
make nlm_build_options

# Generate dependency graph
make dependency_graph

# Generate documentation
make docs

# Generate Python bindings documentation
make pynlm_docs

# Create distribution package
make package_all

# Clean build directory
make clean
rm -rf build
```

### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_BUILD_TYPE` | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) | Release |
| `NLM_ENABLE_PYTHON_BINDINGS` | Enable Python bindings | ON |
| `NLM_ENABLE_TESTS` | Enable tests | ON |
| `NLM_ENABLE_DOCS` | Enable documentation generation | OFF |
| `NLM_ENABLE_COVERAGE` | Enable code coverage | OFF |
| `NLM_ENABLE_SANITIZERS` | Enable memory leak detection | OFF |
| `NLM_ENABLE_PERFORMANCE_PROFILING` | Enable performance profiling | OFF |
| `NLM_ENABLE_DISTRIBUTED_BUILD` | Enable distributed builds | OFF |
| `NLM_BUILD_SHARED_LIBS` | Build shared libraries | OFF |

### Configuration Files

| File | Location | Purpose |
|------|----------|---------|
| `cmake/NLMBuildSystem.cmake` | cmake/ | Main build system |
| `cmake/NLMBuildCore.cmake` | cmake/ | Core library definitions |
| `cmake/NLMDependencyManagement.cmake` | cmake/ | Dependency management |
| `cmake/NLMBuildOptions.cmake` | cmake/ | Build options |
| `cmake/NLMOptimization.cmake` | cmake/ | Optimization features |
| `cmake/NLMPythonBindings.cmake` | cmake/ | Python bindings |
| `cmake/NLMCiCd.cmake` | cmake/ | CI/CD integration |
| `cmake/NLMDocumentation.cmake` | cmake/ | Documentation generation |
| `configs/NLMConfig.cmake` | configs/ | Project configuration |

### Target Reference

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core neural computation |
| `nlm_agent` | Static Library | Agent interface |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_benchmark` | Executable | Performance benchmark |
| `nlm_test` | Executable | Unit test suite |
| `docs` | Custom Target | Documentation generation |
| `dependency_graph` | Custom Target | Dependency graph visualization |
| `pynlm_docs` | Custom Target | Python bindings documentation |
| `nlm_build_options` | Custom Target | Build information display |
| `package_all` | Custom Target | Package creation |

This comprehensive documentation provides examples and tutorials for using the NLM modular build system effectively.
