# NLM Development Workflow

This document outlines the recommended workflow for developing and contributing to the NLM project.

## Project Structure

```
/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_99f67dc5-db51-4dd9-b264-b7fef37dba79
├── CMakeLists.txt                    # Main CMake configuration (modern target-based)
├── cmake/                           # CMake modules and configuration
│   ├── NLMConfig.cmake.in          # Package configuration template
│   └── NLMPythonConfig.cmake.in    # Python bindings configuration
├── src/                             # Source code
│   ├── core/                       # Core neural computation
│   ├── brain/                      # Neural computation
│   ├── agent/                      # Agent interfaces
│   ├── world/                      # Simulation world
│   └── experiments/                # Experimental features
├── tests/                           # Unit tests
├── python/                          # Python bindings
├── docs/                            # Documentation
├── CMakeUsage.md                    # This documentation
├── easy_usage.md                    # Quick start guide
├── build.sh                         # Build and test script
├── benchmark.sh                     # Performance benchmark script
└── configs/                        # Configuration files
```

## Development Workflow

### 1. Initial Setup

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DNLM_BUILD_TESTS=ON
```

### 2. Building the Project

```bash
# Build all targets
cmake --build .

# Build specific targets
cmake --build . --target nlm_core
cmake --build . --target nlm_agent

# Build and run tests
cmake --build . --target test
ctest

# Install to system location (requires sudo)
sudo cmake --install . --prefix /usr/local
```

### 3. Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R "test_type"

# Run tests with verbose output
ctest -V
```

### 4. Python Development

```bash
# Build Python bindings
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON=ON
cmake --build .

# Test Python bindings
python3 -c "import pynlm; print(pynlm.__version__)"
```

### 5. Performance Benchmarking

```bash
# Run benchmarks
./benchmark.sh

# Custom benchmarks
./benchmark.sh --benchmarks basic_connectivity,plasticity_learning --episodes 10
```

### 6. Configuration

#### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `NLM_BUILD_TESTS` | Enable test builds | ON |
| `NLM_BUILD_PYTHON` | Enable Python bindings | OFF |
| `NLM_ENABLE_SANITIZERS` | Enable memory sanitizers | ON |
| `CCACHE_DIR` | CCache directory | system default |

#### Configuration Files

- `configs/default.cfg` - Default NLM configuration
- `pyproject.toml` - Python project configuration

### 7. Code Organization

#### Core Components

- **Core**: Fundamental neural computation (types, config, random, logging)
- **Brain**: Neural dynamics and computation
- **Agent**: Interface between brain and world
- **World**: Simulation environment
- **Experiments**: Experimental features and demonstrations

#### File Organization

All source files are organized by functional area:

```
src/
├── core/        # Core neural types and utilities
│   ├── Config/  # Configuration management
│   ├── Logger/  # Logging system
│   ├── Random/  # Random number generation
│   ├── SimulationClock/  # Timing system
│   └── Types/   # Neural type definitions
├── brain/       # Neural computation
│   ├── Brain.cpp/hpp              # Main brain class
│   ├── NeuralPopulation.cpp/hpp  # Neural populations
│   ├── NeuralRegion.cpp/hpp      # Brain regions
│   ├── Neuron.cpp/hpp            # Individual neurons
│   └── Synapse.cpp/hpp           # Synaptic connections
└── ...         # Other modules
```

### 8. Best Practices

#### Coding Standards

- **C++20**: All code uses C++20 features
- **Modern CMake**: Target-based approach
- **Type Safety**: Strong typing with compile-time checks
- **Memory Safety**: Reference counting and smart pointers
- **Performance**: Optimized algorithms and minimal overhead

#### Build Practices

```cmake
# Use modern CMake target-based approach
target_link_libraries(project_name NLM::nlm_core)

# Set proper C++ standard
target_compile_features(project_name PRIVATE cxx_std_20)

# Use target-specific include directories
target_include_directories(project_name PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

### 9. Troubleshooting

#### Common Build Issues

**Issue**: CMake version too old
```bash
# Update CMake
sudo apt-get install cmake  # Ubuntu/Debian
sudo yum install cmake     # CentOS/RHEL
brew install cmake         # macOS
```

**Issue**: Missing dependencies
```bash
# Install dependencies
sudo apt-get install build-essential libpython3-dev
brew install python3
```

**Issue**: Python bindings not working
```bash
# Ensure pybind11 is available
pip install pybind11
```

**Issue**: Installation permissions
```bash
sudo cmake --install . --prefix /usr/local
```

### 10. Continuous Integration

#### GitHub Actions

The project includes CI configurations:

```yaml
name: NLM CI
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
    
    steps:
    - uses: actions/checkout@v3
    - name: Set up CMake
      uses: actions/setup-python@v4
      with:
        python-version: '3.9'
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DNLM_BUILD_TESTS=ON
    
    - name: Build
      run: cmake --build .
    
    - name: Test
      run: ctest --output-on-failure
```

#### Local Testing

```bash
# Full test suite
./build.sh test

# Build and test with Python
./build.sh build --python
./build.sh test
```

### 11. Performance Considerations

#### Optimization Options

```bash
# Release build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Debug build with sanitizers
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON

# Use CCache for faster rebuilds
cmake .. -DNLM_ENABLE_CCACHE=ON
```

#### Profiling

```bash
# Enable profiling
cmake .. -DNLM_ENABLE_PROFILING=ON

# Run with profiling tools
valgrind --tool=callgrind ./nlm --benchmark
```

### 12. Documentation

#### API Documentation

```bash
# Generate API documentation
mkdir build
dep build
cmake .. -DNLM_ENABLE_DOCS=ON
cmake --build . --target docs
```

#### Code Comments

All source files include:
- Class and function documentation
- Parameter descriptions
- Return value explanations
- Example usage where appropriate

## Conclusion

This workflow document provides a comprehensive guide for developing and contributing to the NLM project. The modern CMake build system ensures:

- **Maintainability**: Clean, modular target-based architecture
- **Flexibility**: Extensive configuration options
- **Portability**: Cross-platform support
- **Performance**: Optimized builds with optional sanitizers
- **Testing**: Comprehensive test suite
- **Packaging**: Full CPack integration

The NLM project is ready for production use with modern development tools and practices.
