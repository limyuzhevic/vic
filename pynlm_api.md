# NLM Python API Documentation

## Overview

This document provides comprehensive API documentation for the NLM Python bindings. These bindings expose the full NLM (Neural Learning Machine) functionality to Python, allowing users to create, simulate, and analyze artificial brains using Python's ecosystem.

## Module Structure

```
import pynlm

# Core brain classes
pynlm.Brain
pynlm.BrainConfig
pynlm.createBrain(config)
pynlm.createDefaultConfig()

# Agent interface classes  
pynlm.AgentBrain
pynlm.createAgentBrain(brain)

# World simulation classes
pynlm.SimpleWorld
pynlm.createSimpleWorld()

# Configuration classes
pynlm.Config

# Enumerations
pynlm.NeuronType
pynlm.DevelopmentalStage
pynlm.ActionType
pynlm.WorldObjectType

# Constants and utilities
pynlm.__version__
pynlm.getVersion()
```

## Quick Start

```python
import pynlm

# Check NLM version
print(f"NLM Python version: {pynlm.__version__}")

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Brain Class

### `pynlm.createBrain(config)`

Creates and returns a new Brain instance.

**Parameters:**
- `config` (BrainConfig): Configuration for the brain

**Returns:**
- `Brain`: Configured brain instance

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity_learning_rate", 0.01)

brain = pynlm.createBrain(config)
```

### Brain Methods

#### Initialization and Control

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain state and connections | None | None |
| `step(step_number)` | Run one simulation step | `step_number` (int) | None |
| `step(step_number, time)` | Run step with timestamp | `step_number` (int), `time` (float) | None |
| `reset()` | Reset brain to initial state | None | None |

#### Sensory and Motor Interfaces

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `receiveSensoryInput(input)` | Inject sensory data into brain | `input` (SensoryInput) | None |
| `produceAction()` | Generate motor command | None | Action |
| `getRegion(region_id)` | Get brain region by ID | `region_id` (int) | NeuralRegion |
| `addRegion(name)` | Add new brain region | `name` (str) | int (region_id) |

#### Statistics and Monitoring

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getTotalNeuronCount()` | Total neurons in brain | None | int |
| `getTotalSynapseCount()` | Total synapses in brain | None | int |
| `getFiringNeuronCount()` | Neurons currently firing | None | int |
| `getAverageFiringRate()` | Average firing rate (Hz) | None | float |
| `getTotalSpikeCount()` | Total spikes generated | None | int |
| `getExcitationInhibitionRatio()` | E/I balance | None | float |
| `getDevelopmentalStage()` | Current developmental stage | None | DevelopmentalStage |

#### State Persistence

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `save(filename)` | Save brain state to file | `filename` (str) | None |
| `load(filename)` | Load brain state from file | `filename` (str) | None |

### Configuration Class

#### `pynlm.createDefaultConfig()`

Creates a default configuration with standard parameters.

**Returns:**
- `BrainConfig`: Default configuration object

#### Configuration Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `set(key, value)` | Set configuration value | `key` (str), `value` (varies) | None |
| `get(key)` | Get configuration value | `key` (str) | varies |
| `has(key)` | Check if key exists | `key` (str) | bool |
| `getKeys()` | Get all configuration keys | None | List[str] |
| `loadFromFile(filename)` | Load config from file | `filename` (str) | None |
| `saveToFile(filename)` | Save config to file | `filename` (str) | None |

#### Configuration Access Patterns

```python
import pynlm

config = pynlm.createDefaultConfig()

# Direct access
neuron_count = config.get("brain.neuron_count")
region_count = config.get("brain.region_count")

# Check existence
if config.has("neuromod.dopamine_baseline"):
    dopamine_level = config.get("neuromod.dopamine_baseline")

# Modify configuration
config.set("brain.neuron_count", 5000)
config.set("plasticity.learning_rate", 0.001)

