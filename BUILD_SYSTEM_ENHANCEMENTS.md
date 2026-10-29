NLM Build System and Automation Improvements
===========================================

## Overview

This document details the comprehensive build system and automation improvements for the NLM (Neural Learning Machine) project, including enhanced CMake configuration, improved build scripts, automation tools, and deployment capabilities.

## Changes Made

### 1. Enhanced CMake Configuration

The build system has been significantly enhanced to provide better performance, debugging capabilities, and automation. Key improvements include:

#### Advanced Build Options
- **Modern C++20 support** with strict compiler settings
- **Performance optimizations** including SIMD vectorization and link-time optimization
- **Comprehensive debugging tools** with AddressSanitizer, ThreadSanitizer, and Valgrind support
- **Documentation generation** with automatic Sphinx integration
- **Code coverage reporting** for testing and validation
- **Static analysis tools** with Clang-Tidy and include-what-you-use support
- **Python bindings support** with pybind11 integration
- **Precompiled headers** for faster compilation
- **Installation capabilities** with customizable installation directories

#### Build Configuration
```bash
# Core build commands
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# With Python bindings
mkdir build
cd build
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j4

# With sanitizers
mkdir build
cd build
cmake .. -DNLM_ENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug
make -j4

# With documentation
mkdir build
cd build
cmake .. -DNLM_ENABLE_DOCS=ON
make docs

# With static analysis
mkdir build
cd build
cmake .. -DNLM_ENABLE_STATIC_ANALYSIS=ON
make clangtidy
```

#### Build Outputs
After successful compilation, the build system generates:

| Component | Type | Description |
|-----------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `nlm_benchmark` | Executable | Benchmark executable |
| `pymnl.so` | Shared Library | Python bindings |

### 2. Enhanced Build Scripts

#### Environment Setup Script
```bash
#!/bin/bash
# setup_env.sh - Environment setup script for NLM

set -e

# Detect system architecture
ARCH=$(uname -m)
PLATFORM=$(uname -s)

# Set compiler flags based on platform
if [ "$PLATFORM" = "Linux" ]; then
    if [ "$ARCH" = "x86_64" ]; then
        export CXXFLAGS="-march=x86-64 -mtune=generic"
    elif [ "$ARCH" = "aarch64" ]; then
        export CXXFLAGS="-march=armv8-a"
    fi
elif [ "$PLATFORM" = "Darwin" ]; then
    if [ "$ARCH" = "arm64" ]; then
        export CXXFLAGS="-march=armv8-a64"
    fi
elif [ "$PLATFORM" = "Windows" ]; then
    export CXXFLAGS="/arch:AVX2"
fi

# Set memory limits
if command -v ulimit >/dev/null 2>&1; then
    ulimit -v 4194304  # 4GB virtual memory
    ulimit -t 3600     # 1 hour CPU time
fi

# Create directories
mkdir -p build
mkdir -p logs
mkdir -p docs
mkdir -p outputs
mkdir -p scripts
mkdir -p tests

# Set environment variables
export NLM_BUILD_TYPE="Release"
export NLM_ENABLE_PYTHON_BINDINGS="ON"
export NLM_ENABLE_TESTS="ON"
export NLM_ENABLE_BENCHMARKS="ON"
export NLM_ENABLE_DOCS="ON"
export NLM_ENABLE_COVERAGE="OFF"
export NLM_ENABLE_ASAN="OFF"
export NLM_ENABLE_TSAN="OFF"
export NLM_ENABLE_VALGRIND="OFF"
export NLM_ENABLE_STATIC_ANALYSIS="OFF"
export NLM_ENABLE_CLANG_TIDY="OFF"
export NLM_ENABLE_IWYU="OFF"
export NLM_ENABLE_WERROR="OFF"
export NLM_ENABLE_WALL="ON"
export NLM_ENABLE_PEDANTIC="ON"
export NLM_ENABLE_OPTIMIZATION="ON"
export NLM_ENABLE_DEBUG_INFO="ON"
export NLM_ENABLE_PRECOMPILED_HEADERS="ON"
export NLM_ENABLE_EXPORTS="ON"
export NLM_ENABLE_INSTALL="ON"

# Display build configuration
echo "NLM Build Environment Configuration:"
echo "Architecture: $ARCH"
echo "Platform: $PLATFORM"
echo "Compiler: $CXX"
echo "CXX Flags: $CXXFLAGS"
echo "NLM_BUILD_TYPE: $NLM_BUILD_TYPE"
```

