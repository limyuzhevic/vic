# Development Tools and IDE Support Documentation

This document provides comprehensive documentation for the development tools, IDE configurations, and setup scripts for the NLM project.

## Overview

NLM development includes a complete toolchain for C++ neural network simulation with:

- Modern CMake build system
- Comprehensive test suite
- Performance monitoring and profiling
- Development environment setup scripts
- IDE integrations for multiple platforms
- Code quality and analysis tools

## Development Environment Setup

### Prerequisites

#### Required Software

| Tool | Version | Purpose |
|------|---------|---------|
| CMake | 3.20+ | Build system and project configuration |
| GCC/G++ | 11+ | C++ compiler |
| Clang | 12+ | Optional compiler alternative |
| Git | Latest | Version control |
| Python | 3.8+ | Python bindings and utilities |

#### Platform-Specific Requirements

**Linux**
```bash
# Install dependencies
sudo apt update
sudo apt install cmake build-essential git python3 python3-pip
```

**macOS**
```bash
# Install dependencies via Homebrew
brew install cmake git python3
```

**Windows**
```powershell
# Install via Chocolatey
choco install cmake git python
```

### Initial Setup

#### Automated Setup Script

The project provides an automated setup script that configures the development environment:

```bash
# Navigate to project directory
cd /workspace/app

# Run setup script (creates build directory and configures CMake)
./scripts/dev_setup.sh
```

#### Manual Setup

1. **Create build directory**
```bash
mkdir -p build
```

2. **Configure CMake**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DBUILD_DOCS=ON
```

3. **Build the project**
```bash
cmake --build build --parallel
```

4. **Run tests**
```bash
cd build && ctest --output-on-failure
```

## IDE Integration

### Visual Studio Code

#### Configuration

The project includes full VS Code integration with:

- **CMake integration**: Native CMake support
- **IntelliSense**: Intelligent code completion and type analysis
- **Debugging**: Comprehensive launch configurations
- **Code quality**: Linting and formatting integration
- **Git integration**: Version control support

#### Installation

1. Open VS Code
2. Navigate to Extensions (Ctrl+Shift+X)
3. Install:
   - **ms-vscode.cpptools**: C/C++ extension pack
   - **ms-vscode.cmake-tools**: CMake integration
   - **github.copilot**: AI code assistance
   - **eamodio.gitlens**: Git visualization

#### Key Features

- **CMake Tasks**: Build, test, and documentation generation
- **Debugger Support**: GDB integration for debugging
- **Code Navigation**: Symbol lookup and go-to-definition
- **Code Formatting**: Automatic code formatting with clang-format
- **Code Analysis**: Static analysis with clang-tidy

### JetBrains CLion

#### Configuration

CLion provides excellent CMake support:

1. **Import as CMake Project**
   - Open CLion
   - Select "Import from existing CMakeLists.txt"
   - Choose `/workspace/app/CMakeLists.txt`

2. **CMake Configuration**
   - Configure build directory
   - Set compiler (GCC or Clang)
   - Enable CMake caches

#### Features

- **CMake Integration**: Native CMake support with intelligent code generation
- **Debug Integration**: GDB/LLDB integration with breakpoints and watch expressions
- **Code Analysis**: Built-in Clang-based code inspection
- **Refactoring**: Safe code refactoring capabilities
- **CMake Compiler Explorer**: Explore compiler flags and options

### Visual Studio (Windows)

#### Configuration

1. **Project Import**
   - Open Visual Studio
   - Select "Open a project or solution"
   - Navigate to `/workspace/app/CMakeLists.txt`
   - Choose "CMake" as project type

2. **IDE Settings**
   - Configure CMake generator (Ninja, Visual Studio)
   - Set build configuration (Debug/Release)
   - Configure IntelliSense database

#### Features

- **CMake Integration**: Full CMake support
- **IntelliSense**: Comprehensive code completion
- **Debugging**: Breakpoints, watch windows, call stack inspection
- **Code Analysis**: Static analysis and code metrics
- **Git Integration**: Built-in version control

## Development Workflow

### Core Commands

#### Build System

| Command | Description |
|---------|-------------|
| `./scripts/dev_setup.sh` | Complete development environment setup |
| `./scripts/build.sh` | Custom build automation with options |
| `./scripts/test_runner.sh` | Comprehensive test execution |
| `./scripts/workflow.sh setup` | Setup development environment |
| `./scripts/workflow.sh simulate phase6` | Run Phase 6 integration demo |
| `./scripts/workflow.sh debug` | Start debugging session |

#### Build Configurations

| Configuration | Command |
|---------------|---------|
| Debug with sanitizers | `cmake -S . -B build_debug -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON` |
| Release with docs | `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCS=ON` |
| Coverage testing | `cmake -S . -B build_coverage -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_COVERAGE=ON` |
| Profiling | `cmake -S . -B build_profile -DNLM_ENABLE_PROFILING=ON` |

#### Execution

| Command | Description |
|---------|-------------|
| `./build/nlm` | Run main neural network simulation |
| `./build/nlm_phase6_demo` | Run Phase 6 integration demo |
| `./build/nlm_test` | Run unit tests |
| `./build/nlm --help` | Show available simulation options |

### Development Best Practices

#### Code Quality

1. **Follow C++20 standards**
2. **Use data-oriented design**
3. **Write comprehensive tests**
4. **Document public APIs**
5. **Use deterministic random seeds**

#### Testing Strategy

- **Unit Tests**: Test individual components
- **Integration Tests**: Verify system connectivity
- **Performance Tests**: Measure simulation performance
- **Regression Tests**: Ensure existing functionality

#### Performance Optimization

1. **Use SIMD when available**
2. **Minimize memory allocations**
3. **Optimize cache locality**
4. **Use event-driven processing**
5. **Profile before optimizing**

## Code Quality Tools

### Static Analysis

#### Clang-Tidy

Configure clang-tidy for NLM:

```bash
# Add to .clang-tidy
Checks: *-google*,
        *-cert*,
        modernize-*,
        performance-*,
        readability-*,

