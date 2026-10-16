# NLM Build and Installation Guide

## Overview

This document provides comprehensive instructions for building, installing, and running the NLM (Neural Learning Machine) project. It covers all platforms, build configurations, and usage scenarios for both C++ core and Python bindings.

## Prerequisites

### System Requirements:
- **Operating System**: Linux, macOS, or Windows (with appropriate toolchain)
- **Compiler**: C++17/20 compatible compiler (g++ 9+, clang 12+, MSVC 19.2+)
- **CMake**: Version 3.16 or higher
- **Python**: Version 3.8 or higher
- **Memory**: 2 GB RAM minimum (8 GB+ recommended for full demonstrations)
- **Disk Space**: 500 MB free space for build artifacts

### Optional Dependencies:
- **PyTorch/TensorFlow**: For advanced visualization and analysis
- **Jupyter Notebook**: For interactive demonstrations
- **Doxygen**: For documentation generation
- **Valgrind**: For memory leak detection (Linux)

## Build Methods

### Method 1: Standard CMake Build (Recommended)

#### Quick Start:
```bash
# Create build directory
mkdir -p build
cd build

# Configure with defaults
-DCMAKE_BUILD_TYPE=Release
-DBUILD_TESTS=ON
-DBUILD_PYTHON_BINDINGS=ON
-DBUILD_BENCHMARKS=ON

# Build all targets
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -- -j$(nproc)
```

#### Detailed Build Steps:

**1. Configure the build:**
```bash
mkdir build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DBUILD_BENCHMARKS=ON \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DCMAKE_INSTALL_PREFIX=/usr/local
```

**2. Build the project:**
```bash
# Build all targets
cmake --build . -- -j$(nproc)

# Or use make (Unix-like systems)
make -j$(nproc)

# Build specific targets
make nlm                    # Main demo
make nlm_phase6_demo         # Phase 6 integration demo
make nlm_test               # Unit tests
make nlm_benchmark          # Performance benchmark
```

**3. Install (optional):**
```bash
# Install to system (requires sudo)
sudo cmake --install .

# Or install locally
cmake --install . --prefix ~/.local
```

### Method 2: Python Package Installation

#### Install via pip (Recommended for Python users):
```bash
# Install NLM with Python bindings
cd /path/to/nlm
pip install -e .
```

#### Verify installation:
```bash
python3 -c "import pynlm; print(pynlm.__version__)"
```

#### Build Python bindings separately:
```bash
# Build only Python bindings
cd /path/to/nlm
pip install scikit-build-core pybind11
python -m pip install --no-build-isolation -e .
```

### Method 3: Docker Build (for containerized environments)

#### Build Docker image:
```bash
cd /path/to/nlm
docker build -t nlm:latest .
```

#### Run in Docker:
```bash
docker run --rm -v $(pwd)/checkpoints:/workspace/app/checkpoints nlm:latest /nlm_phase6_demo
```

## Build Configuration Options

### CMake Options:
```bash
cmake .. \
    -DBUILD_TESTS=ON/OFF \
    -DBUILD_PYTHON_BINDINGS=ON/OFF \
    -DBUILD_BENCHMARKS=ON/OFF \
    -DBUILD_EXAMPLES=ON/OFF \
    -DCMAKE_BUILD_TYPE=Debug/Release/RelWithDebInfo \
    -DCMAKE_CXX_STANDARD=17/20 \
    -DPYTHON_EXECUTABLE=/path/to/python \
    -DPYTHON_MODULE_NAME=pynlm \
    -DBUILD_SHARED_LIBS=ON/OFF
```

### Environment Variables:
```bash
# Set Python path
export PYTHONPATH=/path/to/nlm/python:$PYTHONPATH

# Set NLM configuration
export NLM_CONFIG_FILE=/path/to/configs/default.cfg

# Performance settings
export NLM_NUM_THREADS=4
```

## Post-Build Installation

### System Installation:
```bash
# Install to /usr/local (requires sudo)
sudo cmake --install build --prefix /usr/local

# Verify installation
which nlm_phase6_demo
nlm_phase6_demo --help
```

### Local Installation:
```bash
# Install to ~/.local
cmake --install build --prefix ~/.local

# Add to PATH
export PATH="~/.local/bin:$PATH"
```

### Python Package Installation:
```bash
# Install with pip (recommended)
pip install -e .

# Test installation
python3 -c "import pynlm; print('pynlm imported successfully')"
```

## Running the Software

### 1. Basic Demo (Phase 2)

