# NLM Python API Reference

This document provides comprehensive documentation for the NLM Python API (pynlm), including detailed descriptions of all classes, methods, and functions.

## Overview

The NLM Python API provides access to the Neural Learning Machine simulation framework through Python bindings for the C++ core. It allows you to:

- Create and simulate artificial brains with spiking neural networks
- Interact with virtual environments
- Control agent behavior through sensory input and motor output
- Configure neural parameters and plasticity rules
- Save and load brain states

## Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Install pre-built package
pip install pynlm
```

## Core Classes

### Brain

Represents a complete neural simulation brain with integrated memory, prediction, cognition, and neuromodulation systems.

#### Creation and Initialization

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Main Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize the brain with current configuration | None | None |
| `step(step_number)` | Execute one simulation step | `step_number: int` | None |
| `step(step_number, time)` | Execute one simulation step with timestamp | `step_number: int, time: float` | None |
| `reset()` | Reset brain to initial state | None | None |
| `receiveSensoryInput(input)` | Inject sensory data into the brain | `input: SensoryInput` | None |
| `produceAction()` | Get motor command from brain activity | None | `Action` |
| `injectCurrent(neuron, current)` | Inject current into specific neuron | `neuron: NeuronId, current: float` | None |
| `injectCurrentToNeurons(neuron_type, current)` | Inject current into all neurons of type | `neuron_type: NeuronType, current: float` | None |

#### Statistics and Information

| Method | Description | Returns |
|--------|-------------|---------|
| `getTotalNeuronCount()` | Total number of neurons in brain | `int` |
| `getTotalSynapseCount()` | Total number of synapses | `int` |
| `getFiringNeuronCount()` | Number of currently firing neurons | `int` |
| `getAverageFiringRate()` | Average firing rate across all neurons | `float` |
| `getExcitationInhibitionRatio()` | E/I balance ratio | `float` |
| `getTotalSpikeCount()` | Total spikes generated | `int` |
| `getDevelopmentalStage()` | Current developmental stage | `DevelopmentalStage` |
| `setDevelopmentalStage(stage)` | Set developmental stage | None |
| `logStatus()` | Print detailed brain status | None |

#### Memory Systems

| Method | Description | Returns |
|--------|-------------|---------|
| `getWorkingMemory()` | Access working memory system | `NeuralWorkingMemory` |
| `getEpisodicMemory()` | Access episodic memory system | `NeuralEpisodicMemory` |
| `getAssociativeMemory()` | Access associative memory system | `NeuralAssociativeMemory` |

#### Cognition Systems

| Method | Description | Returns |
|--------|-------------|---------|
| `getPlanner()` | Access neural planning system | `NeuralPlanner` |
| `getConceptFormation()` | Access concept formation system | `ConceptFormation` |
| `getAttention()` | Access attention system | `AttentionalSelection` |

#### Neuromodulation Systems

| Method | Description | Returns |
|--------|-------------|---------|
| `getDopamine()` | Access dopamine neuromodulation | `Dopamine` |
| `getCuriosity()` | Access curiosity system | `Curiosity` |
| `getNovelty()` | Access novelty detection | `Novelty` |
| `getPredictionErrorSignal()` | Access prediction error system | `PredictionError` |

#### Persistence

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `save(filepath)` | Save brain state to file | `filepath: str` | `bool` |
| `load(filepath)` | Load brain state from file | `filepath: str` | `bool` |
| `addRegion(name)` | Add a new neural region | `name: str` | `RegionId` |
| `getRegion(region_id)` | Get region by ID | `region_id: RegionId` | `NeuralRegion` |

### AgentBrain

Interface that connects NLM brain to world environment, handling sensory processing and motor decoding.

#### Creation and Initialization

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)
```

#### Main Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize agent with world | `world: SimpleWorld` | None |
| `processSensoryInput(percept)` | Process sensory percept | `percept: SensoryPercept` | None |
| `decodeMotorCommand()` | Decode motor command | None | `Action` |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward-based learning | `reward: float, predicted_reward: float` | None |
| `updateDevelopment(timestep)` | Update development system | `timestep: float` | None |

#### Subsystem Control

```python
# Enable learning and development
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Subsystem Status

| Method | Description | Returns |
|--------|-------------|---------|
| `getCuriosityLevel()` | Current curiosity level | `float` |
| `getNoveltyLevel()` | Current novelty level | `float` |
| `getPredictionError()` | Current prediction error | `float` |
| `getNeuromodulationLevel()` | Current neuromodulation level | `float` |
| `getDevelopmentalStage()` | Current developmental stage | `DevelopmentalStage` |
| `isRewardModulationEnabled()` | Check if reward modulation enabled | `bool` |
| `isStructuralPlasticityEnabled()` | Check if structural plasticity enabled | `bool` |

### SimpleWorld

Virtual 2D world for agent-environment interaction.

#### Creation and Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
```