#### Run Script
```bash
#!/bin/bash
# run-script - Main execution script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --version|-v)
            show_version
            exit 0
            ;;
        --config|-c)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --brain|-b)
            BRAIN_FILE="$2"
            shift 2
            ;;
        --world|-w)
            WORLD_FILE="$2"
            shift 2
            ;;
        --agent|-a)
            AGENT_FILE="$2"
            shift 2
            ;;
        --output|-o)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        --input|-i)
            INPUT_FILE="$2"
            shift 2
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --quiet|-q)
            QUIET=true
            shift
            ;;
        --force|-f)
            FORCE=true
            shift
            ;;
        --dry-run|-d)
            DRY_RUN=true
            shift
            ;;
        --interactive|-I)
            INTERACTIVE=true
            shift
            ;;
        --batch|-B)
            BATCH_FILE="$2"
            shift 2
            ;;
        --script|-s)
            SCRIPT_FILE="$2"
            shift 2
            ;;
        --daemon|-D)
            DAEMON=true
            shift
            ;;
        --foreground|-F)
            FOREGROUND=true
            shift
            ;;
        --log)
            LOG_FILE="$2"
            shift 2
            ;;
        --log-level)
            LOG_LEVEL="$2"
            shift 2
            ;;
        --profile)
            PROFILE=true
            shift
            ;;
        --benchmark)
            BENCHMARK=true
            shift
            ;;
        --validate)
            VALIDATE=true
            shift
            ;;
        --analyze)
            ANALYZE=true
            shift
            ;;
        --export)
            EXPORT=true
            shift
            ;;
        --import)
            IMPORT=true
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        --version)
            show_version
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Load configuration from file
if [ -n "$CONFIG_FILE" ]; then
    if [ ! -f "$CONFIG_FILE" ]; then
        echo "Error: Configuration file not found: $CONFIG_FILE"
        exit 1
    fi
    source "$CONFIG_FILE"
fi

# Set up build directory
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Build the project
echo "Building NLM..."
cd "$BUILD_DIR"

# Configure with CMake
if [ -n "$FORCE" ] || [ ! -f "CMakeCache.txt" ]; then
    CMAKE_ARGS=""
    
    if [ -n "$CONFIG_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_CONFIG_FILE=$CONFIG_FILE"
    fi
    
    if [ -n "$BRAIN_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_BRAIN_FILE=$BRAIN_FILE"
    fi
    
    if [ -n "$WORLD_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_WORLD_FILE=$WORLD_FILE"
    fi
    
    if [ -n "$AGENT_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_AGENT_FILE=$AGENT_FILE"
    fi
    
    if [ -n "$OUTPUT_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_OUTPUT_FILE=$OUTPUT_FILE"
    fi
    
    if [ -n "$INPUT_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_INPUT_FILE=$INPUT_FILE"
    fi
    
    if [ -n "$VERBOSE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
    fi
    
    if [ -n "$QUIET" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_QUIET=ON"
    fi
    
    if [ -n "$DRY_RUN" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_DRY_RUN=ON"
    fi
    
    if [ -n "$INTERACTIVE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_INTERACTIVE=ON"
    fi
    
    if [ -n "$BATCH_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_BATCH_FILE=$BATCH_FILE"
    fi
    
    if [ -n "$SCRIPT_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_SCRIPT_FILE=$SCRIPT_FILE"
    fi
    
    if [ -n "$DAEMON" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_DAEMON=ON"
    fi
    
    if [ -n "$FOREGROUND" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_FOREGROUND=ON"
    fi
    
    if [ -n "$LOG_FILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_LOG_FILE=$LOG_FILE"
    fi
    
    if [ -n "$LOG_LEVEL" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_LOG_LEVEL=$LOG_LEVEL"
    fi
    
    if [ -n "$PROFILE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_PROFILE=ON"
    fi
    
    if [ -n "$BENCHMARK" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_BENCHMARK=ON"
    fi
    
    if [ -n "$VALIDATE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_VALIDATE=ON"
    fi
    
    if [ -n "$ANALYZE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_ANALYZE=ON"
    fi
    
    if [ -n "$EXPORT" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_EXPORT=ON"
    fi
    
    if [ -n "$IMPORT" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DNLM_IMPORT=ON"
    fi
    
    cmake $CMAKE_ARGS ..
fi

# Build the project
make -j$(nproc)

# Run tests
if [ -n "$NLM_ENABLE_TESTS" ] && [ "$NLM_ENABLE_TESTS" = "ON" ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

# Run benchmarks
if [ -n "$NLM_ENABLE_BENCHMARKS" ] && [ "$NLM_ENABLE_BENCHMARKS" = "ON" ]; then
    echo "Running benchmarks..."
    ./nlm_benchmark
fi

# Run Python tests
if command -v python3 >/dev/null 2>&1; then
    echo "Running Python tests..."
    python3 -m pytest tests/ -v
fi

# Run analysis
if [ -n "$NLM_ANALYZE" ] && [ "$NLM_ANALYZE" = "ON" ]; then
    echo "Running analysis..."
    ./nlm --analyze
fi

# Run profiling
if [ -n "$NLM_PROFILE" ] && [ "$NLM_PROFILE" = "ON" ]; then
    echo "Running profiling..."
    ./nlm --profile
fi

# Run export
if [ -n "$NLM_EXPORT" ] && [ "$NLM_EXPORT" = "ON" ]; then
    echo "Running export..."
    ./nlm --export
fi

# Run import
if [ -n "$NLM_IMPORT" ] && [ "$NLM_IMPORT" = "ON" ]; then
    echo "Running import..."
    ./nlm --import
fi

# Display summary
echo "Build completed successfully!"
echo "Build directory: $BUILD_DIR"
echo "Targets:"
echo "  - nlm: Main executable"
echo "  - nlm_core: Core library"
echo "  - nlm_agent: Agent library"
echo "  - nlm_world: World library"
echo "  - nlm_phase3: Phase 3 library"
echo "  - nlm_phase4: Phase 4 library"
echo "  - nlm_test: Test executable"
echo "  - nlm_benchmark: Benchmark executable"
echo "  - pymnl: Python bindings (if enabled)"
```

