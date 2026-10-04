# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

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

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Part 4: Command-Line Interface (NEW)

NLM now includes a comprehensive command-line interface for advanced users and system integration. The CLI provides multiple commands for running simulations, managing configurations, and analyzing results.

### Basic Usage

```bash
# Show available commands
nlm help

# Show help for a specific command
nlm help run

# Run a simulation
nlm run --steps 1000

# Get statistics about the current brain
nlm stats

# Run Phase 6 integration test
nlm phase 6 --verify

# Run a demonstration
nlm demo basic --steps 500

# Performance benchmarking
nlm benchmark --neurons 1000 --steps 1000
```

### Available Commands

#### 1. `nlm run` - Run Simulation

Run a neural simulation with specified configuration.

**Usage:**
```bash
nlm run --steps <N> [--config <file>] [--output <file>] [--log-level <LEVEL>] [--save] [--quiet]
```

**Arguments:**
- `--steps <N>` (Required): Number of simulation steps to run
- `--config <file>` (Optional): Configuration file path (default: configs/default.cfg)
- `--output <file>` (Optional): Output file for brain state (default: brain_state.bin)
- `--log-level <LEVEL>` (Optional): Log level (INFO, WARNING, ERROR) (default: INFO)
- `--save` (Optional): Save brain state after simulation
- `--quiet` (Optional): Suppress output messages

**Example:**
```bash
nlm run --steps 5000 --config configs/my_config.cfg --output results.bin --save
```

#### 2. `nlm config` - Configuration Management

Manage configuration files and settings.

**Usage:**
```bash
nlm config [list|save <file>|load <file>|show <key>|set <key> <value>]
```

**Subcommands:**
- `list`: List all configuration keys
- `save <file>`: Save current configuration to file
- `load <file>`: Load configuration from file
- `show <key>`: Show value of specific configuration key
- `set <key> <value>`: Set configuration value

**Examples:**
```bash
nlm config list
nlm config set brain.neuron_count 2000
nlm config save my_config.cfg
nlm config load my_config.cfg
```

#### 3. `nlm stats` - Display Statistics

Display brain statistics and metrics.

**Usage:**
```bash
nlm stats [--brain <file>] [--format <json|text|table>] [--verbose]
```

**Arguments:**
- `--brain <file>` (Optional): Brain state file path (default: brain_state.bin)
- `--format <json|text|table>` (Optional): Output format (default: text)
- `--verbose` (Optional): Verbose output

**Example:**
```bash
nlm stats --brain saved_brain.bin --verbose
```

#### 4. `nlm phase` - Phase Experiments

Run specific phase experiments.

**Usage:**
```bash
nlm phase <3|4|5|6> [--steps <N>] [--config <file>] [--verify]
```

**Arguments:**
- `<3|4|5|6>` (Required): Phase number to run
- `--steps <N>` (Optional): Number of steps (default: 1000)
- `--config <file>` (Optional): Configuration file (default: configs/default.cfg)
- `--verify` (Optional): Run integration verification

**Example:**
```bash
nlm phase 6 --steps 5000 --verify
```

#### 5. `nlm demo` - Run Demonstrations

Run pre-configured demonstrations for different scenarios.

**Usage:**
```bash
nlm demo <basic|plasticity|stdp|memory|neuromodulation> [--steps <N>]
```

**Arguments:**
- `<basic|plasticity|stdp|memory|neuromodulation>` (Required): Demo type
- `--steps <N>` (Optional): Number of steps (default: 500)

**Examples:**
```bash
nlm demo basic --steps 1000
nlm demo plasticity
```

#### 6. `nlm benchmark` - Performance Benchmarking

Run performance benchmarks with different configurations.

**Usage:**
```bash
nlm benchmark [--neurons <N>] [--steps <N>] [--iterations <N>] [--warmup <N>]
```

**Arguments:**
- `--neurons <N>` (Optional): Number of neurons (default: 1000)
- `--steps <N>` (Optional): Steps per iteration (default: 1000)
- `--iterations <N>` (Optional): Number of iterations (default: 5)
- `--warmup <N>` (Optional): Warmup iterations (default: 2)

**Example:**
```bash
nlm benchmark --neurons 5000 --steps 2000 --iterations 10
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
