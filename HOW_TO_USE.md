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

---

### Configuration System

NLM now includes an enhanced configuration system that supports:

- **JSON/YAML Configuration Files**: Load and save configurations in standard formats
- **Configuration Categories**: Organize parameters (Brain, Dynamics, Plasticity, Development, Neuromodulation, Memory, Cognition, Sensory, Motor, Environment)
- **Configuration Validation**: Validate configuration values with custom rules
- **Default Values**: Set and use default configuration values

Example configuration files:

**JSON Configuration**:
```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.05
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001
    }
  }
}
```

**YAML Configuration**:
```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.05
plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
    learning_rate: 0.0005
```

### Configuration Examples

#### Example 1: Basic Configuration

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)
config.set("brain.connection_probability", 0.1)

# Save configuration to file
config.saveToFile("my_brain_config.json", "json")

print("Configuration saved successfully!")
```

#### Example 2: Loading and Using Configuration

```python
import pynlm

# Create a brain with configuration
config = pynlm.createDefaultConfig()

# Load from file if it exists
import os
if os.path.exists("brain_config.json"):
    config.loadFromFile("brain_config.json")

# Configure the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

#### Example 3: Advanced Configuration with Categories

```python
import pynlm

# Create configuration with metadata
config = pynlm.createDefaultConfig()

# Add categorized entries
config.addEntry("brain.neuron_count", 1000, 
               "Total number of neurons in the brain",
               "Brain")
config.addEntry("brain.region_count", 1,
               "Number of neural regions",
               "Brain")
config.addEntry("brain.connection_probability", 0.05,
               "Probability of connection between neurons",
               "Brain")
config.addEntry("plasticity.stdp.enable", true,
               "Enable Spike-Timing-Dependent Plasticity",
               "Plasticity")
config.addEntry("plasticity.hebbian.enable", true,
               "Enable Hebbian plasticity",
               "Plasticity")
config.addEntry("development.critical_period", 100,
               "Critical period duration for development",
               "Development")

# Get all keys
keys = config.getKeys()
print(f"All configuration keys: {keys}")

# Get categorized entries
brain_entries = [k for k in keys if k.startswith("brain.")]
plasticity_entries = [k for k in keys if k.startswith("plasticity.")]

print(f"Brain parameters: {brain_entries}")
print(f"Plasticity parameters: {plasticity_entries}")
```

#### Example 4: Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)

# Validate configuration
is_valid = config.validate("brain.neuron_count")
print(f"Neuron count valid: {is_valid}")

# Check if key exists
if config.has("brain.neuron_count"):
    neuron_count = config.get<int>("brain.neuron_count")
    if neuron_count:
        print(f"Neuron count: {neuron_count.value()}")

# Use getOr with default value
max_neurons = config.getOr<int>("brain.neuron_count", 1000)
print(f"Neuron count: {max_neurons}")
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