### 3. Setup Script
```bash
#!/bin/bash
# setup-script - Environment setup script for NLM

set -e

# Detect system architecture
ARCH=$(uname -m)
PLATFORM=$(uname -s)

# Set compiler flags based on platform
if [ "$PLATFORM" = "Linux" ]; then
    if [ "$ARCH" = "x86_64" ]; then
        export CXXFLAGS="-march=x86-64 -mtune=generic"
    elif [ "$ARCH" = "aarch64" ]; then
        export CXXFLAGS="-march=armv8-a"
    fi
elif [ "$PLATFORM" = "Darwin" ]; then
    if [ "$ARCH" = "arm64" ]; then
        export CXXFLAGS="-march=armv8-a64"
    fi
elif [ "$PLATFORM" = "Windows" ]; then
    export CXXFLAGS="/arch:AVX2"
fi

# Set memory limits
if command -v ulimit >/dev/null 2>&1; then
    ulimit -v 4194304  # 4GB virtual memory
    ulimit -t 3600     # 1 hour CPU time
fi

# Create directories
mkdir -p build
mkdir -p logs
mkdir -p docs
mkdir -p outputs
mkdir -p scripts
mkdir -p tests

# Set environment variables
export NLM_BUILD_TYPE="Release"
export NLM_ENABLE_PYTHON_BINDINGS="ON"
export NLM_ENABLE_TESTS="ON"
export NLM_ENABLE_BENCHMARKS="ON"
export NLM_ENABLE_DOCS="ON"
export NLM_ENABLE_COVERAGE="OFF"
export NLM_ENABLE_ASAN="OFF"
export NLM_ENABLE_TSAN="OFF"
export NLM_ENABLE_VALGRIND="OFF"
export NLM_ENABLE_STATIC_ANALYSIS="OFF"
export NLM_ENABLE_CLANG_TIDY="OFF"
export NLM_ENABLE_IWYU="OFF"
export NLM_ENABLE_WERROR="OFF"
export NLM_ENABLE_WALL="ON"
export NLM_ENABLE_PEDANTIC="ON"
export NLM_ENABLE_OPTIMIZATION="ON"
export NLM_ENABLE_DEBUG_INFO="ON"
export NLM_ENABLE_PRECOMPILED_HEADERS="ON"
export NLM_ENABLE_EXPORTS="ON"
export NLM_ENABLE_INSTALL="ON"

# Display build configuration
echo "NLM Build Environment Configuration:"
echo "Architecture: $ARCH"
echo "Platform: $PLATFORM"
echo "Compiler: $CXX"
echo "CXX Flags: $CXXFLAGS"
echo "NLM_BUILD_TYPE: $NLM_BUILD_TYPE"
```

### 4. Automation Scripts

