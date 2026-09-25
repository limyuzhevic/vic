# NLM Installation Guide

## Overview

This guide provides comprehensive instructions for installing and setting up the Neural Learning Machine (NLM) for development and research purposes.

## System Requirements

### Hardware Requirements
- **CPU**: Multi-core processor (8+ cores recommended for simulation work)
- **RAM**: 16 GB minimum (32 GB+ recommended for large-scale simulations)
- **Storage**: At least 100 GB free space for code and simulation data
- **GPU**: Not required but beneficial for acceleration

### Software Requirements
- **Compiler**: GCC 11+ or Clang 14+
- **CMake**: 3.16 or higher
- **Python**: 3.8 or higher
- **Build Tools**: Make, Ninja

## Quick Installation

### Option 1: Using Package Manager (Linux/macOS)

```bash
# Clone the repository
.git

# Navigate to the project directory
cd nlm

# Configure build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)
```

### Option 2: Using Environment Manager (Recommended)

```bash
# For development environments using conda or miniconda
conda create -n nlm python=3.9
cs nlm

# Install dependencies
conda install -c conda-forge scikit-build-core pybind11

# Clone and build from source
cd /path/to
clone nlm-project/nlm.git
cd nlm
cmake . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Installation Steps

### Step 1: System Dependencies

Install required system packages:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install -y gcc g++ make cmake git pkg-config libomp-dev
```

**macOS:**
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git pkg-config
```

**Windows:**
```bash
# Install Visual Studio Build Tools with C++ support
# Install Chocolatey package manager
# choco install cmake git
```

### Step 2: Download NLM Source Code

```bash
# Using git
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Using wget (alternative)
wget https://github.com/nlm-project/nlm/archive/refs/heads/main.tar.gz
tar -xzf main.tar.gz
cd nlm-main
```

### Step 3: Configure Build

```bash
# Create build directory and configure CMake
mkdir build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DPYTHON_EXECUTABLE=/path/to/python3 \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DBUILD_TESTS=ON
```

### Step 4: Build NLM

```bash
# Build with parallel jobs for faster compilation
make -j$(nproc)

# Or use CMake's build tool
cmake --build . --config Release --parallel
```

## Python Installation

### Automated Python Package Installation

```bash
# The NLM Python package (pynlm) includes enhanced bindings
cd /path/to/nlm

# Using pip (for wheel files)
pip install .

# Or building from source
pip install --no-build-isolation .
```

### Manual Python Setup

```python
import sys

# Add the Python module path
sys.path.insert(0, '/path/to/nlm/python')

# Import NLM modules
try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
```

## Verification and Testing

### Step 5: Verify Installation

```bash
# Navigate to build directory
cd build

# Run basic tests
ctest --output-on-failure

# Or run the Phase 6 demo
./nlm_phase6_demo
```

### Step 6: Test Python Bindings

```python
#!/usr/bin/env python3
"""
Test NLM Python installation
"""
import sys
import pynlm

def test_basic_functionality():
    print("Testing NLM Python installation...")
    
    # Test 1: Create configuration
    config = pynlm.createDefaultConfig()
    print("✓ Configuration created")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    # Test 3: Initialize brain
    brain.initialize()
    print("✓ Brain initialized")
    
    # Test 4: Run simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
    
    # Test 5: Test world interaction
    world = pynlm.createWorld(brain)
    print("✓ World created")
    
    print("\nAll tests passed! NLM installation is successful.")

if __name__ == "__main__":
    test_basic_functionality()
```

### Step 7: Run Comprehensive Tests

```bash
# Run all NLM tests
cd build
ctest -T Test

# Or run specific test suites
cd build
./test_nlm_suite
```

## Advanced Installation Options

### 1. Build with Performance Optimizations

```bash
cd /path/to/nlm
mkdir build_perf
cd build_perf
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native" \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE=$(which python3)