#### Command Line:
```bash
# Standard demo
./nlm

# With custom configuration
./nlm --config=experiments/config_phase6.json

# With performance tuning
./nlm --neuron_count=2000 --region_count=2
```

#### Python Interface:
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation loop
for step in range(1000):
    brain.step(step)

# Produce action
action = brain.produceAction()
print(f"Action: {action}")
```

### 2. Phase 6 Integration Demo

#### Command Line:
```bash
# Full integration test
./nlm_phase6_demo

# With custom configuration
./nlm_phase6_demo --neuron_count=500 --max_steps=2000
```

#### Python Interface:
```python
from nlm import Phase6IntegratedExperiment, Phase6Config

# Create experiment
experiment = Phase6IntegratedExperiment()

# Configure experiment
config = Phase6Config()
config.neuronCount = 1000
config.maxSteps = 5000
config.enableCheckpointing = True
config.enableReplay = True

# Run experiment
result = experiment.run(config)
print(f"Total reward: {result.totalReward}")
print(f"Integration status: {result.memoryWorkingMemoryIntegrated}")
```

### 3. Unit Tests

#### Running Tests:
```bash
# Build and run tests
mkdir build_test
cd build_test
cmake .. -DCMAKE_BUILD_TYPE=Debug
make nlm_test
./nlm_test
```

#### Individual Test Categories:
```bash
# Run specific test suites (if supported)
./nlm_test --types          # Run type tests
./nlm_test --config         # Run config tests
./nlm_test --brain          # Run brain tests
```

### 4. Performance Benchmark

#### Command Line:
```bash
# Run benchmark
./nlm_benchmark

# With custom settings
./nlm_benchmark --neurons=2000 --steps=1000
```

#### Python Interface:
```python
import pynlm
from time import time

# Create benchmark configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)

# Run benchmark
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time()
for step in range(1000):
    brain.step(step)
end_time = time()

print(f"Benchmark completed in {end_time - start_time:.2f} seconds")
```

## Configuration Files

### Available Configurations:

#### Default Configuration:
- **File**: `configs/default.cfg`
- **Description**: Basic configuration for development
- **Parameters**: 1000 neurons, 1 region, standard plasticity

#### Phase 6 Configuration:
- **File**: `configs/config_phase6.json`
- **Description**: Optimized for final integration demo
- **Parameters**: 1000 neurons, checkpointing enabled

#### Performance Configuration:
- **File**: `configs/config_performance.cfg`
- **Description**: Optimized for maximum performance
- **Parameters**: 2000 neurons, reduced plasticity

#### Development Configuration:
- **File**: `configs/config_development.cfg`
- **Description**: Detailed logging and validation
- **Parameters**: Small network with extensive debugging

### Configuration Override:
```bash
# Override configuration via command line
./nlm_phase6_demo \
    --config=configs/config_phase6.json \
    --neuron_count=500 \
    --max_steps=2000
```

## Python Package Usage

### Installation Verification:
```python
# Check if pynlm is available
try:
    import pynlm
    print("✓ pynlm is available")
    print(f"Version: {pynlm.__version__}")
except ImportError as e:
    print(f"✗ pynlm not available: {e}")
```

### Basic Usage Examples:

#### 1. Create and Initialize Brain:
```python
import pynlm
import time

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 2)

# Create brain
brain = pynlm.createBrain(config)
print("Brain created successfully")

# Initialize brain
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")
```

#### 2. Run Simulation Loop:
```python
import pynlm
import numpy as np

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)

# Simulation loop
for episode in range(10):
    # Reset for new episode
    brain.reset()
    world.reset()
    
    # Set agent at random position
    import random
    world.setAgentStart(random.uniform(10, 90), random.uniform(10, 90))
    
    # Run episode
    for step in range(200):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        brain.receiveSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Get action
        action = brain.produceAction()
        
        # Apply action in world
        world.applyMotorCommand(action, step * 0.001)
    
    # Log episode results
    brain = agent_brain.getBrain()
    print(f"Episode {episode}: Reward={brain.getTotalSpikeCount()}")
```

#### 3. Checkpoint Management:
```python
import pynlm
import os

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step)

# Save checkpoint
checkpoint_file = "test_checkpoint.bin"
if brain.save(checkpoint_file):
    print(f"✓ Checkpoint saved: {checkpoint_file}")
else:
    print("✗ Checkpoint save failed")

# Verify checkpoint file exists
if os.path.exists(checkpoint_file):
    print(f"✓ Checkpoint file exists: {checkpoint_file}")
    file_size = os.path.getsize(checkpoint_file)
    print(f"  File size: {file_size} bytes")
