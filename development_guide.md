# Development Guide

This document provides comprehensive instructions for developing with NLM, including setup, testing, and contributing.

## Quick Start

### Prerequisites

- CMake 3.16 or higher
- Python 3.8+
- pybind11
- C++ compiler with C++20 support

### Building

#### Standard Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build all targets
make -j$(nproc)
```

#### Development Build

```bash
# For faster iteration with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

#### Python Module Build Only

```bash
# In python directory (for pynlm bindings)
cd python
cmake ..
make -j$(nproc)
```

### Testing

#### Run All Tests

```bash
# In build directory
cd build
ctest --output-on-failure
```

#### Run Specific Test Categories

```bash
# Run brain tests
./test_brain

# Run neuron tests
./test_neuron

# Run synapse tests
./test_synapse
```

### Python Development

#### Install Python Module

```bash
# From python directory
cd python
pip install -e .  # For development install
```

#### Run Python Examples

```bash
# Basic examples are in examples/ directory
cd examples
python3 basic_usage.py
```

#### Testing Python Bindings

```bash
# Run Python tests (if available)
python3 -m pytest tests/ -v
```

## Development Workflow

### 1. Setting Up Development Environment

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create development build
mkdir dev-build
cd dev-build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic" ..
```

### 2. Code Structure

```
/src/
├── core/           # Core utilities and infrastructure
├── brain/          # Neural computation core
├── sensory/        # Sensory processing
├── motor/          # Motor control
├── world/          # Environment simulation
├── memory/         # Memory systems
├── prediction/     # Prediction mechanisms
├── cognition/      # Cognitive processing
├── neuromodulation/ # Neuromodulatory systems
├── plasticity/     # Plasticity rules
├── development/    # Development processes
├── experiments/    # Experimental frameworks
└── visualization/  # Visualization tools

/python/
├── bindings.cpp     # Python bindings implementation
├── CMakeLists.txt   # Python module build

/tests/
├── test_*.cpp       # C++ unit tests
└── integration/     # Integration tests

/examples/           # Python usage examples

/configs/           # Default configuration files

/docs/              # Documentation
```

### 3. Code Quality Guidelines

#### C++ Coding Standards

- **Style**: Use consistent indentation (2 spaces per level)
- **Naming**: Use camelCase for variables, PascalCase for classes
- **Comments**: Document public API with Doxygen-style comments
- **Error Handling**: Use exceptions for exceptional conditions
- **Memory Management**: Prefer smart pointers (`std::shared_ptr`, `std::unique_ptr`)

#### Python API Guidelines

- **API Design**: Provide intuitive, Pythonic interfaces
- **Documentation**: Include docstrings for all public functions
- **Type Hints**: Add type annotations where helpful
- **Error Handling**: Raise appropriate Python exceptions
- **Consistency**: Follow NumPy-style conventions where applicable

### 4. Adding New Features

#### Adding a New Neural Component

1. **Create header file** in `/src/` directory
2. **Implement in `.cpp` file**
3. **Register with Brain class** if needed
4. **Add Python bindings** if exposing to Python
5. **Write unit tests**
6. **Update documentation**

#### Example: Adding a New Sensor Type

```bash
# 1. Create sensor class
cp -r src/sensory/SensoryInput src/sensory/MySensor
mv src/sensory/MySensor/{header,source}.cpp src/sensory/MySensor.cpp
mv src/sensory/MySensor/{header,source}.hpp src/sensory/MySensor.hpp

# 2. Update includes
# In bindings.cpp, add #include "sensory/MySensor.hpp"

# 3. Add Python bindings
# In bindings.cpp, add py::class_<MySensor, SensoryInput>

# 4. Write tests
cp tests/test_sensory.cpp tests/test_mysensor.cpp
```

### 5. Performance Optimization

#### Profiling

```bash
# Compile with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Run with performance profiling
perf record ./nlm_phase6_demo
perf report
```

#### Memory Leak Detection

```bash
# Compile with sanitizers
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined" ..

# Run with leak detection
valgrind ./nlm_phase6_demo
```

#### SIMD Optimization (if available)

```cpp
// In performance-optimized files
#ifdef __SSE2__
// Use SIMD instructions
#else
// Fallback to scalar operations
#endif
```

### 6. Contributing

#### Pull Request Process

1. **Fork the repository**
2. **Create feature branch**
3. **Implement changes**
4. **Write tests**
5. **Update documentation**
6. **Run full test suite**
7. **Create pull request**

#### Commit Message Guidelines

```
feat: add new neural population type
fix: resolve memory leak in spike system
docs: update API documentation
perf: optimize synaptic transmission
refactor: restructure plasticity rules
```

### 7. Advanced Development

#### Extending with Custom Brains

```cpp
// Example: Custom brain implementation
class CustomBrain : public Brain {
public:
    CustomBrain(std::shared_ptr<Config> config) : Brain(config) {}
    
    void customLearningAlgorithm() {
        // Implement custom learning
        // Can access all neural components via Brain:: methods
    }
};
```

#### Python Extensions

```python
# Example: Extending Python API with custom functions
import pynlm

def create_enhanced_brain(config, learning_rate=0.01):
    brain = pynlm.createBrain(config)
    # Custom initialization
    return brain

# Register new function
pynlm.registerFunction("createEnhancedBrain", create_enhanced_brain)
```

### 8. Best Practices

#### C++ Best Practices

- **Don't Repeat Yourself (DRY)**: Extract common code into templates or functions
- **Use RAII**: Ensure proper resource cleanup
- **Thread Safety**: When applicable, use proper synchronization
- **Exception Safety**: Guarantee proper state on exceptions
- **Const Correctness**: Mark methods as const when they don't modify state

#### Python Best Practices

- **PEP 8 Compliance**: Follow Python style guide
- **Type Hints**: Use for better IDE support and documentation
- **Context Managers**: For resource management
- **Unicode Support**: Proper string handling
- **Testing**: Use pytest for unit tests

### 9. Troubleshooting

#### Common Build Issues

**Issue**: CMake cannot find pybind11
**Solution**:
```bash
cmake -D pybind11_DIR=/path/to/pybind11/share/cmake/pybind11 ..
```

**Issue**: Build fails due to missing Python headers
**Solution**:
```bash
# On Ubuntu:
sudo apt-get install python3-dev python3-pip

# On macOS:
brew install python
```

#### Runtime Issues

**Issue**: Python module not found
**Solution**:
```bash
python3 -c "import pynlm; print(pynlm.__version__)"
```

**Issue**: Segmentation fault
**Solution**:
```bash
# Debug build with sanitizers
gdb ./nlm_phase6_demo
```

### 10. Further Reading

- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Guide](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)

## License

MIT License - See LICENSE file for details.