# Run analysis
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy -checks=* -fix
```

#### Cppcheck

```bash
# Install and configure cppcheck
# Run analysis
find src -name "*.cpp" -o -name "*.hpp" | xargs cppcheck --enable=all --suppress=missingIncludeSystem
```

### Formatting

#### Clang-Format

Configure clang-format for consistent code style:

```bash
# Create .clang-format
BasedOnStyle: LLVM
IndentWidth: 4
UseTab: Never
BreakBeforeBraces: Allman
...

# Format code
find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### Documentation

#### Doxygen

The project uses Doxygen for API documentation:

```bash
# Generate documentation
cmake --build build --target docs

# Open documentation
cmake --build build --target docs_open
```

### Code Coverage

#### Coverage Configuration

```bash
# Configure with coverage
cmake -S . -B build_coverage -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_COVERAGE=ON

# Build and run tests
cmake --build build_coverage --parallel
cd build_coverage && ctest --output-on-failure

# Generate coverage report
cmake --build build_coverage --target coverage_report
```

## Debugging and Troubleshooting

### Common Issues

#### Build Failures

**Problem**: CMake configuration errors
**Solution**:
```bash
# Clean rebuild
rm -rf build && mkdir build && cd build && cmake .. && cmake --build .
```

**Problem**: Compiler compatibility issues
**Solution**:
```bash
# Use specific compiler flags
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-11
```

#### Runtime Errors

**Problem**: Simulation crashes
**Solution**:
```bash
# Run with debug symbols and sanitizers
cmake -S . -B build_debug -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON
cmake --build build_debug --parallel
./build_debug/nlm --log-level=debug
```

#### Performance Issues

**Problem**: Slow simulation
**Solution**:
```bash
# Profile with performance tools
./scripts/workflow.sh profile
```

### Debugging Techniques

#### GDB Debugging

```bash
# Start GDB
gdb ./build/nlm

# Common commands
break main
tbreak main.run
continue
info locals
print neuron_count
```

#### LLDB Debugging

```bash
# Start LLDB
lldb ./build/nlm

# Common commands
b main
br step
v neuron_count
```

#### Visual Studio Debugging

1. Set breakpoints in the code editor
2. Use the debug window to inspect variables
3. Use the call stack window to navigate
4. Use the local variables window to inspect state

## Performance Testing

### Performance Measurement

#### Built-in Performance Metrics

NLM provides comprehensive performance monitoring:

```bash
# Run with performance monitoring
./build/nlm --profile=perfetto

# Generate performance report
cmake --build build --target performance_report
```

#### Performance Metrics

The project tracks:

- **Simulation Speed**: Neurons processed per second
- **Memory Usage**: Peak RAM consumption
- **CPU Utilization**: Processor core usage
- **I/O Performance**: File read/write operations
- **Scalability**: Performance with increasing neuron count

### Profiling Tools

#### System-wide Profiling

**Perfetto**
- Provides system-wide performance tracing
- Includes CPU, memory, and I/O profiling
- Supports timeline visualization

**Chrome Trace**
- Browser-compatible tracing format
- Detailed performance analysis
- Interactive visualization

#### Application Profiling

**GProf**
- Function call profiling
- Performance bottlenecks identification
- Call graph analysis

**Valgrind**
- Memory profiling
- Leak detection
- Thread safety analysis

### Performance Optimization

#### Compilation Optimizations

```bash
# Optimize for speed
cmake -S . -B build -DNLM_OPT_LEVEL=3

# Optimize for size
cmake -S . -B build -DNLM_OPT_LEVEL=0

# Profile-guided optimization
cmake -S . -B build -DNLM_ENABLE_PGO=ON
```

#### Runtime Optimizations

1. **Use SIMD instructions**
2. **Cache-friendly data layouts**
3. **Minimize branching**
4. **Use efficient data structures**