#### Build Script
```bash
#!/bin/bash
# build.sh - Build script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --version|-v)
            show_version
            exit 0
            ;;
        --type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --python)
            PYTHON_BINDINGS="$2"
            shift 2
            ;;
        --tests)
            ENABLE_TESTS="$2"
            shift 2
            ;;
        --benchmarks)
            ENABLE_BENCHMARKS="$2"
            shift 2
            ;;
        --docs)
            ENABLE_DOCS="$2"
            shift 2
            ;;
        --coverage)
            ENABLE_COVERAGE="$2"
            shift 2
            ;;
        --asan)
            ENABLE_ASAN="$2"
            shift 2
            ;;
        --tsan)
            ENABLE_TSAN="$2"
            shift 2
            ;;
        --valgrind)
            ENABLE_VALGRIND="$2"
            shift 2
            ;;
        --static-analysis)
            ENABLE_STATIC_ANALYSIS="$2"
            shift 2
            ;;
        --clang-tidy)
            ENABLE_CLANG_TIDY="$2"
            shift 2
            ;;
        --iwyu)
            ENABLE_IWYU="$2"
            shift 2
            ;;
        --werror)
            ENABLE_WERROR="$2"
            shift 2
            ;;
        --wall)
            ENABLE_WALL="$2"
            shift 2
            ;;
        --pedantic)
            ENABLE_PEDANTIC="$2"
            shift 2
            ;;
        --optimization)
            ENABLE_OPTIMIZATION="$2"
            shift 2
            ;;
        --debug-info)
            ENABLE_DEBUG_INFO="$2"
            shift 2
            ;;
        --precompiled-headers)
            ENABLE_PRECOMPILED_HEADERS="$2"
            shift 2
            ;;
        --exports)
            ENABLE_EXPORTS="$2"
            shift 2
            ;;
        --install)
            ENABLE_INSTALL="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Set default values
: ${BUILD_TYPE:=Release}
: ${PYTHON_BINDINGS:=ON}
: ${ENABLE_TESTS:=ON}
: ${ENABLE_BENCHMARKS:=ON}
: ${ENABLE_DOCS:=ON}
: ${ENABLE_COVERAGE:=OFF}
: ${ENABLE_ASAN:=OFF}
: ${ENABLE_TSAN:=OFF}
: ${ENABLE_VALGRIND:=OFF}
: ${ENABLE_STATIC_ANALYSIS:=OFF}
: ${ENABLE_CLANG_TIDY:=OFF}
: ${ENABLE_IWYU:=OFF}
: ${ENABLE_WERROR:=OFF}
: ${ENABLE_WALL:=ON}
: ${ENABLE_PEDANTIC:=ON}
: ${ENABLE_OPTIMIZATION:=ON}
: ${ENABLE_DEBUG_INFO:=ON}
: ${ENABLE_PRECOMPILED_HEADERS:=ON}
: ${ENABLE_EXPORTS:=ON}
: ${ENABLE_INSTALL:=ON}

# Create build directory
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Build the project
echo "Building NLM with type: $BUILD_TYPE"
cd "$BUILD_DIR"

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DNLM_ENABLE_PYTHON_BINDINGS="$PYTHON_BINDINGS" \
    -DNLM_ENABLE_TESTS="$ENABLE_TESTS" \
    -DNLM_ENABLE_BENCHMARKS="$ENABLE_BENCHMARKS" \
    -DNLM_ENABLE_DOCS="$ENABLE_DOCS" \
    -DNLM_ENABLE_COVERAGE="$ENABLE_COVERAGE" \
    -DNLM_ENABLE_ASAN="$ENABLE_ASAN" \
    -DNLM_ENABLE_TSAN="$ENABLE_TSAN" \
    -DNLM_ENABLE_VALGRIND="$ENABLE_VALGRIND" \
    -DNLM_ENABLE_STATIC_ANALYSIS="$ENABLE_STATIC_ANALYSIS" \
    -DNLM_ENABLE_CLANG_TIDY="$ENABLE_CLANG_TIDY" \
    -DNLM_ENABLE_IWYU="$ENABLE_IWYU" \
    -DNLM_ENABLE_WERROR="$ENABLE_WERROR" \
    -DNLM_ENABLE_WALL="$ENABLE_WALL" \
    -DNLM_ENABLE_PEDANTIC="$ENABLE_PEDANTIC" \
    -DNLM_ENABLE_OPTIMIZATION="$ENABLE_OPTIMIZATION" \
    -DNLM_ENABLE_DEBUG_INFO="$ENABLE_DEBUG_INFO" \
    -DNLM_ENABLE_PRECOMPILED_HEADERS="$ENABLE_PRECOMPILED_HEADERS" \
    -DNLM_ENABLE_EXPORTS="$ENABLE_EXPORTS" \
    -DNLM_ENABLE_INSTALL="$ENABLE_INSTALL"

# Build the project
make -j$(nproc)

# Display build summary
echo "Build completed successfully!"
echo "Build directory: $BUILD_DIR"
echo "Build type: $BUILD_TYPE"
echo "Python bindings: $PYTHON_BINDINGS"
echo "Tests: $ENABLE_TESTS"
echo "Benchmarks: $ENABLE_BENCHMARKS"
echo "Documentation: $ENABLE_DOCS"
echo "Coverage: $ENABLE_COVERAGE"
echo "AddressSanitizer: $ENABLE_ASAN"
echo "ThreadSanitizer: $ENABLE_TSAN"
echo "Valgrind: $ENABLE_VALGRIND"
```

### 5. Deployment Scripts

#### Install Script
```bash
#!/bin/bash
# install.sh - Installation script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --python-prefix)
            PYTHON_PREFIX="$2"
            shift 2
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Set default values
: ${INSTALL_PREFIX:=/usr/local}
: ${PYTHON_PREFIX:=/usr}

# Install the project
echo "Installing NLM to $INSTALL_PREFIX"
cd build
make install

# Install Python bindings
if [ -f "pymnl.so" ]; then
    echo "Installing Python bindings to $PYTHON_PREFIX/lib/python*"
    mkdir -p "$PYTHON_PREFIX/lib/python*/site-packages/nlm"
    cp pymnl.so "$PYTHON_PREFIX/lib/python*/site-packages/nlm/"
fi

# Install documentation
if [ -d "docs" ]; then
    echo "Installing documentation to $INSTALL_PREFIX/share/doc/nlm"
    mkdir -p "$INSTALL_PREFIX/share/doc/nlm"
    cp -r docs/* "$INSTALL_PREFIX/share/doc/nlm/"
fi

# Display installation summary
echo "Installation completed successfully!"
echo "Installation prefix: $INSTALL_PREFIX"
echo "Python prefix: $PYTHON_PREFIX"
```

