# NLM Python Bindings Quick Start

This document provides a comprehensive guide to using the NLM Python bindings for the Neural Learning Machine (熙然) framework.

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler (GCC 7+, Clang 5+, or MSVC 2019+)
- CMake 3.16 or higher
- pybind11 and scikit-build-core

### Install from Source

```bash
# Clone the NLM repository
cd /path/to/nlm

# Build and install Python bindings
pip install -e .

# Or build just the Python bindings
pip install scikit-build-core pybind11
pip install .
```

### Install via Conda

```bash
conda install -c conda-forge pynlm
```

## Quick Start

### 1. Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.set("neuron_count", 500)
config.set("region_count", 2)
config.set("connection_probability", 0.15)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

### 2. Create a Complete Agent System

```python
import pynlm
from pynlm.utils import create_example_brain, create_example_world, create_example_agent_brain

# Create learning-ready brain
brain = create_example_brain(300)

# Create world environment
world = create_example_world(30, 30)

# Create agent brain interface
agent_brain = create_example_agent_brain(brain, world)

print("Agent system created successfully!")
print(f"World: {world.getWidth()}x{world.getHeight()}")
print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

### 3. Learning Simulation

```python
import time
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld(20, 20)
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run learning simulation
for step in range(100):
    current_time = step * 0.01
    
    # Create sensory input
    sensory_input = pynlm.Vision(5, 5, 3)
    sensory_input.setData([0.1] * 25)
    
    # Process sensory input
    agent_brain.processSensoryInput(sensory_input)
    
    # Update world
    world.update(current_time)
    
    # Take action
    action = pynlm.MotorCommand.MoveForward
    result = world.applyMotorCommand(action, current_time)
    
    # Apply reward
    if result.success:
        agent_brain.applyRewardModulation(result.reward, 0.1)
    
    if step % 20 == 0:
        print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, Active={brain.getActiveNeuronCount()}")
```

### 4. Configuration Management

```python
import pynlm
from pynlm.utils import show_config

# Create configuration
config = pynlm.createDefaultConfig()

# Customize
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.005)
config.set("plasticity_learning_rate", 0.02)

# View configuration
print(show_config(config))

# Save configuration
config.saveToFile("my_config.cfg")
```

### 5. Plasticity Experiments

```python
import pynlm
from pynlm.examples import plasticity_experiment_example

# Run plasticity experiment
plasticity_experiment_example()
```

### 6. Configuration Explorer

```python
import pynlm
from pynlm.examples import configuration_explorer_example

# Explore different configurations
configuration_explorer_example()
```

### 7. Running All Examples

```bash
python -m pynlm.examples
```

## Advanced Usage

### Custom Brain Configuration

```python
import pynlm
from pynlm.utils import create_example_brain

# Create brain with specific parameters
brain = create_example_brain(neuron_count=500, regions=3)

# Configure for specific learning task
brain.getConfig().set("plasticity_learning_rate", 0.05)
brain.getConfig().set("reward_discount_factor", 0.95)
```

### World Customization

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld(50, 50)
world.configure(50, 50, 7, 7)  # width, height, vision_width, vision_height

# Set agent start position
world.setAgentStart(25, 25)

# Customize world properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
```

### Agent Brain Configuration

```python
import pynlm

# Create agent brain with specific settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld(20, 20)

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Configure agent brain
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
```

## Utility Functions

### Core Utilities

| Function | Description |
|----------|-------------|
| `print_info()` | Print NLM Python bindings information |
| `show_config(config)` | Display configuration in user-friendly format |
| `create_example_brain(neuron_count=100)` | Create a brain with example configuration |
| `create_example_world(width, height)` | Create a world with example setup |
| `create_example_agent_brain(brain, world)` | Create an agent brain with example setup |

### Benchmarking

| Function | Description |
|----------|-------------|
| `run_benchmarks(brain, world, duration, verbose)` | Run comprehensive performance benchmarks |

## Available Classes

### Core Classes

- **Config**: Configuration management
- **Brain**: Neural brain simulation
- **SimpleWorld**: Environment simulation

### Agent System

- **AgentBrain**: Interface between brain and world
- **SensoryPercept**: Processed sensory input
- **AgentBody**: Agent state (position, orientation, etc.)

### Sensory Processing

- **SensoryInput**: Base class for sensory input
  - **Vision**: Visual sensory input (width, height, channels)
  - **Audio**: Audio sensory input (sample rate, samples)
  - **InternalSignals**: Internal neural signals

### Motor System

- **Action**: Motor action representation
- **ActionType**: High-level action types
- **MotorCommand**: Low-level motor commands

### Neural Components

- **NeuronId**, **SynapseId**, **RegionId**, **PopulationId**: Unique identifiers
- **NeuronType**: Neuron types (Excitatory, Inhibitory, Modulatory, etc.)
- **SynapseType**: Synapse types (Excitatory, Inhibitory, etc.)

## Examples Directory

The `pynlm/examples.py` module contains comprehensive examples demonstrating:

1. **Basic Brain Example**: Simple brain creation and initialization
2. **Sensory-Motor Agent**: Complete agent with perception and action
3. **Configuration Explorer**: Testing different configuration combinations
4. **Plasticity Experiment**: Demonstrating STDP and Hebbian learning

Run all examples:

```bash
python -m pynlm.examples
```

## Configuration Files

### Default Configuration

The default configuration (`configs/default.cfg`) includes:

```
# Neural computation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Neuromodulation settings
dopamine_baseline = 0.1

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

### Configuration Override

You can override configuration via command line:

```bash
# Override configuration file
./nlm --config my_config.cfg

# Override specific parameters
./nlm --neuron_count=500 --connection_probability=0.2
```

## Command Line Tools

The NLM Python bindings include several command line tools:

### `nlm-info`

Display NLM Python bindings information:

```bash
nlm-info
```

### `nlm-config`

Show configuration details:

```bash
nlm-config
```

### `nlm-benchmark`

Run performance benchmarks:

```bash
nlm-benchmark --duration 30 --verbose
```

## Development

### Building Python Bindings

```bash
cd /path/to/nlm
python -m pip install -e .
```

### Running Tests

```bash
# Run Python bindings tests
pytest

# Run all tests including C++ tests
make test
```

### Adding New Features

1. **Add new Python classes**: Extend `bindings.cpp` in the Python CMakeLists
2. **Add utility functions**: Add to `pynlm/utils.py`
3. **Add examples**: Add new examples to `pynlm/examples.py`
4. **Add command line tools**: Add scripts to `pynlm/` directory

## Troubleshooting

### Common Issues

**Issue 1: NLM Python bindings not available**

```python
import pynlm
# If this fails, check:
# 1. Installation: pip install -e .
# 2. Build output: Check CMake build logs
# 3. Environment: Ensure Python can find the module
```

**Issue 2: Configuration not loading**

```python
# Check file paths
config.saveToFile("valid_path/config.cfg")
# Use absolute paths
config.saveToFile("/home/user/config.cfg")
```

**Issue 3: Memory issues**

```python
# Use smaller configurations for testing
brain = create_example_brain(neuron_count=100)
# Monitor memory usage during long simulations
```

### Debug Commands

```bash
# Check build status
python -c "import pynlm; print('NLM available')"

# Check version
nlm-info

# Run simple test
python -c "import pynlm; config = pynlm.createDefaultConfig(); print('Config created')"
```

## Version History

### Version 0.1.0
- Initial release with core Python bindings
- Basic configuration management
- Agent brain interface
- Sensory and motor systems

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