# Get all keys
all_keys = config.getKeys()
```

### AgentBrain Class

#### `pynlm.createAgentBrain(brain)`

Creates an AgentBrain interface that connects a Brain to a World.

**Parameters:**
- `brain` (Brain): Brain instance to control

**Returns:**
- `AgentBrain`: Agent-brain interface

#### AgentBrain Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize agent with world | `world` (SimpleWorld) | None |
| `processSensoryInput(percept)` | Process sensory input | `percept` (SensoryInput) | None |
| `decodeMotorCommand()` | Decode motor command from brain | None | Action |
| `applyRewardModulation(reward, predicted)` | Apply reward signal | `reward` (float), `predicted` (float) | None |
| `updateDevelopment(timestep)` | Update developmental stage | `timestep` (float) | None |

#### Subsystem Control

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `enableRewardModulation(enabled)` | Enable/disable reward learning | `enabled` (bool) | None |
| `enableStructuralPlasticity(enabled)` | Enable/disable structural growth | `enabled` (bool) | None |
| `enableDevelopment(enabled)` | Enable/disable development | `enabled` (bool) | None |
| `enableCuriosity(enabled)` | Enable/disable curiosity | `enabled` (bool) | None |

#### Agent Statistics

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getCuriosityLevel()` | Current curiosity value | None | float |
| `getNoveltyLevel()` | Current novelty detection | None | float |
| `getPredictionError()` | Prediction error signal | None | float |
| `getNeuromodulationLevel()` | Overall neuromodulation level | None | float |
| `getBrainState()` | Get agent's brain state | None | BrainState |

### SimpleWorld Class

#### `pynlm.createSimpleWorld()`

Creates a simple 2D environment for agent interaction.

**Returns:**
- `SimpleWorld`: Configured world instance

#### World Configuration

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent start position
world.setAgentStart(x=10.0, y=10.0)

# Reset world
world.reset()

# Update world (time passes)
world.update(timestep=0.1)
```

#### World Interaction Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world dimensions | Various | None |
| `reset()` | Reset world to initial state | None | None |
| `update(timestep)` | Update world by timestep | `timestep` (float) | None |
| `applyMotorCommand(action, time)` | Apply agent's action | `action` (Action), `time` (float) | None |
| `getSensoryPercept()` | Get current sensory input | None | SensoryInput |
| `getAgentBody()` | Get agent's physical state | None | BodyState |

#### World Object Management

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `addObject(x, y, type, value)` | Add world object | `x, y` (float), `type` (WorldObjectType), `value` (float) | None |
| `removeObject(x, y)` | Remove object at position | `x, y` (float) | None |
| `isValidPosition(x, y)` | Check if position is valid | `x, y` (float) | bool |

#### World Properties

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `getWidth()` | Get world width | None | int |
| `getHeight()` | Get world height | None | int |
| `getSimulationTime()` | Get current simulation time | None | float |
| `getMaxEnergy()` | Get maximum agent energy | None | float |
| `setMaxEnergy(value)` | Set maximum agent energy | `value` (float) | None |
| `setRandomSeed(seed)` | Set random seed | `seed` (int) | None |

### SensoryInput Class

Represents sensory data received by the agent.

#### Properties

- `getVision()` → List[List[float]]: Visual input (grayscale 0-255)
- `getTouch()` → List[float]: Touch/force sensor readings
- `getInternal()` → List[float]: Internal state signals
- `get proprioception()` → List[float]: Body position/sensor data

### Action Class

Represents motor commands generated by the brain.

#### Action Types

| Property | Description |
|----------|-------------|
| `type` | ActionType enum (MoveForward, TurnLeft, Interact, Eat, etc.) |
| `magnitude` | Action strength (0.0-1.0) |
| `duration` | Action duration in seconds |

### BrainState Class

Agent's internal brain state representation.

#### Properties

- `getMemoryFiring()` → float: Working memory activation level
- `getAttentionLevel()` → float: Attention strength
- `getPredictionConfidence()` → float: Prediction certainty
- `getEnergyLevel()` → float: Current energy level

### Enumerations

#### NeuronType

| Value | Description |
|-------|-------------|
| `Excitation` | Excitatory neuron (depolarizing) |
| `Inhibition` | Inhibitory neuron (hyperpolarizing) |
| `Sensory` | Sensory neuron (receives input) |
| `Motor` | Motor neuron (outputs actions) |
| `Modulatory` | Neuromodulatory neuron |
| `Internal` | Internal neuron (maintenance) |

#### DevelopmentalStage

| Value | Description |
|-------|-------------|
| `Initial` | Newborn brain (high plasticity) |
| `CriticalPeriod` | Sensitive learning period |
| `Maturation` | Development toward adult state |
| `Adult` | Fully developed brain |
| `Aging` | Age-related decline |

#### ActionType

| Value | Description |
|-------|-------------|
| `MoveForward` | Move in forward direction |
| `MoveBackward` | Move in backward direction |
| `TurnLeft` | Rotate left |
| `TurnRight` | Rotate right |
| `TurnAround` | Rotate 180 degrees |
| `Interact` | Interact with world object |
| `Eat` | Consume food/resource |
| `Wait` | Do nothing |

#### WorldObjectType

| Value | Description |
|-------|-------------|
| `Empty` | No object |
| `Resource` | Consumable resource |
| `Hazard` | Dangerous object |
| `Wall` | Impassable barrier |
| `Marker` | Special landmark |

### Constants and Utilities

#### Version Information

```python
import pynlm

