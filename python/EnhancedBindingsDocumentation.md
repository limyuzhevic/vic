# Enhanced NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the Enhanced NLM Python bindings, which offer an improved Pythonic interface for interacting with the NLM computational brain system.

## Installation

The enhanced Python bindings are included with the standard NLM build. No separate installation is required:

```bash
# Build NLM with Python bindings support
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_PYTHON_BINDINGS=ON
make -j4
```

After building, the Python module will be available as `pynlm`.

## API Reference

### Core Classes

#### `Config` - Configuration Management

A Pythonic configuration interface for NLM brain parameters.

**Methods:**
- `createFromJSON(json_str: str) -> Config`
  Creates configuration from JSON string. Returns a new Config instance.
  
- `saveToFile(filename: str) -> None`
  Saves configuration to JSON file.
  
- `toJSON() -> str`
  Converts configuration to JSON string.
  
- `getOr(key: str, default_value) -> Any`
  Gets configuration value with fallback to default.

**Pythonic Access Patterns:**
- Dictionary access: `config["key"] = value` or `config["key"]`
- Iteration: `for key in config:`
- Membership: `"key" in config`
- Context manager: `with config.open():`

**Example:**
```python
import pynlm as nlm

# Create from JSON
config_json = '''{
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "brain.connection_probability": 0.1
}'''

config = nlm.Config.createFromJSON(config_json)

# Pythonic access
config["learning_rate"] = 0.01
config["batch_size"] = 32

# Save to file
config.saveToFile("my_config.json")

# Get with default
learning_rate = config.getOr("learning_rate", 0.001)
```

#### `Brain` - Neural Computation Core

The main brain computation engine implementing the NLM architecture.

**Convenience Methods:**
- `createDefault(config_json: str) -> Brain`
  Creates brain with default configuration.
  
- `runEpisode(steps: int) -> None`
  Runs complete simulation episode.
  
- `initialize() -> None`
  Initializes brain state.
  
- `reset() -> None`
  Resets brain to initial state.

**Context Manager Support:**
```python
with brain:
    brain.step(0)
    brain.step(1)
    # Brain automatically reset on exit
```

**Statistics Access:**
- `getTotalSpikeCount() -> int`
- `getActiveNeuronCount() -> int`
- `getAverageFiringRate() -> float`
- `getMemoryUsage() -> dict`
- `getStepCount() -> int`

**Example:**
```python
import pynlm as nlm

# Create brain
brain = nlm.Brain.createDefault('''{
    "neuron_count": 1000,
    "region_count": 2
}''')

# Use context manager
with brain:
    brain.runEpisode(100)

# Get statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

#### `SimpleWorld` - Environment Simulator

Simplified world environment for agent interaction.

**Convenience Methods:**
- `create(width: int, height: int, vision_width: int = 20, vision_height: int = 20) -> SimpleWorld`
  Creates and configures world in one call.
  
- `runSteps(steps: int, timestep: float = 0.01) -> None`
  Runs multiple simulation steps efficiently.

**Context Manager Support:**
```python
with world:
    world.runSteps(100, 0.01)
```

**Example:**
```python
import pynlm as nlm

# Create and configure world
world = nlm.SimpleWorld.create(100, 100, 20, 20)
world.reset()

# Run multiple steps
world.runSteps(1000, 0.01)

# Access world state
print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Vision range: {world.getVisionWidth()} x {world.getVisionHeight()}")
```

#### `AgentBrain` - Agent Interface

Bridges NLM brain with world interaction.

**Convenience Methods:**
- `createWithBrain(brain: Brain) -> AgentBrain`
  Creates agent brain with existing brain instance.
  
- `processEpisode(world: SimpleWorld, percept: dict, timestep: float) -> None`
  Processes complete agent episode.

**Key Methods:**
- `processSensoryInput(percept: dict) -> None`
- `decodeMotorCommand() -> dict`
- `applyRewardModulation(reward: float) -> None`
- `enableCuriosity(enabled: bool) -> None`

**Example:**
```python
import pynlm as nlm

# Setup
brain = nlm.Brain.createDefault()
world = nlm.SimpleWorld.create(200, 200)
agent = nlm.AgentBrain.createWithBrain(brain)