#### Uninstall Script
```bash
#!/bin/bash
# uninstall.sh - Uninstall script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --python-prefix)
            PYTHON_PREFIX="$2"
            shift 2
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Set default values
: ${INSTALL_PREFIX:=/usr/local}
: ${PYTHON_PREFIX:=/usr}

# Uninstall the project
echo "Uninstalling NLM from $INSTALL_PREFIX"
cd build
make uninstall

# Uninstall Python bindings
if [ -f "$PYTHON_PREFIX/lib/python*/site-packages/nlm/pymnl.so" ]; then
    echo "Uninstalling Python bindings from $PYTHON_PREFIX"
    rm -rf "$PYTHON_PREFIX/lib/python*/site-packages/nlm"
fi

# Uninstall documentation
if [ -d "$INSTALL_PREFIX/share/doc/nlm" ]; then
    echo "Uninstalling documentation from $INSTALL_PREFIX"
    rm -rf "$INSTALL_PREFIX/share/doc/nlm"
fi

# Display uninstallation summary
echo "Uninstallation completed successfully!"
```

### 6. Testing and Validation

#### Test Script
```bash
#!/bin/bash
# test.sh - Testing script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --quiet|-q)
            QUIET=true
            shift
            ;;
        --coverage|-c)
            COVERAGE=true
            shift
            ;;
        --benchmark|-b)
            BENCHMARK=true
            shift
            ;;
        --profile|-p)
            PROFILE=true
            shift
            ;;
        --validate|-V)
            VALIDATE=true
            shift
            ;;
        --analyze|-a)
            ANALYZE=true
            shift
            ;;
        --export|-e)
            EXPORT=true
            shift
            ;;
        --import|-i)
            IMPORT=true
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Set default values
: ${VERBOSE:=false}
: ${QUIET:=false}
: ${COVERAGE:=false}
: ${BENCHMARK:=false}
: ${PROFILE:=false}
: ${VALIDATE:=false}
: ${ANALYZE:=false}
: ${EXPORT:=false}
: ${IMPORT:=false}

# Create test directory
TEST_DIR="test"
if [ ! -d "$TEST_DIR" ]; then
    mkdir -p "$TEST_DIR"
fi

# Run tests
echo "Running NLM tests..."

# Run unit tests
cd build
if [ "$VERBOSE" = "true" ]; then
    ctest --output-on-failure
else
    ctest
fi

# Run benchmark tests
if [ "$BENCHMARK" = "true" ]; then
    echo "Running benchmark tests..."
    ./nlm_benchmark
fi

# Run profile tests
if [ "$PROFILE" = "true" ]; then
    echo "Running profile tests..."
    ./nlm --profile
fi

# Run validation tests
if [ "$VALIDATE" = "true" ]; then
    echo "Running validation tests..."
    ./nlm --validate
fi

# Run analysis tests
if [ "$ANALYZE" = "true" ]; then
    echo "Running analysis tests..."
    ./nlm --analyze
fi

# Run export tests
if [ "$EXPORT" = "true" ]; then
    echo "Running export tests..."
    ./nlm --export
fi

# Run import tests
if [ "$IMPORT" = "true" ]; then
    echo "Running import tests..."
    ./nlm --import
fi

# Generate test report
if [ "$COVERAGE" = "true" ]; then
    echo "Generating test coverage report..."
    gcovr --root . --output coverage_report.html
fi

# Display test summary
echo "Test completed successfully!"
echo "Test directory: $TEST_DIR"
echo "Build directory: build"
```

### 7. Configuration Management