# Version string
print(pynlm.__version__)

# Version check
if pynlm.getVersion() >= "0.4.0":
    # Use new features
    pass
```

#### Error Handling

The Python bindings raise standard Python exceptions:

- `ValueError`: Invalid parameter values
- `KeyError`: Missing configuration keys
- `RuntimeError`: Simulation errors (uninitialized brain, etc.)
- `IOError`: File I/O errors

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except (ValueError, KeyError, RuntimeError) as e:
    print(f"NLM error: {e}")
```

## Complete Example

Here's a complete example demonstrating all major API features:

```python
import pynlm

def run_complete_experiment(config_overrides=None):
    """Run a complete NLM experiment with all systems"""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # Apply configuration overrides if provided
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # 2. Create brain with configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent-brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run complete simulation loop
    experiment_data = {
        'steps': [],
        'rewards': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'development_stages': []
    }
    
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect data
        if step % 10 == 0:  # Sample every 10 steps
            experiment_data['steps'].append(step)
            experiment_data['rewards'].append(reward)
            experiment_data['curiosity_levels'].append(agent.getCuriosityLevel())
            experiment_data['novelty_levels'].append(agent.getNoveltyLevel())
            experiment_data['prediction_errors'].append(agent.getPredictionError())
            experiment_data['development_stages'].append(brain.getDevelopmentalStage())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Reward: {reward:.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent, experiment_data

# Run the experiment
brain, world, agent, data = run_complete_experiment({
    "brain.neuron_count": 2000,
    "plasticity.learning_rate": 0.01,
    "development.synaptogenesis_rate": 0.01
})

print(f"\nExperiment complete!")
print(f"Total steps: {len(data['steps'])}")
print(f"Average curiosity: {sum(data['curiosity_levels']) / len(data['curiosity_levels']):.3f}")
print(f"Final development stage: {data['development_stages'][-1]}")
```

## Installation

### From PyPI

```bash
pip install pynlm
```

### From Source

```bash
# Clone repository
cd /path/to/nlm

# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Build Dependencies

- **CMake** >= 3.16
- **C++ Compiler** with C++20 support (GCC 10+, Clang 12+, or MSVC 2019+)
- **Python** >= 3.8
- **pybind11** >= 2.11.0

## Troubleshooting

### Common Issues

**"ImportError: No module named 'pynlm'"**

```bash
# Reinstall the package
pip install --force-reinstall .

# Or install in development mode
pip install -e .
```

**"RuntimeError: Brain not initialized""**

```python
# Always initialize before using
brain = pynlm.createBrain(config)
brain.initialize()  # REQUIRED
brain.step(0)
```

**"Segmentation fault during simulation"**

```bash
# Try Release build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Check for memory issues in large configurations
config.set("brain.neuron_count", 1000)  # Start smaller
```

**"Configuration parameter not found"**

```python
# Check if parameter exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")
else:
    print("Parameter not found in configuration")
```

### Getting Help

- **Documentation**: Read this file and the main NLM documentation
- **Examples**: Check the examples directory in the source repository
- **Issues**: Report bugs at the NLM GitHub repository
- **Community**: Join the NLM discussion forum

## See Also

- **README.md** - Project overview and getting started
- **HOW_TO_USE.md** - Technical implementation details
- **TUTORIAL.md** - Step-by-step learning guide
- **CONFIG_REFERENCE.md** - Complete configuration parameter reference
- **docs/ARCHITECTURE.md** - System architecture documentation
- **docs/SCIENCE.md** - Scientific background and biological inspiration
- **docs/EXPERIMENTS.md** - Detailed experiment descriptions

## Version History

- **v0.4.0**: Added AgentBrain interface and complete Phase 3/4 integration
- **v0.3.0**: Added prediction and memory systems
- **v0.2.0**: Added world interaction and developmental mechanisms
- **v0.1.0**: Initial release with basic neural core

The Python API is maintained alongside the C++ core and updated with each major NLM release.
