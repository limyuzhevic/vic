# NLM Project - CMake Documentation

This document describes the CMake build system and available build targets for the NLM (Neural Learning Machine) project.

## Overview

NLM uses CMake for building both C++ and Python components. The build system supports multiple build types, platforms, and provides various targets for different use cases.

## Project Structure

### Core Libraries

| Target | Type | Dependencies | Purpose |
|--------|------|--------------|---------|
| `nlm_core` | Static Library | C++ Standard Library | Core brain components (neurons, synapses, dynamics, plasticity) |
| `nlm_agent` | Static Library | `nlm_core` | Agent brain interface (sensory processing, motor decoding) |
| `nlm_world` | Static Library | `nlm_core` | World simulation (environment, objects, perception) |
| `nlm_phase3` | Static Library | `nlm_core`, `nlm_agent`, `nlm_world` | Phase 3 experiment framework |
| `nlm_phase4` | Static Library | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3` | Phase 4 experiment framework |

### Executables

| Target | Type | Dependencies | Purpose |
|--------|------|--------------|---------|
| `nlm` | Executable | `nlm_core` | Main demonstration (Phase 2: Basic Neural Computation) |
| `nlm_enhanced` | Executable | `nlm_core`, `tools/NLMCommandLine.cpp` | Enhanced CLI with full command-line interface |
| `nlm_phase3_demo` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3` | Phase 3 demonstration |
| `nlm_phase3_enhanced` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3`, `tools/NLMCommandLine.cpp` | Enhanced Phase 3 with CLI |
| `nlm_phase4_demo` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3`, `nlm_phase4` | Phase 4 demonstration |
| `nlm_phase4_enhanced` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3`, `nlm_phase4`, `tools/NLMCommandLine.cpp` | Enhanced Phase 4 with CLI |
| `nlm_phase6_demo` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3`, `nlm_phase4` | Phase 6 integration demonstration |
| `nlm_phase6_enhanced` | Executable | `nlm_core`, `nlm_agent`, `nlm_world`, `nlm_phase3`, `nlm_phase4`, `tools/NLMCommandLine.cpp` | Enhanced Phase 6 with CLI |
| `nlm_benchmark` | Executable | `nlm_core` | Performance benchmarking tool |

## Building Instructions

### Standard Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (parallel build)
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Development Build

```bash
# Debug build with symbols and assertions
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release

# Minimal build (no examples)
cmake .. -DBUILD_EXAMPLES=OFF
```

### Python Bindings Build

Python bindings are automatically built via scikit-build-core from the pyproject.toml file. After building the C++ components:

```bash
# Install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

### Custom Build Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `BUILD_EXAMPLES` | Boolean | ON | Build example files and documentation |
| `CMAKE_BUILD_TYPE` | String | Release | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |

### Examples Configuration File

```bash
# Enable examples
cmake .. -DBUILD_EXAMPLES=ON

# Disable examples
cmake .. -DBUILD_EXAMPLES=OFF
```

## Running Built Targets

### Main Executables

| Target | Purpose | Usage |
|--------|---------|-------|
| `nlm` | Basic neural computation demo | `./nlm` |
| `nlm_enhanced` | Enhanced CLI with full interface | `./nlm_enhanced [command] [options]` |
| `nlm_phase3_demo` | Phase 3 world interaction demo | `./nlm_phase3_demo` |
| `nlm_phase3_enhanced` | Enhanced Phase 3 with CLI | `./nlm_phase3_enhanced [command]` |
| `nlm_phase4_demo` | Phase 4 cognition demo | `./nlm_phase4_demo` |
| `nlm_phase4_enhanced` | Enhanced Phase 4 with CLI | `./nlm_phase4_enhanced [command]` |
| `nlm_phase6_demo` | Phase 6 integration demo | `./nlm_phase6_demo` |
| `nlm_phase6_enhanced` | Enhanced Phase 6 with CLI | `./nlm_phase6_enhanced [command]` |
| `nlm_benchmark` | Performance benchmarking | `./nlm_benchmark [neurons] [steps]` |

### Command Line Interface (nlm_enhanced)

The enhanced CLI provides advanced functionality:

#### Basic Commands

- `nlm_enhanced help` - Show help message
- `nlm_enhanced run --steps 1000` - Run a simulation
- `nlm_enhanced demo phase6` - Run Phase 6 demo
- `nlm_enhanced benchmark 1000 500` - Run performance benchmark

#### Advanced Commands

- `nlm_enhanced config show` - Display current configuration
- `nlm_enhanced config set <key> <value>` - Modify configuration
- `nlm_enhanced config load <file>` - Load configuration from file
- `nlm_enhanced config save <file>` - Save configuration to file
- `nlm_enhanced explore` - Explore system capabilities
- `nlm_enhanced save <file>` - Save brain state
- `nlm_enhanced load <file>` - Load brain state
- `nlm_enhanced analyze` - Analyze brain state
- `nlm_enhanced compare <file1> <file2>` - Compare configurations

#### Examples

```bash
# Run a simulation with 1000 steps
nlm_enhanced run --steps 1000 --config my_config.cfg

# Run Phase 6 demo
nlm_enhanced demo phase6 --steps 2000

# Performance benchmark
nlm_enhanced benchmark 1000 500

# Configuration management
nlm_enhanced config show
nlm_enhanced config set brain.neuron_count 2000
nlm_enhanced config load config.json
nlm_enhanced config save config.json

# Explore system
nlm_enhanced explore

# Save/load brain state
nlm_enhanced save my_brain.bin
nlm_enhanced load my_brain.bin
```

## Python Bindings

### Installation

```bash
pip install .
```

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

### Python Examples

See `examples/example_simple.py` for a complete working example.

## Testing

### Unit Tests

```bash
# Run C++ unit tests
./nlm_test

# Or use ctest
cd build
ctest --output-on-failure
```

### Python Tests

```bash
# Run Python tests
python3 -m pytest tests/test_python_bindings.py -v

# Run all tests
python3 -m pytest tests/ -v
```

## Installation

### System-wide Installation

```bash
# Configure CMake
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Install
make install
```

### Python Package Installation

```bash
# Install Python bindings
pip install .
```

## Build Outputs

After successful compilation, the build directory will contain:

```
build/
├── libnlm_core.a
├── libnlm_agent.a
├── libnlm_world.a
├── bin/
│   ├── nlm
│   ├── nlm_enhanced
│   ├── nlm_phase3_demo
│   ├── nlm_phase3_enhanced
│   ├── nlm_phase4_demo
│   ├── nlm_phase4_enhanced
│   ├── nlm_phase6_demo
│   ├── nlm_phase6_enhanced
│   └── nlm_benchmark
├── examples/
│   ├── example_simple.py
│   └── example_simple.cpp
└── tests/
    └── ...
```

## Troubleshooting

### Common Issues

1. **Python bindings not found**
   ```bash
   pip install scikit-build-core pybind11
   pip install .
   ```

2. **CMake cannot find Python**
   ```bash
   cmake .. -DPython_EXECUTABLE=$(which python)
   ```

3. **Build errors with missing dependencies**
   ```bash
   # Check for required libraries
   sudo apt-get install build-essential cmake libpython3-dev
   ```

4. **Memory errors on large builds**
   ```bash
   # Use parallel build to reduce memory usage
   make -j$(nproc)
   ```

### Getting Help

- Documentation: https://nlm.readthedocs.io
- Repository: https://github.com/nlm-project/nlm
- Issues: https://github.com/nlm-project/nlm/issues

## License

MIT License. See LICENSE file for details.