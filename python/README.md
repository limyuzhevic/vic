# NLM Python Bindings

This directory contains the Python bindings for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The NLM Python bindings provide a Pythonic interface to the C++ NLM neural simulation framework. They offer access to complex brain simulation capabilities through a clean, intuitive Python API.

## Installation

### Prerequisites

- Python 3.8+
- CMake 3.16+
- C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- pybind11 2.11.0+
- scikit-build-core 0.5.0+

### Installation Methods

#### 1. Using pip (Recommended)

```bash
pip install pynlm
```

This will automatically build the C++ extension and install the Python bindings.

#### 2. Building from Source

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

Or use the setup script:

```bash
python setup.py
```

### 3. Development Installation

For development, install in editable mode:

```bash
pip install -e .
```

## Getting Started

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.firing_neurons} neurons firing")
```

### Complete Agent Example

```python
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run complete simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation completed!")
print(f"Final stats: {brain.getTotalNeuronCount()} neurons, {brain.getFiringNeuronCount()} firing")
```

## Advanced Features

### 1. Context Managers

```python
# Brain context manager ensures proper initialization/cleanup
with nlm.brain_simulation(brain):
    brain.step(0)
    print("Brain simulation running...")

# World context manager for automatic reset
with nlm.world_context(world):
    world.update(0.1)
    print("World updated and reset")

# Agent context manager for automatic reset
with nlm.agent_context(agent):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
```

### 2. Property-Based Accessors

```python
# Direct property access instead of getter methods
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.total_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Average firing rate: {brain.average_firing_rate}")
print(f"Development stage: {brain.developmental_stage}")

# Modify config properties
config = pynlm.createDefaultConfig()
config["neuron_count"] = 2000
config["synapse_density"] = 0.15
```

### 3. Dictionary Interface

```python
# Config can be accessed like a dictionary
config = pynlm.createDefaultConfig()
config["neuron_count"] = 3000
config["enable_curiosity"] = True

# Get values with defaults
neuron_count = config.get("neuron_count", 1000)
curiosity_enabled = config.get("enable_curiosity", False)

# Convert to Python dictionary
config_dict = nlm.config_to_dict(config)
```

### 4. Performance Monitoring

```python
# Get performance statistics
brain_stats = nlm.get_brain_stats(brain)
print(f"Brain stats: {dict(brain_stats)}")

world_stats = nlm.get_world_stats(world)
print(f"World stats: {dict(world_stats)}")

agent_stats = nlm.get_agent_stats(agent)
print(f"Agent stats: {dict(agent_stats)}")
```

### 5. Batch Operations

```python
# Run many steps efficiently
spike_count = nlm.batch_steps(brain, 1000)
print(f"After 1000 steps: {spike_count} spikes")

# Run many world updates efficiently
final_time = nlm.batch_world_updates(world, 500, 0.02)
print(f"World simulation time: {final_time:.2f}")
```

### 6. Data Analysis

```python
# Get iterators for data analysis
regions_iterator = nlm.iter_regions(brain)
neurons_iterator = nlm.iter_neurons(brain, region_id)

# List comprehensions for analysis
active_neurons = nlm.get_active_neuron_ids(brain)
firing_neurons = nlm.get_firing_neuron_ids(brain)
region_names = nlm.get_region_names(brain)

print(f"Active neurons: {len(active_neurons)}")
print(f"Firing neurons: {len(firing_neurons)}")
print(f"Regions: {region_names}")
```

### 7. Simulation Convenience Functions

```python
# Run a complete agent episode with options
options = {
    "enable_learning": True,
    "enable_curiosity": True,
    "timestep": 0.05
}

result = nlm.run_agent_episode(agent, world, 100, options)
print(f"Episode result: {result}")
```

### 8. Serialization

```python
# Save and load brain state
brain.save("trained_brain.bin")
print("Brain saved to trained_brain.bin")

loaded_brain = nlm.deserialize_brain("trained_brain.bin")
if loaded_brain:
    print(f"Brain loaded with {loaded_brain.total_neurons} neurons")
```

### 9. Debugging and Diagnostics

```python
# Get detailed debug information
debug_info = nlm.debug_brain_state(brain)
print(f"Debug info: {debug_info}")
```

## Enums and Constants

### Neuron Types

- `nlm.NeuronType.Excitatory`
- `nlm.NeuronType.Inhibitory`
- `nlm.NeuronType.Modulatory`
- `nlm.NeuronType.Sensory`
- `nlm.NeuronType.Motor`
- `nlm.NeuronType.Internal`

### Action Types

- `nlm.ActionType.MoveForward`
- `nlm.ActionType.MoveBackward`
- `nlm.ActionType.MoveLeft`
- `nlm.ActionType.MoveRight`
- `nlm.ActionType.TurnLeft`
- `nlm.ActionType.TurnRight`
- `nlm.ActionType.Look`
- `nlm.ActionType.LookUp`
- `nlm.ActionType.LookDown`
- `nlm.ActionType.Interact`
- `nlm.ActionType.Eat`
- `nlm.ActionType.Drink`
- `nlm.ActionType.Rest`
- `nlm.ActionType.Wait`
- `nlm.ActionType.Custom`

### Developmental Stages

- `nlm.DevelopmentalStage.Initial`
- `nlm.DevelopmentalStage.CriticalPeriod`
- `nlm.DevelopmentalStage.Maturation`
- `nlm.DevelopmentalStage.Adult`
- `nlm.DevelopmentalStage.Aging`

### Invalid IDs

- `nlm.INVALID_NEURON_ID`
- `nlm.INVALID_SYNAPSE_ID`
- `nlm.INVALID_REGION_ID`
- `nlm.INVALID_POPULATION_ID`

## Examples

### 1. Silent Brain

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### 2. Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### 3. Complete Agent with Learning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Advanced Examples

Run the advanced examples:

```bash
python examples/advanced_examples.py
```

This includes examples for:
- Context managers
- Configuration management
- Agent simulation
- Data analysis
- Batch operations
- Debugging
- Serialization
- Advanced configuration

## Testing

Run the NLM tests:

```bash
cd build
ctest --output-on-failure
```

Or run Python tests (if any):

```bash
pytest
```

## Troubleshooting

### Common Issues

#### "ImportError: No module named pynlm"
```bash
pip install --force-reinstall pynlm
```

#### "Segmentation fault"
```bash
# Make sure you call initialize() before step()
# Use context managers for automatic cleanup
```

#### "My brain isn't doing anything"
```bash
# Did you call brain.initialize()?
# Try increasing the number of steps
```

#### "The agent isn't moving"
```bash
# Did you call world.applyMotorCommand()?
# Make sure world.update() is being called
```

## Build Instructions

### For Developers

To build and modify the Python bindings:

```bash
# Clone the repository
# cd into the nlm directory

# Build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build the extension
python setup.py build_ext --inplace

# Install in development mode
pip install -e .
```

### CMake Build

For more control over the build process:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The build will create the `pynlm` Python module and install it.

## License

NLM Python Bindings are part of the NLM (Neural Learning Machine) project.
See the main LICENSE file for details.

## Support

For support, visit the NLM project repository or contact the development team.

---

**Note**: The NLM Python bindings are optimized for performance while maintaining Pythonic usability. They provide a bridge between the powerful C++ neural simulation engine and Python developers, enabling rapid prototyping and experimentation with artificial brain systems.