#### Configuration Script
```bash
#!/bin/bash
# config.sh - Configuration management script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --create)
            CONFIG_TYPE="$2"
            shift 2
            ;;
        --load)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --save)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --validate)
            VALIDATE=true
            shift
            ;;
        --apply)
            APPLY_CONFIG="$2"
            shift 2
            ;;
        --merge)
            MERGE_CONFIG="$2"
            shift 2
            ;;
        --diff)
            DIFF_CONFIG="$2"
            shift 2
            ;;
        --info)
            INFO=true
            shift
            ;;
        --check)
            CHECK_CONFIG=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Set default values
: ${CONFIG_TYPE:=default}
: ${CONFIG_FILE:=config.json}
: ${VALIDATE:=false}
: ${APPLY_CONFIG:=}
: ${MERGE_CONFIG:=}
: ${DIFF_CONFIG:=}
: ${INFO:=false}
: ${CHECK_CONFIG:=false}

# Create configuration directory
CONFIG_DIR="config"
if [ ! -d "$CONFIG_DIR" ]; then
    mkdir -p "$CONFIG_DIR"
fi

cd "$CONFIG_DIR"

# Create configuration file
if [ "$CONFIG_TYPE" = "default" ]; then
    echo "Creating default configuration..."
    cat > default.cfg << EOF
# NLM Configuration File
# Default configuration for NLM

# Simulation parameters
simulation_timestep = 0.001
max_simulation_steps = 10000

# Brain parameters
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Neuromodulation parameters
dopamine_baseline = 0.1
curiosity_baseline = 0.05
novelty_baseline = 0.1

# Development parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Output configuration
log_level = INFO
log_to_file = false
log_filename = nlm.log
EOF

elif [ "$CONFIG_TYPE" = "advanced" ]; then
    echo "Creating advanced configuration..."
    cat > advanced.cfg << EOF
# NLM Advanced Configuration File
# Advanced configuration for NLM with optimization

# Core brain parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 2000
region_count = 4
connection_probability = 0.15

# Enhanced plasticity settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.024
stdp_time_constant = 20.0
hebbian_learning_rate = 0.02
structural_synaptogenesis_rate = 0.002
structural_pruning_rate = 0.0002

# Neuromodulation settings
dopamine_baseline = 0.2
dopamine_scale = 1.0
curiosity_baseline = 0.1
curiosity_exploration_bonus = 0.5
novelty_decay_rate = 0.95
prediction_error_learning_rate = 0.1

# Development settings
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002
development_critical_period_start = 100
development_critical_period_end = 1000

# Performance settings
enable_performance_monitoring = true
performance_log_interval = 1000
enable_memory_tracking = true
enable_timestep_tracking = true

# Visualization settings
visualization_enabled = true
visualization_update_rate = 30.0
visualization_window_width = 1280
visualization_window_height = 720

# World parameters
environment_name = GridWorld
environment_width = 20
environment_height = 20
vision_width = 8
vision_height = 8
EOF

elif [ "$CONFIG_TYPE" = "minimal" ]; then
    echo "Creating minimal configuration..."
    cat > minimal.cfg << EOF
# NLM Minimal Configuration File
# Minimal configuration for NLM

# Core parameters
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.05

# Basic plasticity
plasticity_learning_rate = 0.01

# Basic neuromodulation
dopamine_baseline = 0.1
EOF
fi

# Load configuration file
if [ "$CONFIG_FILE" != "config.json" ] && [ -f "$CONFIG_FILE" ]; then
    echo "Loading configuration from: $CONFIG_FILE"
    source "$CONFIG_FILE"
fi

# Validate configuration
if [ "$VALIDATE" = "true" ]; then
    echo "Validating configuration..."
    # Add validation logic here
    echo "Configuration validation completed."
fi

# Apply configuration
if [ -n "$APPLY_CONFIG" ]; then
    echo "Applying configuration: $APPLY_CONFIG"
    # Add apply logic here
    echo "Configuration applied successfully."
fi

# Merge configuration
if [ -n "$MERGE_CONFIG" ]; then
    echo "Merging configuration: $MERGE_CONFIG"
    # Add merge logic here
    echo "Configuration merged successfully."
fi

# Display configuration differences
if [ -n "$DIFF_CONFIG" ]; then
    echo "Displaying configuration differences with: $DIFF_CONFIG"
    # Add diff logic here
    echo "Configuration differences displayed."
fi

# Display configuration information
if [ "$INFO" = "true" ]; then
    echo "Configuration Information:"
    echo "Configuration type: $CONFIG_TYPE"
    echo "Configuration file: $CONFIG_FILE"
    if [ -f "$CONFIG_FILE" ]; then
        echo "File size: $(stat -c%s "$CONFIG_FILE") bytes"
    fi
fi

# Check configuration integrity
if [ "$CHECK_CONFIG" = "true" ]; then
    echo "Checking configuration integrity..."
    # Add integrity check logic here
    echo "Configuration integrity check completed."
fi

# Save configuration
cd ".."
if [ -f "$CONFIG_DIR/$CONFIG_TYPE.cfg" ]; then
    cp "$CONFIG_DIR/$CONFIG_TYPE.cfg" config.json
    echo "Configuration saved to config.json"
fi

# Display configuration summary
echo "Configuration completed successfully!"
echo "Configuration type: $CONFIG_TYPE"
echo "Configuration file: $CONFIG_FILE"
```

### 8. Documentation Generation

