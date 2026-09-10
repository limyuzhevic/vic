# Build Instructions and Configuration for NLM
# Agent Brain Enhancements - Phase 6 Integration

## Overview
This document provides comprehensive instructions for building and running the enhanced NLM (Neural Learning Machine) Agent Brain system.

## Dependencies

### System Dependencies
- C++17 or higher compiler (GCC 9+, Clang 9+, MSVC 2019+)
- CMake 3.16 or higher
- pthread support
- Optional: OpenMP for parallel processing
- Optional: Python 3.8+ with pybind11 for Python bindings

### Build Tools
- CMake
- Make (or MSBuild on Windows)
- ninja (optional, faster builds)

## Standard Build Process

### Unix/Linux/macOS
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Install (optional)
sudo make install
```

### Windows
```cmd
# Using Visual Studio CMake generator
cmake -B build -G "Visual Studio 16 2019" ..
cmake --build build --config Release
```

### Using Ninja (Recommended for faster builds)
```bash
mkdir build
cd build
cmake .. -G ninja
cmake --build . -j
```

## Build Options

### Configuration Options
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=20 \
  -DBUILD_PYTHON_BINDINGS=ON \
  -DENABLE_TESTS=ON \
  -DENABLE_PERFORMANCE_PROFILING=ON \
  -DAGENT_BRAIN_ADVANCED=ON \
  -DNEURAL_NETWORK_OPTIMIZATIONS=ON
```

### Build Type Options
- `Debug` - Full debugging symbols, no optimizations
- `Release` - Optimized build with minimal debugging
- `RelWithDebInfo` - Optimized with debugging symbols
- `MinSizeRel` - Optimized for minimum size

## Building Specific Components

### Core Brain Library
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target nlm_core
```

### Agent Library (Enhanced)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target nlm_agent
```

### World Simulation Library
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target nlm_world
```

### Experiment Libraries
```bash
# Phase 3 experiments (world interaction)
cmake --build . --target nlm_phase3

# Phase 4 experiments (cognitive emergence)
cmake --build . --target nlm_phase4
```

### Executables
```bash
# Main neural computation demo
cmake --build . --target nlm

# Phase 3 demo (world interaction)
cmake --build . --target nlm_phase3_demo

# Phase 4 demo (cognitive emergence)
cmake --build . --target nlm_phase4_demo

# Phase 6 integration demo
cmake --build . --target nlm_phase6_demo
```

## Python Bindings

### Building Python Module
```bash
# From the project root
mkdir build
cd build
cmake .. -DBUILD_PYTHON_BINDINGS=ON
cmake --build . --target pynlm

# Install Python module
cmake --build . --target install
```

### Using Python Bindings
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with enhanced features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced agent features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Running Examples and Demos

### Basic Neural Computation
```bash
./build/nlm [options]

Options:
  --config <file>    Configuration file (default: configs/default.cfg)
  --neurons <n>      Number of neurons (default: 500)
  --timestep <t>     Simulation timestep (default: 0.001)
  --steps <n>        Number of simulation steps (default: 1000)
```

### Phase 3 Demo (World Interaction)
```bash
./build/nlm_phase3_demo [options]
```

### Phase 4 Demo (Cognitive Emergence)
```bash
./build/nlm_phase4_demo [options]
```

### Phase 6 Integration Demo
```bash
./build/nlm_phase6_demo
```

## Testing

### Running Unit Tests
```bash
mkdir build
cd build
cmake .. -DENABLE_TESTS=ON
cmake --build . --target test
ctest -V  # Run with verbose output
```

### Running Integration Tests
```bash
./build/nlm_phase6_demo --test-mode
```

### Performance Benchmarks
```bash
./build/benchmarks
```

## Advanced Configuration

### Custom Configuration File
```cpp
// Create custom configuration
cat > my_config.cfg << EOF
[random_seed] 42
[neuron_count] 2000
[region_count] 2
[connection_probability] 0.2
[stdp_ltp_weight] 0.025
[stdp_ltd_weight] 0.01
[synaptogenesis_rate] 0.0002
[pruning_rate] 0.00001
EOF