# Process complete episode
percept = world.getSensoryPercept()
agent.processEpisode(world, percept, 0.01)
```

### Advanced Classes

#### `SimulationManager` - State Management

Manages simulation state and checkpoint persistence.

**Methods:**
- `runEpisode(steps: int, checkpoint_interval: int = 100) -> None`
- `saveCheckpoint(filename: str) -> None`
- `loadCheckpoint(filename: str) -> None`
- `getState() -> dict`
- `restoreState(state: dict) -> None`

**Example:**
```python
import pynlm as nlm

# Create manager
manager = nlm.SimulationManager(brain, world)

# Run episode with checkpointing
manager.runEpisode(1000)

# Save checkpoint
manager.saveCheckpoint("checkpoint_100.bin")

# Get current state
state = manager.getState()
print(f"Total spikes: {state['total_spikes']}")
print(f"Step count: {state['step_count']}")
```

#### `ExperimentRunner` - Experiment Execution

Framework for running structured experiments.

**Methods:**
- `runAgentExperiment(world: SimpleWorld, agent: AgentBrain, episodes: int, steps_per_episode: int) -> dict`
- `runBrainStimulationExperiment(brain: Brain, steps: int, neuron_type: str, intensity: float) -> dict`
- `runRewardExperiment(world: SimpleWorld, agent: AgentBrain, rewards: list, steps: int) -> dict`

**Example:**
```python
import pynlm as nlm

# Setup
runner = nlm.ExperimentRunner()
world = nlm.SimpleWorld.create(200, 200)
brain = nlm.Brain.createDefault()
agent = nlm.AgentBrain.createWithBrain(brain)

# Run reward experiment
rewards = [1.0, 0.5, 0.0, -0.5, -1.0]
results = runner.runRewardExperiment(world, agent, rewards, 50)
print(f"Average reward: {results['average_reward']}")
print(f"Learning improvement: {results['improvement']}")
```

#### `Visualization` - Output Display

Utilities for visual representation of NLM state.

**Methods:**
- `visualizeBrain(brain: Brain) -> str`
- `visualizeWorld(world: SimpleWorld) -> str`
- `visualizeSpikePattern(brain: Brain) -> str`
- `visualizeMemoryState(brain: Brain) -> str`

**Example:**
```python
import pynlm as nlm

viz = nlm.Visualization()
print(viz.visualizeBrain(brain))
print(viz.visualizeWorld(world))
```

### Constants and Enums

#### Neuron Types
- `NeuronType.Excitatory`
- `NeuronType.Inhibitory`
- `NeuronType.Internal`

#### Error Types
- `NLMError` - Base exception class
- `ConfigError` - Configuration-related errors
- `BrainError` - Brain operation errors

**Example Error Handling:**
```python
import pynlm as nlm

try:
    brain = nlm.Brain.createDefault('''{
        "neuron_count": -100
    }''')
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
except nlm.BrainError as e:
    print(f"Brain error: {e}")
```

## Best Practices and Performance Tips

### 1. Configuration Management
```python
# Good: Use configuration files for experiments
config = nlm.Config.createFromJSON(open("experiment.cfg").read())

# Good: Validate configuration before use
config.validate()

# Good: Use named parameters for critical settings
config["brain.learning_rate"] = 0.01
config["brain.plasticity_strength"] = 0.05
```

### 2. Memory Management
```python
# Good: Use context managers for resource management
with brain:
    brain.runEpisode(100)

# Good: Monitor memory usage
state = brain.getMemoryUsage()
print(f"Memory usage: {state['memory_mb']:.2f} MB")
```

### 3. Performance Optimization
```python
# Good: Batch operations
world.runSteps(1000, 0.01)

# Good: Use efficient data structures
percept = world.getSensoryPercept()

# Good: Reuse objects when possible
agent = nlm.AgentBrain.createWithBrain(brain)
```

### 4. Debugging and Testing
```python
# Good: Use checkpoints for debugging
manager.saveCheckpoint("debug_state.bin")

# Good: Log simulation state
state = manager.getState()
print(f"Step {state['step_count']}: {state['active_neurons']} active neurons")