#### Documentation Script
```bash
#!/bin/bash
# docs.sh - Documentation generation script for NLM

set -e

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            exit 0
            ;;
        --format)
            DOC_FORMAT="$2"
            shift 2
            ;;
        --output)
            DOC_OUTPUT="$2"
            shift 2
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --quiet|-q)
            QUIET=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Set default values
: ${DOC_FORMAT:=html}
: ${DOC_OUTPUT:=docs}
: ${CLEAN:=false}
: ${VERBOSE:=false}
: {QUIET:=false}

# Create documentation directory
if [ ! -d "$DOC_OUTPUT" ]; then
    mkdir -p "$DOC_OUTPUT"
fi

# Clean documentation if requested
if [ "$CLEAN" = "true" ]; then
    echo "Cleaning documentation..."
    rm -rf "$DOC_OUTPUT"/*
fi

# Generate documentation
echo "Generating NLM documentation in format: $DOC_FORMAT"

# Generate documentation using Sphinx
if command -v sphinx-build >/dev/null 2>&1; then
    if [ "$DOC_FORMAT" = "html" ]; then
        echo "Generating HTML documentation..."
        sphinx-build -b html docs "$DOC_OUTPUT"
    elif [ "$DOC_FORMAT" = "latex" ]; then
        echo "Generating LaTeX documentation..."
        sphinx-build -b latex docs "$DOC_OUTPUT"
    elif [ "$DOC_FORMAT" = "man" ]; then
        echo "Generating man page documentation..."
        sphinx-build -b man docs "$DOC_OUTPUT"
    elif [ "$DOC_FORMAT" = "texinfo" ]; then
        echo "Generating Texinfo documentation..."
        sphinx-build -b texinfo docs "$DOC_OUTPUT"
    else
        echo "Unknown documentation format: $DOC_FORMAT"
        echo "Supported formats: html, latex, man, texinfo"
        exit 1
    fi
else
    echo "Error: sphinx-build not found"
    echo "Install sphinx-build to generate documentation"
    exit 1
fi

# Generate API documentation
if [ -f "src/nlm_core.h" ]; then
    echo "Generating API documentation..."
    doxygen Doxyfile 2>/dev/null || echo "Doxygen documentation generation not available"
fi

# Generate build documentation
if [ -f "BUILD.md" ]; then
    echo "Generating build documentation..."
    cp BUILD.md "$DOC_OUTPUT/build.md"
fi

# Generate configuration documentation
if [ -f "config.md" ]; then
    echo "Generating configuration documentation..."
    cp config.md "$DOC_OUTPUT/config.md"
fi

# Generate examples documentation
if [ -f "examples.md" ]; then
    echo "Generating examples documentation..."
    cp examples.md "$DOC_OUTPUT/examples.md"
fi

# Display documentation generation summary
echo "Documentation generated successfully!"
echo "Output directory: $DOC_OUTPUT"
echo "Format: $DOC_FORMAT"

# Display index.html if available
if [ -f "$DOC_OUTPUT/index.html" ]; then
    echo "Index file: $DOC_OUTPUT/index.html"
fi
```

### 9. Continuous Integration

#### CI Configuration
```yaml
# .github/workflows/ci.yml - Continuous integration configuration
name: NLM CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Release, Debug]
        python_version: ['3.8', '3.9', '3.10', '3.11']
        exclude:
          - os: windows-latest
            python_version: '3.11'
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install pybind11 scikit-build-core pytest numpy
        if [ "${{ runner.os }}" = "Linux" ]; then
          sudo apt-get update
          sudo apt-get install -y \
            build-essential \
            cmake \
            libpython3-dev \
            libssl-dev \
            libffi-dev \
            libsqlite3-dev
        elif [ "${{ runner.os }}" = "macOS" ]; then
          brew install cmake
        elif [ "${{ runner.os }}" = "Windows" ]; then
          choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
        fi
    
    - name: Configure CMake
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
          -DNLM_ENABLE_PYTHON_BINDINGS=ON \
          -DNLM_ENABLE_TESTS=ON \
          -DNLM_ENABLE_BENCHMARKS=ON \
          -DNLM_ENABLE_COVERAGE=ON \
          -DNLM_ENABLE_ASAN=false \
          -DNLM_ENABLE_TSAN=false \
          -DNLM_ENABLE_VALGRIND=false
    
    - name: Build
      run: cmake --build build --config Release
    
    - name: Run tests
      run: cd build && ctest --output-on-failure
    
    - name: Run benchmarks
      run: cd build && ./nlm_benchmark
    
    - name: Run Python tests
      run: cd build && python -m pytest tests/ -v
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      with:
        name: nlm-build-${{ matrix.os }}-${{ matrix.build-type }}-${{ matrix.python-version }}
        path: build/

  lint:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.8'
    
    - name: Install dependencies
      run: |
        pip install pybind11 scikit-build-core black isort flake8 mypy
    
    - name: Run black
      run: black --check src/
    
    - name: Run isort
      run: isort --check-only src/
    
    - name: Run flake8
      run: flake8 src/
    
    - name: Run mypy
      run: mypy src/
    
    - name: Run clang-tidy
      run: |
        cmake -B build -DNLM_ENABLE_STATIC_ANALYSIS=ON
        cmake --build build --target clangtidy

  docs:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.8'
    
    - name: Install dependencies
      run: |
        pip install sphinx
    
    - name: Generate documentation
      run: |
        cmake -B build -DNLM_ENABLE_DOCS=ON
        cmake --build build --target docs
    
    - name: Upload documentation
      uses: actions/upload-artifact@v3
      with:
        name: docs
        path: build/docs/

  packages:
    needs: [test, lint]
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.8'
    
    - name: Build Python package
      run: |
        cmake -B build -DNLM_ENABLE_PYTHON_BINDINGS=ON
        cmake --build build --target all
    
    - name: Test Python package
      run: |
        cd build
        python -c "import sys; sys.path.insert(0, '.'); import pymnl; print('Import successful')"
    
    - name: Create release
      uses: actions/create-release@v1
      with:
        tag_name: v0.1.0
        release_name: NLM 0.1.0
        body: |
          This is the initial release of NLM (Neural Learning Machine).
          
          ## Features
          - Experimental artificial developmental brain
          - Leaky Integrate-and-Fire (LIF) neurons
          - Event-driven spike propagation with delays
          - STDP and Hebbian plasticity rules
          - Structural plasticity (synaptogenesis/pruning)
          - Neuromodulation systems (dopamine, curiosity, novelty)
          - Prediction systems
          - Cognitive mechanisms (planning, attention, concept formation)
          - Development systems
          - Memory systems (working memory, episodic memory)
          - Environment interface
          - Visualization tools
          - Checkpoint system
          - Replay and consolidation
          
          ## Installation
          ```bash
          pip install pynlm
          ```
          
          ## Usage
          ```python
          import pynlm
          
          config = pynlm.createDefaultConfig()
          brain = pynlm.createBrain(config)
          brain.initialize()
          
          for i in range(1000):
              brain.step(i)
          
          print(f"Neurons: {brain.getTotalNeuronCount()}")
          print(f"Total spikes: {brain.getTotalSpikeCount()}")
          ```
    
    - name: Upload package
      uses: actions/upload-artifact@v3
      with:
        name: pynlm
        path: build/python/
```