```

#### 4. Advanced Configuration:
```python
import pynlm
import json

# Create custom configuration
config_data = {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "replay_interval": 100,
    "consolidation_interval": 1000
}

# Create configuration from JSON
config = pynlm.createDefaultConfig()
for key, value in config_data.items():
    config.set(key, value)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

## Troubleshooting

### Common Issues and Solutions:

#### 1. Build Failures
**Symptoms**: CMake errors, compiler errors
**Solutions**:
```bash
# Check CMake version
cmake --version

# Ensure C++20 support
g++ --version

# Try clean build
rm -rf build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### 2. Python Bindings Not Working
**Symptoms**: ImportError when running Python code
**Solutions**:
```bash
# Install Python dependencies
pip install pybind11 scikit-build-core

# Build Python bindings
python setup.py build_ext --inplace

# Or install via pip
pip install -e .
```

#### 3. Performance Issues
**Symptoms**: Slow simulation, high memory usage
**Solutions**:
```bash
# Use optimized build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Reduce network size for development
./nlm --neuron_count=100
```

#### 4. Configuration Issues
**Symptoms**: Invalid configuration values
**Solutions**:
```bash
# Use valid configuration files
./nlm --config=configs/default.cfg

# Check configuration syntax
python3 -c "import json; json.load(open('configs/default.cfg'))"
```

### Debug Commands:
```bash
# Check system dependencies
g++ --version
cmake --version
python3 --version

# Check build output
ls -la build/

# Run with verbose output
./nlm --verbose

# Check for memory issues
valgrind --leak-check=full ./nlm
```

## Development Setup

### Setting Up Development Environment:

#### 1. Clone Repository:
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

#### 2. Install Dependencies:
```bash
# Install C++ dependencies (system package manager)
sudo apt-get update
sudo apt-get install build-essential cmake g++-9

# Install Python dependencies
pip install -r requirements.txt
```

#### 3. Build for Development:
```bash
# Create development build
mkdir build_dev
cd build_dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

#### 4. Code Analysis:
```bash
# Generate documentation
cd build_dev
make doc  # If Doxygen is configured

# Run tests
make nlm_test
./nlm_test
```

## Performance Optimization

### For Production:
```bash
# Optimized build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNDEBUG=ON
make -j$(nproc)

# Set environment variables
export OMP_NUM_THREADS=4
export MALLOC_ARENA_MAX=2
```

### For Development:
```bash
# Development build with debugging
cmake .. -DCMAKE_BUILD_TYPE=Debug -g -O0
make -j$(nproc)
```

## Advanced Usage

### Custom Extensions:

#### 1. Adding New Sensory Modalities:
```cpp
// In sensory/ directory
// Create new sensory input type
// Implement Vision, Audio, InternalSignals, Proprioception interfaces
```

#### 2. Custom Plasticity Rules:
```cpp
// In plasticity/ directory
// Implement custom plasticity rules inheriting from PlasticityRule
```

#### 3. Custom Memory Systems:
```cpp
// In memory/ directory
// Implement custom memory systems
```

### Performance Profiling:
```bash
# Use perf (Linux)
perf record ./nlm_phase6_demo
perf report

# Use gprof
./nlm_phase6_demo --profile
cd build
gprof nlm_phase6_demo gmon.out > profile.txt
```

## Migration Guide

### From Previous Versions:

#### 1. Configuration Changes:
- Configuration files moved from `configs/` to `configs/`
- New JSON-based configuration format
- Command line argument syntax updated

#### 2. API Changes:
- Brain class interface remains compatible
- New Python bindings for advanced features
- Enhanced memory and prediction systems

#### 3. Build System Changes:
- CMake-based build system (replacing Make)
- Python bindings via pybind11
- Separate test targets

## Support and Community

### Getting Help:
- **Documentation**: `docs/` directory
- **Issue Tracker**: GitHub issues
- **Community**: Discord/Slack channels
- **Examples**: `examples/` directory

### Reporting Issues:
```bash
# Create issue with detailed information
./nlm --version
./nlm --info
```

### Contributing:
```bash
# Fork repository
# Make changes
# Create pull request
# Ensure tests pass
```

## Conclusion

This build and installation guide provides comprehensive instructions for working with the NLM project. The build system is flexible, supporting various platforms and configurations. The Python bindings provide additional convenience for research and development work.

For the most up-to-date information, refer to the latest version of this document and the project's official documentation.

---
*Document Version: 1.0*
*Last Updated: October 16, 2026*
*Phase 6: FINAL INTEGRATION*
