# Enhanced Python Bindings for NLM - Build Instructions

## Overview

This document provides comprehensive build instructions for the enhanced Python bindings of the NLM (Neural Learning Machine) framework.

## Requirements

### Build System
- CMake 3.16 or higher
- C++ compiler with C++20 support (GCC 9+, Clang 10+, MSVC 2019+)
- Python 3.8 or higher with development headers
- pybind11 2.11.0 or higher
- scikit-build-core >= 0.5.0

### Dependencies
- Python bindings: `pip install pybind11 scikit-build-core pytest numpy`

## Building the Enhanced Bindings

### Method 1: Using the Python Build System (Recommended)

```bash
# Clone the repository
cd /path/to/nlm

# Navigate to the python directory
cd python

# Create build directory for enhanced bindings
mkdir build_enhanced
cd build_enhanced

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_BUILD_PYTHON_BINDINGS=ON \
         -DBUILD_ENHANCED=ON

# Build with parallel jobs
make -j$(nproc)

# Install the enhanced module to Python site-packages
make install
```

### Method 2: Building Both Original and Enhanced Modules

```bash
cd /path/to/nlm

# Create main build directory
mkdir build
cd build

# Configure with CMake for both modules
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_BUILD_PYTHON_BINDINGS=ON \
         -DBUILD_ENHANCED=ON

# Build all targets
make -j$(nproc)
```

### Method 3: Using pip (if available)

```bash
cd /path/to/nlm/python
pip install -e .
```

## Build Options

### CMake Options

- `-DNLM_BUILD_PYTHON_BINDINGS=ON/OFF` (default: ON)
  Enable/disable Python bindings

- `-DBUILD_ENHANCED=ON/OFF` (default: ON)
  Build enhanced bindings in addition to original

- `-DCMAKE_BUILD_TYPE=Debug/Release/RelWithDebInfo` (default: Release)
  Build type for optimization settings

- `-DPYTHON_EXECUTABLE=/path/to/python` (auto-detected)
  Path to Python interpreter

### Configuration Options

The enhanced bindings can be configured via CMake variables:

```bash
cmake .. \
         -DNLM_BUILD_PYTHON_BINDINGS=ON \
         -DBUILD_ENHANCED=ON \
         -DNLM_ENABLE_TRAINING=ON \
         -DNLM_ENABLE_VISUALIZATION=ON \
         -DNLM_ENABLE_EXPORT=ON \
         -DNLM_ENABLE_MEMORY_OPTIMIZATION=ON
```

## Building with Different Compilers

### GCC/Clang
```bash
# For GCC
cd build_enhanced
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CXX_STANDARD=20
make -j$(nproc)

# For Clang
cd build_enhanced
cmake .. -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_STANDARD=20
make -j$(nproc)
```

### MSVC
```bash
# On Windows
mkdir build_enhanced
cd build_enhanced
cmake .. -G "Visual Studio 16 2019" -A x64

# Build with MSBuild
cd build_enhanced
cmake --build . --config Release --parallel
```

## Installation

### System-wide Installation

```bash
cd build_enhanced
cmake --install . --prefix /usr/local
```

### Python Installation

```bash
cd build_enhanced
cmake --install . --prefix "$(python -c 'import site; print(site.getsitepackages()[0])')"
```

### Virtual Environment

```bash
# Create virtual environment
python -m venv venv_source
cd venv_source
source bin/activate

# Install enhanced bindings
cd /path/to/nlm/python/build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --install . --prefix "$(pwd)/venv_source/lib/python3.x/site-packages"
```

## Testing the Enhanced Bindings

### Running Tests

```bash
cd /path/to/nlm/python

# Run the test executable (if built)
./test_bindings_enhanced

# Or run Python tests (if available)
python -m pytest test_*.py
```

### Test Building

The enhanced bindings include several test files:

1. **`test_bindings_enhanced.cpp`**: Unit tests for C++ bindings
2. **`example_enhanced.py`**: Python examples demonstrating usage
3. **`ENHANCED_BINDINGS.md`**: Comprehensive documentation
4. **`README.md`**: Original project documentation

### Building Tests

```bash
cd /path/to/nlm/python/build_enhanced

# Build with tests
mkdir build_tests
cd build_tests
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON -DBUILD_TESTS=ON
make -j$(nproc)
```

## Verification

### Verify Installation

```bash
# Test Python import
python -c "import pynlm_enhanced; print(pynlm_enhanced.get_version())"

# Test basic functionality
python -c "
import pynlm_enhanced as nlm
config = nlm.create_default_config()
brain = nlm.create_brain(config)
print('Basic functionality test passed')
"
```

### Verify Features

```bash
python -c "
import pynlm_enhanced as nlm

# Test configuration
config = nlm.create_default_config()
print('Configuration test passed')

# Test brain creation
brain = nlm.create_brain(config)
print('Brain creation test passed')

# Test world creation
world = nlm.create_simple_world()
print('World creation test passed')

# Test context manager
with nlm.create_brain_session(brain, None, world) as session:
    print('Context manager test passed')

print('All feature tests passed')
"
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: CMake cannot find pybind11**

```bash
# Install pybind11 first
pip install pybind11