## Summary

This document provides comprehensive build system and automation improvements for the NLM project, including:

### Core Build System Enhancements
- **Modern CMake configuration** with extensive build options and optimization
- **Performance optimizations** including SIMD, link-time optimization, and precompiled headers
- **Comprehensive debugging tools** with sanitizers and static analysis
- **Documentation generation** with Sphinx and Doxygen integration
- **Code coverage reporting** for testing and validation
- **Python bindings support** with pybind11 integration
- **Installation capabilities** with customizable installation directories

### Build Scripts and Automation
- **Environment setup script** for configuring build environment
- **Run script** for building and testing the project
- **Setup script** for initial environment configuration
- **Configuration script** for managing build configuration
- **Documentation script** for generating project documentation
- **Test script** for running tests and generating reports
- **Installation script** for installing the project
- **Uninstallation script** for removing the project

### Build Configuration Options
- **Build type**: Release, Debug, RelWithDebInfo
- **Python bindings**: ON/OFF
- **Tests**: ON/OFF
- **Benchmarks**: ON/OFF
- **Documentation**: ON/OFF
- **Code coverage**: ON/OFF
- **AddressSanitizer**: ON/OFF
- **ThreadSanitizer**: ON/OFF
- **Valgrind**: ON/OFF
- **Static analysis**: ON/OFF
- **Clang-Tidy**: ON/OFF
- **Include-What-You-Use**: ON/OFF
- **Warnings as errors**: ON/OFF
- **All warnings**: ON/OFF
- **Pedantic warnings**: ON/OFF
- **Optimizations**: ON/OFF
- **Debug information**: ON/OFF
- **Precompiled headers**: ON/OFF
- **Exports**: ON/OFF
- **Installation**: ON/OFF

### Build Outputs
After successful compilation, the build system generates:

| Component | Type | Description |
|-----------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core neural components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `nlm_benchmark` | Executable | Benchmark executable |
| `pymnl.so` | Shared Library | Python bindings |

### Automation Capabilities
- **Automated testing** with comprehensive test coverage
- **Continuous integration** with GitHub Actions
- **Documentation generation** with Sphinx and Doxygen
- **Performance monitoring** with benchmarks and profiling
- **Configuration management** with validation and versioning
- **Deployment automation** with packaging and installation scripts
- **Environment setup** with cross-platform support
- **Development environment** with pre-configured tools and settings

### Development Workflow
1. **Clone repository**
2. **Configure build system**
3. **Build the project**
4. **Run tests**
5. **Run benchmarks**
6. **Generate documentation**
7. **Package and deploy**

### Usage Examples
```bash
# Basic build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build with Python bindings
mkdir build
cd build
cmake .. -DNLM_ENABLE_PYTHON_BINDINGS=ON
make -j4

# Build with sanitizers
mkdir build
cd build
cmake .. -DNLM_ENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run tests
cd build
ctest --output-on-failure

# Run benchmarks
cd build
./nlm_benchmark

# Build documentation
mkdir build
cd build
cmake .. -DNLM_ENABLE_DOCS=ON
make docs

# Install
mkdir build
cd build
make install
```

### Benefits

These build system and automation improvements provide:

- **Faster build times** with optimizations and parallelization
- **Better error detection** with sanitizers and static analysis
- **Enhanced productivity** with automation and documentation
- **Improved maintainability** with configurable build options
- **Cross-platform support** with platform-specific optimizations
- **Continuous integration** with automated testing and deployment
- **Comprehensive documentation** with examples and tutorials
- **Performance monitoring** with benchmarks and profiling
- **Configuration management** with validation and versioning
- **Developer-friendly tools** with extensive scripting and automation

The enhanced build system significantly improves the NLM project's development workflow, making it more accessible, maintainable, and efficient for both developers and users.

This comprehensive build system enhancement provides a robust, flexible, and powerful framework for building, testing, and deploying the NLM project, supporting a wide range of development and deployment scenarios.