# Good: Profile performance
import time
start = time.time()
brain.runEpisode(100)
elapsed = time.time() - start
print(f"Episode completed in {elapsed:.2f} seconds")
```

## Migration Guide

### From Old to New API

| Old API | New API |
|---------|---------|
| `nlm.createBrain(config)` | `nlm.Brain.createDefault()` |
| `config.has("key")` | `"key" in config` |
| `config.get<int>("key")` | `config.getOr("key", default)` |
| `brain.initialize()` | `with brain:` (context manager) |
| `brain.step(step)` | `brain.runEpisode(steps)` |
| Manual loop over steps | `world.runSteps(steps, timestep)` |

### Backward Compatibility

All existing code continues to work without modification:

```python
# Old code (still works)
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
brain.step(0)
```

## Working Examples

### Example 1: Complete Simulation
```python
import pynlm as nlm

# Create everything
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

### Example 2: Configuration Management
```python
import pynlm as nlm

# Create config from JSON
config_json = '''{
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "brain.connection_probability": 0.1,
    "learning.rate": 0.01,
    "learning.momentum": 0.9
}'''

config = nlm.Config.createFromJSON(config_json)

# Modify using Pythonic access
config["learning.rate"] = 0.001
config["new.parameter"] = "test"

# Save to file
config.saveToFile("experiment_config.json")

# Convert back to JSON
config_json_output = config.toJSON()
print(config_json_output)
```

### Example 3: Advanced Experiment
```python
import pynlm as nlm

# Setup for ablation study
config = nlm.createDefaultConfig()
world = nlm.SimpleWorld.create(100, 100)
brain = nlm.createBrain(config)
brain.initialize()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Create manager and runner
manager = nlm.SimulationManager(brain, world)
runner = nlm.ExperimentRunner()

# Run experiment with checkpointing
print("Running ablation experiment...")
results = runner.runAgentExperiment(world, agent, 100, 20)

# Get results and visualize
state = manager.getState()
viz = nlm.Visualization()
print(viz.visualizeBrain(brain))

print(f"Experiment completed!")
print(f"Total spikes: {state['total_spikes']}")
print(f"Final activity: {state['active_neurons']/state['total_neurons']:.2%}")
```

## Type Hints and Documentation

The enhanced bindings include comprehensive type hints and documentation. Use Python's built-in `help()` function:

```python
import pynlm as nlm

# Get help for any class
help(nlm.Brain)
help(nlm.Config)
help(nlm.SimulationManager)

# Check available methods
print(dir(nlm.Brain))
```

## Advanced Features

### 1. Custom Config Schemas
```python
# Create specialized configuration schemas
class ExperimentConfig(nlm.Config):
    def __init__(self):
        super().__init__()
        self["experiment.name"] = "test"
        self["experiment.episodes"] = 10
        self["experiment.learning_rate"] = 0.01
```

### 2. Callback Functions
```python
# Define custom callbacks
def onStepCompleted(brain, step):
    print(f"Step {step} completed")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")

# Use callbacks in experiments
manager.setStepCallback(onStepCompleted)
```

### 3. Custom Exceptions
```python
# Define custom exception types
class CustomNLMError(nlm.NLMError):
    def __init__(self, message, error_code):
        super().__init__(message)
        self.error_code = error_code

# Raise custom exceptions
raise CustomNLMError("Custom validation error", "ERR_VALIDATION")
```

## Troubleshooting

### Common Issues

1. **Import Errors**
   - Ensure NLM was built with Python bindings
   - Check Python module installation
   - Verify Python version compatibility

2. **Configuration Errors**
   - Validate configuration before use
   - Check required parameters
   - Use descriptive error messages

3. **Memory Issues**
   - Monitor memory usage regularly
   - Use efficient data structures
   - Consider batching operations

4. **Performance Issues**
   - Profile simulation performance
   - Use efficient algorithms
   - Consider hardware acceleration

### Debugging Commands
```python
# Debug brain state
brain = nlm.Brain.createDefault()
brain.initialize()
print(f"Brain initialized: {brain.getActiveNeuronCount()} neurons")

# Debug world state
world = nlm.SimpleWorld.create(100, 100)
print(f"World created: {world.getWidth()} x {world.getHeight()}")

# Debug memory usage
manager = nlm.SimulationManager(brain, world)
state = manager.getState()
print(f"Total memory usage: {state['memory_mb']:.2f} MB")
```

## License

This documentation is part of the NLM project. Refer to the project LICENSE file for licensing information.

## Updates

This documentation is regularly updated to reflect the latest enhancements to the Python bindings. Always check for the most recent version.