# Run with custom config
./build/nlm --config my_config.cfg
```

### Command Line Arguments
```cpp
// Override configuration from command line
./build/nlm --neurons 1000 --timestep 0.01 --steps 5000
```

## Performance Optimization

### Recommended Build Optimizations
```bash
# For maximum performance
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp" \
  -DENABLE_PERFORMANCE_PROFILING=ON \
  -DENABLE_VECTORIZATION=ON \
  -DENABLE_CACHE_OPTIMIZATION=ON

# Use Ninja for faster builds
mkdir build
cd build
cmake .. -G ninja
cmake --build . -j$(nproc)
```

### Runtime Optimizations
- Enable OpenMP for parallel processing
- Use larger neuron counts for more realistic simulations
- Increase checkpoint intervals for better performance
- Enable structural plasticity for adaptive networks

## Installation

### System Installation
```bash
# Install to system directories
sudo cmake --build . --target install

# Check installation
find /usr/local/include -name "nlm" -type d
find /usr/local/lib -name "*nlm*" -type f
```

### Python Package Installation
```bash
# Install Python module to user site
pip install ./build/pynlm-*.whl

# Or install system-wide
sudo pip install ./build/pynlm-*.whl
```

## Troubleshooting

### Common Issues

#### Build Errors
**Problem**: "unable to find required OpenMP support"
**Solution**: Install OpenMP development packages:
```bash
# Ubuntu/Debian
sudo apt-get install libomp-dev

# macOS
brew install libomp

# CentOS/RHEL
sudo yum install libomp-devel
```

**Problem**: "CMakeError: The 'CXX' compiler is not supported by CMake"
**Solution**: Install modern C++ compiler:
```bash
# Ubuntu/Debian
sudo apt-get install g++

# macOS
xcode-select --install
```

#### Runtime Errors
**Problem**: "Segmentation fault during simulation"
**Solution**: Check memory limits and configuration:
```bash
# Increase swap if needed
sudo swapon -s

# Check system memory
free -h

# Reduce neuron count in configuration
[neuron_count] 500
```

**Problem**: "Python bindings not found"
**Solution**: Build and install Python bindings:
```bash
mkdir build
cd build
cmake .. -DBUILD_PYTHON_BINDINGS=ON
cmake --build . --target pynlm
```

#### Performance Issues
**Problem**: Slow simulation performance
**Solution**: Optimize build and configuration:
```bash
# Use optimized build
cmake .. -DCMAKE_BUILD_TYPE=Release -O3

# Reduce checkpoint frequency
[checkpoint_interval] 10000

# Disable unnecessary features
[enable_visualization] false
```

### Debug Mode
```bash
# Build with debugging symbols
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
cmake --build . -j$(nproc)

# Run with gdb

# For gdb debugging
export PYTHONPATH=./pynlm:$PYTHONPATH

# For valgrind
valgrind --tool=memcheck ./nlm_phase6_demo
```

## Additional Resources

### Documentation
- `README.md` - Basic usage and overview
- `easy_usage.md` - Beginner-friendly guide
- `HOW_TO_USE.md` - Comprehensive Python API documentation
- `docs/` - Technical architecture documentation

### Examples
- `examples/` - Additional code examples
- `python/examples/` - Python usage examples

### Support
- GitHub Issues: Report bugs and request features
- Documentation: API reference and usage guides
- Community: Discussion forums and mailing lists

## Version Information

### Current Version
- NLM Version: 0.1.0
- Phase: 6 - Final Integration
- Build Date: $(date)
- Compiler: $(c++ --version)
- CMake: $(cmake --version)

### Compatibility
- C++20 standard
- Python 3.8+
- Linux, macOS, Windows support

## License
MIT License - See LICENSE file for details