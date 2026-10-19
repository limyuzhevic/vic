# NLM Development Guide

## Overview

This document provides comprehensive development information for the NLM (Neural Learning Machine) project. NLM is an experimental computational brain project that creates neural systems learning through interaction with an environment.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Development Environment Setup](#development-environment-setup)
3. [Code Quality and Standards](#code-quality-and-standards)
4. [Debugging and Testing](#debugging-and-testing)
5. [Performance Testing](#performance-testing)
6. [IDE Configuration](#ide-configuration)
7. [Common Development Shortcuts](#common-development-shortcuts)
8. [Project Structure](#project-structure)
9. [Building and Running](#building-and-running)
10. [Contributing Guidelines](#contributing-guidelines)

## Getting Started

NLM is a **Phase 6: Final Integration** project that combines all previously developed systems into a coherent artificial brain. The project implements biologically-inspired neural computation with:

- **LIF neurons** with event-driven spike propagation
- **Multi-layer memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, novelty, curiosity)
- **Prediction systems** for temporal sequence learning
- **Cognitive mechanisms** (attention, planning, concept formation)
- **Full development stages** affecting plasticity rates

### Current Capabilities

✅ **Complete Integration**: All systems connected and functional
✅ **Checkpoint Persistence**: Save/load brain states
✅ **Memory Replay**: Episodic memory consolidation
✅ **Performance Optimization**: SIMD vectorization, memory pools
✅ **Cross-platform Support**: Windows, macOS, Linux

## Development Environment Setup

### Prerequisites

```bash
# Compiler requirements (CMake 3.20+)
gcc/g++ >= 11.0 || clang++ >= 12.0 || MSVC >= 19.29

# Python (for bindings)
python >= 3.8

# CMake 3.20 or higher
# Git
```

### Standard Development Build

```bash
# Configure with CMake
mkdir build && cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . --parallel

# Run all tests
ctest --output-on-failure
```

### Debug Build with All Features

```bash
mkdir build_debug && cd build_debug
cmake -S .. -B . \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DBUILD_DOCS=ON \
    -DNLM_ENABLE_COVERAGE=ON \
    -DNLM_ENABLE_SANITIZERS=ON
cmake --build . --parallel
```

### Python Environment Setup

```bash
# Install project dependencies
cd /workspace/app
pip install -e .[dev]

# Install Python bindings (if needed)
pip install pynlm
```

## Code Quality and Standards

### Code Style

NLM follows modern C++ best practices with the following conventions:

1. **C++20** standard with `CMAKE_CXX_STANDARD=20`
2. **Data-oriented design** for performance
3. **Deterministic behavior** with explicit random seeds
4. **Modular architecture** with clear interfaces
5. **Comprehensive error handling** and validation

### Linters and Formatters

The project supports comprehensive code analysis:

#### Available Tools

- **Clang-Tidy**: Static code analysis and linting
- **Clang-Format**: Code formatting
- **Cppcheck**: Static analysis
- **Doxygen**: Documentation generation
- **CTest**: Unit testing framework

#### Running Code Analysis

```bash
# Run static analysis (if configured)
# Add specific targets or commands as needed
cmake --build build --target lint
```

#### Code Coverage

```bash
# Generate coverage report
cmake --build build --target coverage_report
```

## Debugging and Testing

### Running Tests

NLM has a comprehensive test suite organized by categories:

```bash
# Run all tests
cmake --build build --target run_all_tests

# Run specific test categories
cmake --build build --target unit_tests
cmake --build build --target integration_tests
cmake --build build --target performance_tests
```

### Test Categories

- **Unit Tests**: Individual components (neurons, synapses, memory)
- **Integration Tests**: System connectivity and integration
- **Performance Tests**: Benchmark measurements and profiling

### Debugging Tips

1. **Use Debug Builds**: Always build with `-DCMAKE_BUILD_TYPE=Debug` for debugging
2. **Enable Sanitizers**: Use `-DNLM_ENABLE_SANITIZERS=ON` for memory error detection
3. **Checkpoint Debugging**: Use `--checkpoint-debug` flag to analyze saved states
4. **Performance Profiling**: Use `--profile` flag for performance measurements

### Common Debugging Commands

```bash
# Run with visualization
./nlm --visualize

# Run with logging
./nlm --log-level=debug

# Run with specific simulation parameters
./nlm --simulation-duration=3600 --time-step=0.001
```

## Performance Testing

### Performance Measurement

NLM includes built-in performance measurement capabilities:

```bash
# Run performance benchmarks
cmake --build build --target performance_tests

# Profile with specific tools
./nlm --profile=perfetto
./nlm --profile=chrome-trace
```

### Performance Metrics

The project tracks:

- **Simulation Speed**: Neurons per second, spikes processed
- **Memory Usage**: Peak RAM, memory allocation patterns
- **CPU Utilization**: Processor core usage
- **I/O Performance**: File read/write operations

### Optimization Targets

```bash
# Optimize for speed
cmake -S .. -B build -DNLM_OPT_LEVEL=3

# Optimize for size
cmake -S .. -B build -DNLM_OPT_LEVEL=0

# Profile-guided optimization
cmake -S .. -B build -DNLM_ENABLE_PGO=ON
```

## IDE Configuration

### Visual Studio Code

The project includes VS Code configuration with intellisense and debugging support:

#### Key Features

- **IntelliSense**: C/C++ language support
- **Code Navigation**: Symbol lookup, go-to-definition
- **Debugging**: Launch configurations for all targets
- **Code Quality**: Linting and formatting integration
- **Git Integration**: Version control support

#### Installation Steps

1. Install the [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
2. Open the project folder in VS Code
3. The `.vscode/settings.json` and `launch.json` files will be automatically detected

#### Recommended Extensions

```json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cpptools-extension-pack",
        "srcclr.cpptools-linting",
        "waderyan.cmake-tools",
        "eamodio.gitlens",
        "LeXtudio.restructuredtext-vscode"
    ]
}
```

### JetBrains CLion

For JetBrains IDE users:

#### Import as CMake Project

1. Open CLion
2. Select "Import from existing CMakeLists.txt"
3. Choose `/workspace/app/CMakeLists.txt`
4. Configure build directory and compiler

#### CLion-Specific Features

- **CMake Integration**: Full CMake support with intelligent code generation
- **Debug Integration**: GDB/LLDB integration with breakpoints and watch expressions
- **Code Analysis**: Built-in Clang-based code inspection
- **Refactoring**: Safe code refactoring capabilities

### Visual Studio (Windows)

#### Opening the Project

1. Open Visual Studio
2. Select "Open a project or solution"
3. Navigate to `/workspace/app/CMakeLists.txt`
4. Choose "CMake" as the project type

#### VS Features

- **CMake Integration**: Native CMake support
- **IntelliSense**: Comprehensive code completion
- **Debugging**: Breakpoints, watch windows, call stack inspection
- **Code Analysis**: Static analysis and code metrics

## Common Development Shortcuts

### Build and Run

| Action | Command |
|--------|---------|
| Configure release build | `mkdir build && cd build && cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release && cmake --build .` |
| Configure debug build | `mkdir build_debug && cd build_debug && cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug` |
| Build all | `cmake --build . --parallel` |
| Run all tests | `ctest --output-on-failure` |
| Build docs | `cmake --build . --target docs` |

### Testing and Debugging

| Action | Command |
|--------|---------|
| Run Phase 6 demo | `./nlm_phase6_demo` |
| Run unit tests | `ctest -R unit_tests` |
| Run integration tests | `ctest -R integration_tests` |
| Run performance tests | `ctest -R performance_tests` |
| Debug with GDB | `gdb ./nlm` |
| Debug with LLDB | `lldb ./nlm` |

### Development Workflow

| Action | Command |
|--------|---------|
| Create new target | `cd /workspace/app && echo "add_custom_target(new_target COMMAND echo Running)" >> CMakeLists.txt` |
| Generate documentation | `cmake --build build --target docs_open` |
| Clean build | `rm -rf build && mkdir build && cd build && cmake ..` |

## Project Structure

### Main Components

```
NLM/
├── CMakeLists.txt              # Main build configuration (365 lines)
├── README.md                   # Main project documentation
├── README_DEV.md                # Development guide (this file)
├── cmake/                     # CMake configuration directory
│   ├── NLMConfig.cmake.in      # CMake configuration template
│   ├── Doxyfile.in             # Doxygen configuration template
│   ├── NLMSetup.cmake          # CMake setup utilities
│   ├── GenerateVSProject.cmake  # Visual Studio project generation
│   └── Examples.cmake          # Configuration examples
├── src/                        # Source code
│   ├── core/                  # Core utilities
│   ├── brain/                 # Brain components
│   ├── dynamics/              # Neural dynamics
│   ├── plasticity/            # Plasticity rules
│   ├── development/           # Developmental system
│   ├── neuromodulation/       # Neuromodulators
│   ├── memory/               # Memory systems
│   ├── prediction/            # Prediction systems
│   ├── cognition/            # Cognitive mechanisms
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework
│   └── visualization/        # Visualization
├── tests/                      # Test suite
│   ├── CMakeLists.txt         # Tests configuration
│   ├── test_main.cpp          # Test runner
│   ├── test_*.cpp             # Individual tests
│   └── test_config_validation.cpp  # Configuration validation
├── python/                     # Python bindings
│   ├── bindings.cpp          # Main Python interface
│   └── CMakeLists.txt        # Python bindings configuration
├── docs/                       # Technical documentation
│   ├── ARCHITECTURE.md        # System architecture
│   ├── SCIENCE.md            # Scientific background
│   └── ...                   # Other documentation
├── configs/                   # Configuration files
└── examples/                  # Example configurations
```

### Key Directories

- **`src/`**: Main source code with modular architecture
- **`tests/`**: Comprehensive test suite with coverage support
- **`cmake/`**: Modern CMake configuration files
- **`docs/`**: Technical and scientific documentation
- **`python/`**: Python bindings for NLM

## Building and Running

### Building

#### Standard Build

```bash
mkdir build && cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

#### Build Options

```bash
# With tests
cmake -S .. -B build -DBUILD_TESTS=ON

# With documentation
cmake -S .. -B build -DBUILD_DOCS=ON

# For profiling
cmake -S .. -B build -DNLM_ENABLE_PROFILING=ON

# Cross-platform build
cmake -S .. -B build -DNLM_PLATFORM_WINDOWS=ON
```

### Running

#### Phase 6 Demo

```bash
# Run comprehensive integration test
./nlm_phase6_demo

# Run with specific parameters
./nlm_phase6_demo --duration=3600 --steps=100
```

#### Individual Components

```bash
# Run neural network simulation
./nlm --simulation-duration=1800 --visualize

# Run development experiment
./nlm_phase3_demo --developmental-stages=5
```

#### Python Bindings

```python
import pynlm

# Create NLM instance
brain = pynlm.NeuralLearningMachine()

# Run simulation
brain.run(duration=1000)

# Access memory systems
memory = brain.working_memory
episodic = brain.episodic_memory
```

## Contributing Guidelines

### Code Quality Standards

1. **C++20**: Use modern C++20 features where appropriate
2. **Modular Design**: Create reusable components with clear interfaces
3. **Deterministic**: Ensure reproducible results with explicit random seeds
4. **Performance**: Optimize for the target platform (SIMD, cache-friendly)
5. **Documentation**: Add Doxygen comments for public APIs
6. **Testing**: Include unit tests for new code

### Testing Requirements

- **New Components**: Must include comprehensive unit tests
- **Integration**: Test with existing systems to ensure compatibility
- **Performance**: Include performance benchmarks where applicable
- **Documentation**: Document public APIs with Doxygen

### Commit Standards

- **Conventional Commits**: Use conventional commit messages
- **Clear Descriptions**: Describe what and why, not just what
- **Co-authored-by**: Include "co-authored-by: openhands <openhands@all-hands.dev>" for contributions
- **Tests**: Include relevant tests in commit

### Review Process

1. **Code Review**: All changes require review by project maintainers
2. **Testing**: Comprehensive testing including performance tests
3. **Documentation**: Update relevant documentation
4. **Integration**: Test with existing codebase

## Performance Considerations

### Simulation Performance

NLM is optimized for large-scale neural simulation:

1. **Memory Efficiency**: Uses memory pools and event queues
2. **Parallel Processing**: SIMD vectorization and thread pools
3. **Cache Locality**: Data-oriented design for CPU cache efficiency
4. **Event-Driven**: Efficient spike processing and synaptic updates

### Scalability

The project supports:

- **100+ neurons**: Real-time simulation
- **1000+ neurons**: Approximate real-time
- **10000+ neurons**: With optimizations
- **100000+ neurons**: With distributed computing

### Profiling Tools

Available profiling options:

- **Perfetto**: System-wide performance tracing
- **Chrome Trace**: Browser-compatible tracing
- **GProf**: Function call profiling
- **Valgrind**: Memory profiling and leak detection

## Troubleshooting

### Common Issues

#### Build Failures

**Problem**: CMake configuration errors
**Solution**: 
```bash
# Clean rebuild
cd /workspace/app && rm -rf build && mkdir build && cd build && cmake .. && cmake --build .
```

**Problem**: Missing compiler features
**Solution**: Update compiler or configure with specific features:
```bash
cmake -S .. -B build -DCMAKE_CXX_FLAGS="-march=native -O3"
```

#### Runtime Errors

**Problem**: Simulation crashes
**Solution**:
```bash
# Run with debug symbols and sanitizers
mkdir build_debug && cd build_debug
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON
cmake --build . --parallel
./nlm --log-level=debug
```

**Problem**: Memory issues
**Solution**:
```bash
# Monitor memory usage
gdb ./nlm
(gdb) run --memory-monitor
```

#### IDE Issues

**Problem**: IntelliSense not working
**Solution**: 
```bash
# Regenerate IntelliSense data
cmake --build build --target IntelliSense
```

## License

MIT License

## Authors

Research project — See docs for scientific background.