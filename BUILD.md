# NLM Build Instructions

# Quick Build and Installation Guide

This document provides comprehensive instructions for building, installing, and using NLM (Neural Learning Machine) in both C++ and Python.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Building from Source](#building-from-source)
3. [Installing Python Package](#installing-python-package)
4. [Quick Start](#quick-start)
5. [Troubleshooting](#troubleshooting)
6. [Common Issues](#common-issues)

---

## Prerequisites

### Required Software

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Git**: For cloning the repository

### Optional Dependencies

- **Python Packages**: `pybind11`, `scikit-build-core`, `numpy`, `pytest` (for testing)
- **Linux**: `cmake`, `build-essential`, `g++`
- **macOS**: `xcode-select --install` (for command line tools)
- **Windows**: Visual Studio 2019+ with C++ workload

---

## Building from Source

### 1. Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git

cd nlm
```

### 2. Build Configuration

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake (choose one)

# Option A: Release build (recommended for production)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Option B: Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Option C: Custom installation path
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local

# Option D: Build with Python bindings enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)
```

### 3. Building

```bash
# Build all targets
make -j$(nproc)

# Or use CMake's build tool
cmake --build . --config Release

# For multi-config generators (like Visual Studio)
cmake --build . --config Debug
```

### 4. Running Tests

```bash
# Run the test suite (if available)
ctest --output-on-failure

# Or run specific tests
./nlm_test
```

### 5. Installation

```bash
# Install system-wide (requires sudo)
sudo make install

# Or install to specific prefix
make install DESTDIR=/tmp/nlm-install
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

---

## Installing Python Package

### Method 1: Using pip (Recommended)

```bash
# Install build dependencies first
pip install pybind11 scikit-build-core numpy pytest

# Install NLM Python package
pip install .

# Or install in development mode (for editing)
pip install -e .
```

### Method 2: Using pyproject.toml

```bash
# Install with scikit-build-core (used automatically)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/nlm-*.whl
```

### Method 3: Manual Build

```bash
cd /path/to/nlm

# Create Python-specific build directory
mkdir -p python_build
cd python_build

# Configure for Python bindings
cmake .. -DPYTHON_EXECUTABLE=$(which python3) -DPYTHON_MODULE_NAME=pynlm

# Build Python module
make pynlm
```

---

## Quick Start

### 1. Verify Installation

```bash
# Check if Python module is available
python3 -c "import pynlm; print('NLM Python bindings loaded successfully!'); print(f'Version: {pynlm.__version__}')"
```

### 2. Create Your First Brain

```python
import pynlm

# Method 1: Use convenience functions (EASIEST!)
brain = pynlm.create_default_brain()

# Method 2: Traditional way
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### 3. Run a Simple Simulation

```python
import pynlm

# Create brain
brain = pynlm.create_default_brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### 4. Complete Example with Learning Agent

```python
import pynlm

# Setup
config = pynlm.create_default_config_with_params(
    neuron_count=2000,
    region_count=3
)
brain = pynlm.create_default_brain(config)

world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning simulation
for step in range(1000):
    world.update(timestep=0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

---

## Troubleshooting

### Build Issues

#### CMake cannot find Python

```bash
# Install Python development headers
pip install pybind11 scikit-build-core

# Re-run configuration
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

#### Missing Dependencies

```bash
# Ubuntu/Debian
 sudo apt-get update
sudo apt-get install build-essential cmake g++

# macOS
xcode-select --install

# Windows
# Install Visual Studio with C++ workload
```

#### Compilation Errors

**Common fixes:**

1. **C++20 not supported**
   ```bash
   # Use newer compiler
   cmake .. -DCMAKE_CXX_COMPILER=g++-10
   ```

2. **CMake version too old**
   ```bash
   # Update CMake
   sudo apt-get install cmake
   ```

3. **Memory issues during build**
   ```bash
   # Reduce build parallelism
   make -j4
   
   # Or increase memory
   swap --add /tmp/swapfile 2G
   ```

### Python Issues

#### ImportError: No module named 'pynlm'

```bash
# Reinstall Python package
pip install --force-reinstall .

# Or install from scratch
pip uninstall pynlm
pip install .
```

#### Version Mismatch

```bash
# Check Python and C++ versions
python3 -c "import sys; print(f'Python: {sys.version}')"

# Ensure compatibility
# Python 3.8+ with C++20 compiler
```

---

## Common Issues and Solutions

### 1. "Brain not firing any neurons"

**Problem**: Brain isn't producing any activity.

**Solution**:
```python
# Increase input or enable learning
brain = pynlm.create_default_brain()
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
brain.step(0)
```

### 2. Memory usage too high

**Problem**: Large brains consuming excessive memory.

**Solution**:
```python
# Use smaller configuration
config = pynlm.create_default_config_with_params(
    neuron_count=500,      # Reduced from default 1000
    region_count=1         # Reduced from default 1
)
```

### 3. Simulation too slow

**Problem**: Simulation running slowly.

**Solution**:
```python
# Optimize configuration
config = pynlm.create_default_config_with_params(
    simulation_timestep=0.01,  # Larger time step
    neuron_count=1000
)
```

### 4. Checkpoints not saving

**Problem**: Brain state not being saved to file.

**Solution**:
```python
# Check file permissions and directory
import os
if not os.access('./', os.W_OK):
    os.makedirs('./checkpoints', exist_ok=True)
    
brain.save('./checkpoints/brain.checkpoint')
```

---

## Additional Resources

### Documentation

- **ARCHITECTURE.md**: Detailed system architecture
- **SCIENCE.md**: Scientific background and models
- **EXPERIMENTS.md**: Experiment descriptions and results
- **README.md**: Project overview and usage

### Examples

- **Phase 6 Demo**: `build/nlm_phase6_demo` - Complete integration test
- **Phase 4 Demo**: `build/nlm_phase4_demo` - Cognitive functions demo
- **Phase 3 Demo**: `build/nlm_phase3_demo` - World interaction demo

### Testing

- **Unit Tests**: `build/nlm_test` - Comprehensive test suite
- **Python Tests**: `python/test*.py` - Python binding tests
- **Integration Tests**: Phase demos show system integration

---

## Development Tips

### For Python Developers

1. **Use convenience functions for rapid prototyping**
2. **Save checkpoints frequently for long experiments**
3. **Monitor statistics with `visualize_brain_state()`**
4. **Use smaller configs for testing**

### For C++ Developers

1. **Follow the modular structure in `src/`**
2. **Use `CMakeLists.txt` for build configuration**
3. **Test individual components before integration**
4. **Profile with `gprof` or `valgrind`**

### Performance Optimization

1. **Use `simulation_timestep=0.01` for faster simulation**
2. **Limit `neuron_count` for resource-constrained environments**
3. **Use `region_count=1` for simple tests**
4. **Enable `checkpointing` for long-running simulations**

---

## Getting Help

### Online Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: Report bugs and feature requests

### Local Resources

```bash
# View documentation locally
pdflatex docs/ARCHITECTURE.md

# View Python help
python3 -c "import pynlm; help(pynlm.create_default_brain)"
```

---

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

---

*Last updated: $(date +%Y-%m-%d)*