# Or find pybind11 path
find /usr -name "pybind11Config.cmake" 2>/dev/null

# Then set with:
cmake .. -DPYTHON_EXECUTABLE=/path/to/python \
         -Dpybind11_DIR=/path/to/pybind11/cmake
```

**Issue 2: Python import error**

```bash
# Check Python path
python -c "import sys; print(sys.path)"

# Check if module is built
find . -name "*.so" -o -name "*.pyd"

# Rebuild if necessary
make clean && make
```

**Issue 3: Compilation errors**

```bash
# Check compiler version
g++ --version

# Ensure C++20 support
mkdir build_enhanced
cd build_enhanced
cmake .. -DCMAKE_CXX_STANDARD=20
make
```

**Issue 4: Memory errors**

```bash
# Build with optimization flags
cd build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Or use less parallel jobs
make -j2
```

### Debug Information

To gather debugging information:

```bash
# Enable verbose output
cd build_enhanced
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
make VERBOSE=1

# Check build logs
make clean
make > build.log 2>&1
```

## Performance Optimization

### Compilation Optimizations

```bash
# Optimize for size
cd build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel

# Optimize for speed
cd build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_OPTIMIZATION_LEVEL=3

# Profile builds
cd build_enhanced
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_PROFILE=ON
```

### Runtime Optimizations

```python
# Use context managers for automatic cleanup
with nlm.create_brain_session(brain, agent, world) as session:
    # Code here
    pass  # Automatically cleaned up

# Monitor memory usage
stats = nlm.memory.get_memory_stats()
print(f"Memory usage: {stats}")

# Optimize memory periodically
nlm.memory.optimize_memory()
```

## Advanced Build Configuration

### Custom Build Scripts

Create a custom build script in the python directory:

```bash
# create build_enhanced.py
cat > /path/to/nlm/python/build_enhanced.py << 'EOF'
#!/usr/bin/env python3
import subprocess
import sys
import os

def run_command(cmd):
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        return False
    return True

def main():
    # Set up environment
    os.environ['CXXFLAGS'] = '-O3 -march=native'
    
    # Configure
    if not run_command('mkdir -p build_enhanced && cd build_enhanced && cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON -DBUILD_ENHANCED=ON'):
        sys.exit(1)
    
    # Build
    if not run_command('cd build_enhanced && make -j$(nproc)'):
        sys.exit(1)
    
    # Install
    if not run_command('cd build_enhanced && cmake --install . --prefix $(python -c "import site; print(site.getsitepackages()[0])")'):
        sys.exit(1)
    
    print("Build completed successfully!")

if __name__ == "__main__":
    main()
EOF

python build_enhanced.py
```

### Docker Build

```dockerfile
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3 \
    python3-pip \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
RUN pip3 install pybind11 scikit-build-core numpy

# Copy source code
WORKDIR /app
COPY . /app

# Build enhanced bindings
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON -DBUILD_ENHANCED=ON && \
    make -j$(nproc)

# Install Python module
RUN cd build && \
    cmake --install . --prefix "$(python3 -c 'import site; print(site.getsitepackages()[0])')"

CMD ["python3", "-c", "import pynlm_enhanced; print('Enhanced NLM bindings loaded successfully')"]
```

## Cleanup

### Remove Build Artifacts

```bash
# Remove build directories
cd /path/to/nlm
rm -rf build build_enhanced build_tests

# Remove Python cache files
find . -name "__pycache__" -type d -exec rm -rf {} +
find . -name "*.pyc" -delete
find . -name ".pytest_cache" -type d -exec rm -rf {} +

# Remove documentation build files
rm -rf docs/_build
rm -rf python/generated_docs
```

### Uninstall

```bash
# Uninstall Python package
pip uninstall pynlm_enhanced

# Remove system installation
rm -f /usr/local/lib/python3.x/site-packages/pynlm_enhanced*.so
rm -f /usr/local/lib/python3.x/site-packages/pynlm_enhanced*.pyd
```

## Support

### Getting Help

1. **Documentation**: https://nlm.readthedocs.io
2. **GitHub Issues**: https://github.com/nlm-project/nlm/issues
3. **Community**: Check the project's documentation for contact information

### Reporting Bugs

When reporting bugs, please include:

1. **Build log**: Full output of the build process
2. **Platform information**: OS, compiler, Python version
3. **Reproduction steps**: Minimal example that reproduces the issue
4. **Expected vs actual behavior**: What should happen vs what actually happens
5. **Code examples**: Relevant code snippets

### Contributing

1. **Fork the repository**: Create your own copy
2. **Create a branch**: Work on a specific feature or bug fix
3. **Follow coding standards**: Use the existing code style and conventions
4. **Add tests**: Include tests for new functionality
5. **Update documentation**: Document new features and APIs
6. **Submit pull request**: Submit your changes for review

## Acknowledgments

This enhanced bindings framework was developed to provide:

1. **Improved usability**: Pythonic API with comprehensive documentation
2. **Advanced features**: Training utilities, visualization, export capabilities
3. **Robust error handling**: Custom exceptions and validation
4. **Memory management**: Context managers and optimization utilities
5. **Performance**: Optimized for both compilation and runtime

The enhanced bindings build upon the original NLM framework and provide significant improvements for Python users.