#### Main Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Set world dimensions | `width: int, height: int, visionWidth: int, visionHeight: int` | None |
| `reset()` | Reset world to initial state | None | None |
| `setAgentStart(x, y)` | Set agent starting position | `x: float, y: float` | None |
| `update(timestep)` | Update world state | `timestep: float` | None |
| `applyMotorCommand(cmd, currentTime)` | Apply motor command | `cmd: MotorCommand, currentTime: float` | None |

#### State Access

| Method | Description | Returns |
|--------|-------------|---------|
| `getSensoryPercept()` | Get agent's sensory perception | `SensoryPercept` |
| `getAgentBody()` | Get agent body state | `AgentBody` |
| `getWidth()` | World width | `int` |
| `getHeight()` | World height | `int` |
| `getSimulationTime()` | Current simulation time | `float` |
| `getMaxEnergy()` | Maximum agent energy | `float` |
| `setMaxEnergy(value)` | Set maximum energy | `value: float` | None |

#### Object Management

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `addObject(obj)` | Add world object | `obj: WorldObject` | None |
| `removeObject(x, y)` | Remove object at position | `x: float, y: float` | None |
| `isValidPosition(x, y)` | Check if position is valid | `x: float, y: float` | `bool` |

### Config

Configuration system for NLM parameters.

#### Creation and Basic Usage

```python
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.enable", True)

# Check if key exists
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Get configuration summary
print(config.summary())
```

#### File Operations

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load configuration from JSON file | `filepath: str` | `bool` |
| `loadFromArgs(argc, argv)` | Load configuration from command line | `argc: int, argv: list` | `bool` |
| `saveToFile(filepath)` | Save configuration to JSON file | `filepath: str` | `bool` |
| `clear()` | Clear all configuration entries | None | None |

### SensoryInput Classes

Base class for sensory modalities.

#### Vision

Visual sensory input.

```python
vision = pynlm.Vision(width=8, height=8, channels=3)
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9])
```

#### Audio

Audio sensory input.

```python
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setData([0.0, 0.1, 0.2, 0.3, 0.4])
audio.setSampleRate(22050)
```

#### InternalSignals

Internal neural signals.

```python
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.7)
```

### Action

Motor action representation.

```python
# Create action from type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.8])

# Get action properties
print(action.getType())  # ActionType.Custom
print(action.getParameters())  # [0.5, 0.3, 0.8]
```

### WorldObject

Representation of objects in the world.

```python
# Create resource object
resource = pynlm.WorldObject(x=5.0, y=5.0, 
                           type=pynlm.WorldObjectType.Resource, 
                           value=1.0)

# Create wall object
wall = pynlm.WorldObject(x=10.0, y=10.0, 
                        type=pynlm.WorldObjectType.Wall, 
                        radius=0.5)
```

### AgentBody

Agent's physical state in the world.

```python
body = agent.getAgentBody()
print(f"Position: ({body.x}, {body.y})")
print(f"Velocity: ({body.velocityX}, {body.velocityY})")
print(f"Energy: {body.energy}")
print(f"Age: {body.age}")
```

## Enumerations

### NeuronType

- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Modulatory`: Modulatory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

### ActionType

- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look in current direction
- `LookUp`: Look upward
- `LookDown`: Look downward
- `Interact`: Interact with object
- `Eat`: Eat food
- `Drink`: Drink water
- `Rest`: Rest
- `Wait`: Wait
- `Custom`: Custom action with parameters

### DevelopmentalStage

- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period for learning
- `Maturation`: Maturation phase
- `Adult`: Adult stage
- `Aging`: Aging stage

### WorldObjectType

- `Empty`: Empty space
- `Resource`: Resource object
- `Hazard`: Hazardous object
- `Wall`: Wall object
- `Marker`: Marker object

## Advanced API Functions

### Configuration Helpers

```python
def createCustomConfig(neuron_count=1000, connection_probability=0.1,
                      stdp_enabled=True, hebbian_enabled=True):
    """Create a custom configuration with specified parameters."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", connection_probability)
    config.set("plasticity.stdp.enable", stdp_enabled)
    config.set("plasticity.hebbian.enable", hebbian_enabled)
    return config

def loadConfigFromJSON(filepath):
    """Load configuration from JSON file with error handling."""
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile(filepath):
        raise ValueError(f"Failed to load config from {filepath}")
    return config

def validateConfig(config):
    """Validate configuration and return list of issues."""
    issues = []
    if not config.has("brain.neuron_count"):
        issues.append("Missing brain.neuron_count")
    if not config.has("brain.connection_probability"):
        issues.append("Missing brain.connection_probability")
    return issues
```

### Simulation Helpers

```python
def runSimulation(brain, world, agent, num_steps=1000):
    """Run complete agent simulation with error handling."""
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        if percept is None:
            continue
            
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        if action is not None:
            world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'total_steps': num_steps,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount()
    }

def stepMultiple(brain, num_steps, timestep=0.001):
    """Efficiently run multiple simulation steps."""
    for step in range(num_steps):
        brain.step(step, step * timestep)

def createCustomBrain(neuron_count=500, regions=1, 
                     connection_probability=0.1, config=None):
    """Create a brain with custom parameters."""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", regions)
    config.set("brain.connection_probability", connection_probability)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
```

### Utility Functions

```python
def createScript():
    """Create an execution script for complex simulations."""
    script = {
        'steps': 0,
        'events': [],
        'checkpoints': [],
        'actions': []
    }
    return script

def runScript(script, brain, world, agent):
    """Execute a simulation script."""
    for event in script['events']:
        if event['type'] == 'step':
            for _ in range(event['count']):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(event['step'])
                agent.decodeMotorCommand()
                event['step'] += 1
        elif event['type'] == 'checkpoint':
            brain.save(event['filepath'])
    
    return script

def exportConfiguration(config, filepath):
    """Export configuration with detailed metadata."""
    export_data = {
        'config': config.summary(),
        'timestamp': datetime.now().isoformat(),
        'metadata': {
            'source': 'NLM Python API',
            'version': '6.0'
        }
    }
    
    import json
    with open(filepath, 'w') as f:
        json.dump(export_data, f, indent=2)
    
    return True
```

## Error Handling

The NLM Python API includes comprehensive error handling:

### Common Exceptions

- `RuntimeError`: General runtime errors
- `ValueError`: Invalid parameter values
- `FileNotFoundError`: File not found errors
- `IOError`: Input/output errors

### Error Handling Best Practices

```python
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Simulation loop
    for step in range(1000):
        brain.step(step)
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
    # Handle gracefully or rethrow
    
except ValueError as e:
    print(f"Invalid parameter: {e}")
    # Fix parameters and retry
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log and rethrow for debugging
    raise
```

## Performance Tips

### Efficient Simulation

1. **Batch processing:**
   ```python
   # Instead of individual steps
   for i in range(1000):
       brain.step(i)
   
   # Use batch processing
   stepMultiple(brain, 1000)
   ```

2. **Memory management:**
   ```python
   # Clear unused data to free memory
   if step % 100 == 0:
       brain.getWorkingMemory().clear()
   ```

3. **Configuration optimization:**
   ```python
   # Use minimal configuration for testing
   config = createCustomConfig(neuron_count=100, connection_probability=0.05)
   ```

### Large Scale Simulations

```python
# For large simulations, consider these optimizations
config = createCustomConfig(neuron_count=10000, connection_probability=0.05)
brain = pynlm.createBrain(config)
brain.initialize()

# Use efficient stepping
stepMultiple(brain, 10000)

# Monitor performance
import time
start_time = time.time()
stepMultiple(brain, 10000)
end_time = time.time()
print(f"Simulation time: {end_time - start_time:.2f} seconds")
```

## Migration Guide

### From v5.x to v6.0

1. **Brain creation:**
   ```python
   # Old way
   brain = pynlm.Brain(config)
   
   # New way
   brain = pynlm.createBrain(config)
   ```

2. **Direct instantiation:**
   ```python
   # Old way (no longer supported)
   brain = pynlm.Brain(config)
   
   # New way (factory functions)
   brain = pynlm.createBrain(config)
   ```

3. **Method changes:**
   - `brain.initialize()` - Still supported
   - `brain.step(step_number)` - Still supported
   - Added `brain.getWorkingMemory()`, `brain.getEpisodicMemory()`, etc.

## Examples

See the `examples/` directory for comprehensive examples of NLM usage, including:

- Basic brain simulation
- Agent-world interaction
- Learning and plasticity
- Memory systems usage
- Advanced configuration

## Additional Resources

- **Source code:** Available on GitHub
- **Documentation:** `README.md`, `HOW_TO_USE.md`, `easy_usage.md`
- **Community:** NLM discussion forum
- **Issues:** Bug tracker and feature requests

This reference provides a complete guide to using the NLM Python API. For more advanced usage, refer to the examples and tutorials in the `examples/` directory.