## Project Structure Guide

### Directory Structure

```
NLM/
├── CMakeLists.txt              # Main build configuration
├── README.md                   # Project overview
├── README_DEV.md                # Development guide
├── cmake/                     # CMake configuration
│   ├── NLMConfig.cmake.in      # CMake configuration template
│   ├── Doxyfile.in             # Doxygen configuration
│   ├── NLMSetup.cmake          # CMake setup utilities
│   └── Examples.cmake          # Configuration examples
├── src/                        # Source code
│   ├── core/                  # Core utilities
│   ├── brain/                 # Brain components
│   ├── dynamics/              # Neural dynamics
│   ├── plasticity/            # Plasticity rules
│   ├── development/           # Developmental system
│   ├── neuromodulation/       # Neuromodulators
│   ├── memory/                # Memory systems
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
│   └── test_*.cpp             # Individual tests
├── python/                     # Python bindings
│   ├── bindings.cpp          # Main Python interface
│   └── CMakeLists.txt        # Python bindings configuration
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md        # System architecture
│   ├── SCIENCE.md            # Scientific background
│   └── ...                   # Other documentation
├── configs/                   # Configuration files
└── scripts/                   # Development scripts
    ├── dev_setup.sh           # Development environment setup
    ├── build.sh              # Build automation
    ├── test_runner.sh        # Test execution
    └── workflow.sh           # Development workflow
```

### Key Files

#### Source Code

- **`src/core/`**: Core types and utilities
- **`src/brain/`**: Neural network components
- **`src/dynamics/`**: Neuron dynamics models
- **`src/plasticity/`**: Learning rules
- **`src/development/`**: Developmental stages
- **`src/neuromodulation/`**: Neuromodulator systems
- **`src/memory/`**: Memory systems (working, episodic, associative)
- **`src/prediction/`**: Temporal prediction systems
- **`src/cognition/`**: Cognitive mechanisms
- **`src/sensory/`**: Sensory processing
- **`src/motor/`**: Motor control
- **`src/environment/`**: Environment interface
- **`src/experiments/`**: Experiment framework
- **`src/visualization/`**: Visualization tools

#### Build System

- **`CMakeLists.txt`**: Main build configuration
- **`cmake/`**: CMake utilities and configuration

#### Testing

- **`tests/`**: Comprehensive test suite
- **`scripts/test_runner.sh`**: Test execution script

#### Documentation

- **`docs/`**: Technical documentation
- **`README_DEV.md`**: Development guide

#### Python Bindings

- **`python/`**: Python bindings for NLM

## Common Development Shortcuts

### Shell Shortcuts

| Shortcut | Command |
|----------|---------|
| `cd` | `cd /workspace/app` |
| `build` | `cd build && ls -la` |
| `test` | `cd build && ctest --output-on-failure` |
| `run` | `./build/nlm --help` |

### IDE Shortcuts

| Action | VS Code | CLion | Visual Studio |
|--------|---------|-------|---------------|
| Open project | `File > Open Folder` | `File > Open > CMakeLists.txt` | `File > Open > CMakeLists.txt` |
| Build all | `Ctrl+Shift+B` | `Build > Build All` | `Build > Build All` |
| Run tests | `Ctrl+Shift+F5` | `Run > Run All Tests` | `Debug > Start Debugging` |
| Debug | `F5` | `Debug > Debug` | `Debug > Start Debugging` |

### Development Workflows

#### Local Development

```bash
# 1. Setup environment
./scripts/dev_setup.sh

# 2. Edit code
nano src/core/Types.cpp

# 3. Build changes
cmake --build build --parallel

# 4. Test changes
cd build && ctest --output-on-failure

# 5. Run simulation
./build/nlm --simulation-duration=3600
```

#### Debugging Development

```bash
# 1. Setup debug environment
./scripts/workflow.sh setup

# 2. Start debugging
./scripts/workflow.sh debug

# 3. Analyze results
./scripts/workflow.sh analyze
```

#### Performance Analysis

```bash
# 1. Profile simulation
./scripts/workflow.sh profile

# 2. Check performance
./scripts/workflow.sh quality
```

## Contributing

### Code Quality Standards

1. **Follow C++20 standards**
2. **Use consistent formatting**
3. **Write comprehensive tests**
4. **Document public APIs**
5. **Use descriptive variable names**
6. **Include error handling**
7. **Optimize for performance**
8. **Review changes with peers**

### Commit Guidelines

- Use conventional commit messages
- Include test coverage
- Document significant changes
- Reference related issues
- Include co-authored-by metadata

### Testing Guidelines

- All new code must have unit tests
- Integration tests for system components
- Performance tests for optimized paths
- Regression tests for existing functionality

## License

MIT License

## Acknowledgements

This development documentation builds upon the existing NLM project structure and CMake improvements. The development tools and IDE integrations are designed to provide a comprehensive development experience for the NLM project.