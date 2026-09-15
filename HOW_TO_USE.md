# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Running the core NLM executable

---

## Part 1: Building the C++ project from source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
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
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Running the NLM Simulation

### Basic Usage

```bash
# Navigate to the build directory
cd /path/to/nlm/build

# Run the core simulation (Phase 2 LIF dynamics demo)
./nlm

# Or run with a configuration file
./nlm --config /path/to/configs/default.cfg

# You can also specify other options
./nlm --neuron-count 2000 --region-count 2 --connection-probability 0.15
```

### Configuration Options

The NLM simulation uses configuration files (`.cfg`) that specify:
- Number of neurons
- Number of regions
- Connection probability
- Simulation timestep
- Various plasticity and neuromodulation parameters

Configuration files are typically stored in the `configs/` directory.

### Performance Options

The simulation supports various performance optimizations including:
- Multi-threading
- SIMD vectorization
- Sparse connectivity
- Event-driven processing

These can be enabled during build time or configured in config files.

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
# Python is only needed for pybind11 bindings, which are optional
# For standard C++ build, you can disable Python bindings
cmake .. -DBUILD_PYTHON_BINDINGS=OFF
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**Expected behavior:**
- The simulation runs multiple tests demonstrating different aspects of NLM:
  - Basic connectivity test
  - Plasticity learning experiment
  - STDP verification

**Output:**
- Detailed logs showing neuron activity, synaptic changes, and learning metrics
- Final status report showing integration results

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