make -j$(nproc)
```

### 2. Build with Debug Support

```bash
cd /path/to/nlm
mkdir build_debug
cd build_debug
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-g -O0 -DDEBUG" \
    -DBUILD_PYTHON_BINDINGS=ON

make -j$(nproc)
```

### 3. Install Development Version

```bash
cd /path/to/nlm
pip install -e .
```

## Configuration Management

### Environment Variables

```bash
# Set NLM configuration environment variables
export NLM_DATA_PATH="/path/to/nlm/data"
export NLM_LOG_LEVEL="info"
export NLM_ENABLE_PERF_STATS="true"

# Set Python paths
source /path/to/nlm/python/setup_environment.sh
```

### Configuration Files

NLM uses multiple configuration files:

1. **Default Configuration** (`configs/default.json`)
2. **Performance Configuration** (`configs/performance.json`)
3. **Development Configuration** (`configs/development.json`)

To use a specific configuration:

```python
import json
import pynlm

# Load configuration from file
with open('configs/my_config.json', 'r') as f:
    config_dict = json.load(f)

config = pynlm.createConfig(config_dict)
brain = pynlm.createBrain(config)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: CMake Configuration Errors

**Problem**: CMake cannot find required dependencies.

**Solution**:

```bash
# Reconfigure CMake with full paths
cmake .. \
    -DCMAKE_PREFIX_PATH=/usr/local \
    -DCMAKE_LIBRARY_PATH=/usr/local/lib
```

#### Issue 2: Python Import Errors

**Problem**: Cannot import pynlm module.

**Solution**:

```bash
# Check Python path
python3 -c "import sys; print(sys.path)"

# Install Python module
pip install -e .
```

#### Issue 3: Compilation Errors

**Problem**: Build fails due to missing headers or libraries.

**Solution**:

```bash
# Install missing dependencies
# Ubuntu: sudo apt install build-essential libomp-dev
# macOS: brew install libomp
```

#### Issue 4: Memory Issues During Build

**Problem**: Build process runs out of memory.

**Solution**:

```bash
# Use more memory and fewer parallel jobs
make -j4
# Or use smaller build targets
```

#### Issue 5: Runtime Errors

**Problem**: NLM runs but produces unexpected results.

**Solution**:

```bash
# Run with debugging
./nlm_demo --debug

# Check log files
cat logs/nlm.log
```

### Getting Help

#### Stack Traces

```bash
# Run with core dumps for debugging
gdb ./nlm_demo
coredump-filter full
```

#### Performance Profiling

```bash
# Use performance profilers
valgrind ./nlm_demo
perf record ./nlm_demo
```

#### Community Support

For additional help:

1. **GitHub Issues**: Report bugs or request features
2. **Documentation**: Check our comprehensive documentation
3. **Forums**: Join the NLM community forums
4. **Email**: Contact support@nlm-project.org

## Best Practices

### 1. Version Control

```bash
# Initialize git repository if not already done
git init
git add .
git commit -m "Initial NLM installation"
```

### 2. Environment Management

Use conda or virtualenv for Python development:

```bash
conda create -n nlm-dev python=3.9
conda activate nlm-dev
pip install -e .
```

### 3. Build Optimization

For faster builds:

```bash
# Use fewer parallel jobs to avoid memory issues
make -j4

# Clean build before rebuilding
cmake --build . --target clean
```

### 4. Testing Strategy

```bash
# Run unit tests regularly
ctest --output-on-failure

# Run integration tests
./integration_tests.sh

# Run performance benchmarks
./performance_tests.sh
```

## Uninstallation

### Remove NLM from System

```bash
# Remove Python package
pip uninstall pynlm

# Remove build artifacts (optional)
rm -rf build
rm -rf *.log

# Remove environment
conda env remove -n nlm
```

## Conclusion

This installation guide provides comprehensive instructions for setting up NLM across different platforms and use cases. Following these steps will ensure a successful installation and prepare you for effective development and experimentation with the Neural Learning Machine.

For the most up-to-date installation instructions and troubleshooting tips, always refer to the official NLM documentation.
