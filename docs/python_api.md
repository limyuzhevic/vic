# NLM Python API Reference

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates neural systems capable of developmental learning through interaction with an environment. This document provides comprehensive API reference for the NLM Python bindings (`pynlm`).

## Introduction

The Python bindings (`pynlm`) provide a high-level interface to the NLM C++ neural simulation framework. They enable users to create, configure, and run neural simulations without needing to understand the underlying C++ implementation.

## Installation

```bash
# Install from source (recommended for development)
pip install .

# Or install in development mode
pip install -e .

# Install with optional dependencies for testing
pip install .[dev]
```

## Basic Usage Examples

### Example 1: Simple Neural Simulation

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

# Setup agent environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete simulation loop
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Class and Function Reference

### Classes

#### Brain
The central neural simulation brain class that coordinates neural computation, memory, prediction, and development.

**Constructor:**
```python
brain = pynlm.createBrain(config)
```

**Key Methods:**

| Method | Parameters | Return Type | Description |
|--------|------------|-------------|-------------|
| `brain.initialize()` | - | None | Initialize brain with configuration |
| `brain.step(step)` | `step: SimulationStep` | None | Perform one simulation step |
| `brain.step(step, time)` | `step: SimulationStep, time: Timestamp` | None | Perform step with timestamp |
| `brain.receiveSensoryInput(input)` | `input: SensoryInput` | None | Inject sensory input into brain |
| `brain.produceAction()` | - | Action | Get motor action from neural activity |
| `brain.reset()` | - | None | Reset brain to initial state |
| `brain.save(filepath)` | `filepath: str` | None | Save brain state to file |
| `brain.load(filepath)` | `filepath: str` | None | Load brain state from file |
| `brain.addRegion(name)` | `name: str = ""` | RegionId | Add a new neural region |
| `brain.getRegion(id)` | `id: RegionId` | Region | Get region by ID |

**Key Properties/Statistics:**

| Method | Return Type | Description |
|--------|-------------|-------------|
| `brain.getTotalNeuronCount()` | int | Total neurons across all regions |
| `brain.getTotalSynapseCount()` | int | Total synapses across all regions |
| `brain.getFiringNeuronCount()` | int | Currently firing neurons |
| `brain.getAverageFiringRate()` | float | Average firing rate across all neurons |
| `brain.getExcitationInhibitionRatio()` | float | E/I balance ratio |
| `brain.getTotalSpikeCount()` | int | Total spike count |
| `brain.getDevelopmentalStage()` | DevelopmentalStage | Current developmental stage |
| `brain.getConfig()` | Config | Get configuration object |

**Exceptions:**
- `RuntimeError`: Thrown for invalid operations or configuration errors

#### Config
Configuration management system for NLM parameters.

**Constructor:**
```python
config = pynlm.createDefaultConfig()
```

**Key Methods:**

| Method | Parameters | Return Type | Description |
|--------|------------|-------------|-------------|
| `config.loadFromFile(filepath)` | `filepath: str` | None | Load from JSON file |
| `config.loadFromArgs(argc, argv)` | `argc: int, argv: List[str]` | bool | Load from command line |
| `config.saveToFile(filepath)` | `filepath: str` | None | Save to JSON file |
| `config.has(key)` | `key: str` | bool | Check if key exists |
| `config.getKeys()` | - | List[str] | Get all configuration keys |
| `config.set(key, value)` | `key: str, value: Any` | None | Set configuration value |
| `config.clear()` | - | None | Clear all configuration |
| `config.summary()` | - | str | Get configuration summary |

#### AgentBrain
Interface connecting NLM brain to environment, handling sensory input, motor output, and learning.

**Constructor:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Key Methods:**

| Method | Parameters | Return Type | Description |
|--------|------------|-------------|-------------|
| `agent.initialize(world)` | `world: SimpleWorld` | None | Initialize with world |
| `agent.processSensoryInput(percept)` | `percept: SensoryPercept` | None | Process sensory input |
| `agent.decodeMotorCommand()` | - | Action | Decode motor command from neural activity |
| `agent.applyRewardModulation(reward, predicted)` | `reward: float, predicted: float` | None | Apply reward-based neuromodulation |
| `agent.updateDevelopment(timestep)` | `timestep: float` | None | Update development system |
| `agent.reset()` | - | None | Reset for new episode |
| `agent.getBrain()` | - | Brain | Get underlying brain |

**Key Statistics:**

| Method | Return Type | Description |
|--------|-------------|-------------|
| `agent.getCuriosityLevel()` | float | Current curiosity level |
| `agent.getNoveltyLevel()` | float | Current novelty level |
| `agent.getPredictionError()` | float | Current prediction error |
| `agent.getNeuromodulationLevel()` | float | Current neuromodulation level |
| `agent.getDevelopmentalStage()` | DevelopmentalStage | Current developmental stage |

**Enable/Disable Methods:**

| Method | Parameters | Description |
|--------|------------|-------------|
| `agent.enableRewardModulation(enable)` | `enable: bool` | Enable/disable reward learning |
| `agent.enableStructuralPlasticity(enable)` | `enable: bool` | Enable/disable structural plasticity |
| `agent.enableDevelopment(enable)` | `enable: bool` | Enable/disable development |
| `agent.enableCuriosity(enable)` | `enable: bool` | Enable/disable curiosity-driven exploration |

**Check Methods:**

| Method | Return Type | Description |
|--------|-------------|-------------|
| `agent.isRewardModulationEnabled()` | bool | Check if reward modulation enabled |
| `agent.isStructuralPlasticityEnabled()` | bool | Check if structural plasticity enabled |
| `agent.isDevelopmentEnabled()` | bool | Check if development enabled |
| `agent.isCuriosityEnabled()` | bool | Check if curiosity enabled |

#### SimpleWorld
2D environment simulation for NLM agent interaction.

**Constructor:**
```python
world = pynlm.createSimpleWorld()
```

**Key Methods:**

| Method | Parameters | Return Type | Description |
|--------|------------|-------------|-------------|
| `world.configure(width, height, visionWidth, visionHeight)` | `width: int, height: int, visionWidth: int, visionHeight: int` | None | Configure world dimensions |
| `world.reset()` | - | None | Reset world to initial state |
| `world.setAgentStart(x, y)` | `x: float, y: float` | None | Set agent starting position |
| `world.update(timestep)` | `timestep: float` | None | Update world state |
| `world.applyMotorCommand(cmd, currentTime)` | `cmd: Action, currentTime: float` | None | Apply motor command |
| `world.getSensoryPercept()` | - | SensoryPercept | Get sensory percept from world |
| `world.getAgentBody()` | - | AgentBody | Get agent body state |
| `world.addObject(obj)` | `obj: WorldObject` | None | Add object to world |
| `world.removeObject(x, y)` | `x: float, y: float` | None | Remove object at position |
| `world.isValidPosition(x, y)` | `x: float, y: float` | bool | Check if position is valid |

**Key Properties:**

| Method | Return Type | Description |
|--------|-------------|-------------|
| `world.getWidth()` | int | World width |
| `world.getHeight()` | int | World height |
| `world.getSimulationTime()` | float | Current simulation time |
| `world.getMaxEnergy()` | float | Maximum energy |
| `world.setMaxEnergy(e)` | `e: float` | Set maximum energy |

#### SensoryInput Classes
Base class for all sensory inputs. Available implementations:

##### Vision
Visual sensory input (RGB or grayscale).

**Constructor:**
```python
vision = pynlm.Vision(width, height, channels=3)
```

**Methods:**
- `vision.setData(data)`: Set visual data as list of floats
- `vision.getWidth()`: Get width
- `vision.getHeight()`: Get height
- `vision.getChannels()`: Get number of channels

##### Audio
Audio sensory input.

**Constructor:**
```python
audio = pynlm.Audio(sampleRate, numSamples)
```

**Methods:**
- `audio.setData(data)`: Set audio data as list of floats
- `audio.setSampleRate(rate)`: Set sample rate
- `audio.getSampleRate()`: Get sample rate
- `audio.getNumSamples()`: Get number of samples

##### InternalSignals
Internal state signals (e.g., reward, prediction, novelty).

**Methods:**
- `addSignal(value)`: Add signal value
- `clearSignals()`: Clear all signals

#### Action
Motor action representation.

**Constructor:**
```python
action = pynlm.Action(action_type, parameters=None)
action = pynlm.Action(ActionType.MoveForward)
action = pynlm.Action(ActionType.Custom, [1.0, 2.0, 0.5])
```

**Methods:**
- `action.getType()`: Get action type
- `action.setType(type)`: Set action type
- `action.getParameters()`: Get action parameters
- `action.setParameters(params)`: Set action parameters
- `action.getName()`: Get human-readable action name
- `action.clone()`: Create copy

#### SensoryPercept
Container for all sensory inputs from the world.

**Methods:**
- `getVision()`: Get vision data
- `setVision(vision)`: Set vision data
- `getTouch()`: Get touch data
- `setTouch(touch)`: Set touch data
- `getInternal()`: Get internal signals
- `setInternal(internal)`: Set internal signals
- `getProprioception()`: Get proprioception
- `setProprioception(proprioception)`: Set proprioception
- `getAudio()`: Get audio data
- `setAudio(audio)`: Set audio data
- `getAllSignals()`: Get all sensory signals
- `getTimestamp()`: Get timestamp
- `setTimestamp(timestamp)`: Set timestamp

#### WorldObject
Physical object in the world.

**Constructor:**
```python
obj = pynlm.WorldObject(x, y, world_type, value=0.0, radius=0.5)
```

**Properties:**
- `x`, `y`: Position coordinates
- `type`: World object type
- `value`: Object value (e.g., energy, resource amount)
- `radius`: Interaction radius
- `active`: Object activity state

#### AgentBody
Agent state information including position, velocity, and physiological state.

**Properties:**
- `x`, `y`: Position
- `orientation`: Direction (radians)
- `velocityX`, `velocityY`: Linear velocity
- `angularVelocity`: Angular velocity
- `energy`: Current energy level
- `health`: Health status
- `age`: Age in time steps
- `isMoving`: Movement state
- `isTurning`: Turning state
- `lastActionTime`: Time of last action

#### ActionResult
Result of applying a motor command to the world.

**Constructor:**
```python
result = pynlm.ActionResult(reward, success, message="")
```

**Properties:**
- `reward`: Reward value
- `success`: Success status
- `message`: Descriptive message

### Global Functions

#### Module-Level Functions

| Function | Parameters | Return Type | Description |
|----------|------------|-------------|-------------|
| `pynlm.createDefaultConfig()` | - | Config | Create default configuration |
| `pynlm.createBrain(config)` | `config: Config` | Brain | Create brain with configuration |
| `pynlm.createSimpleWorld()` | - | SimpleWorld | Create simple 2D world |
| `pynlm.createAgentBrain(brain)` | `brain: Brain` | AgentBrain | Create agent brain interface |

#### Enumeration Types

##### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

##### SynapseType
```python
pynlm.SynapseType.Excitatory
pynlm.SynapseType.Inhibitory
pynlm.SynapseType.Modulatory
pynlm.SynapseType.Electrical
pynlm.SynapseType.GapJunction
```

##### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

##### FiringState
```python
pynlm.FiringState.Resting
pynlm.FiringState.Active
pynlm.FiringState.Refractory
pynlm.FiringState.Inhibited
```

##### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

##### MotorCommand
```python
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
```

##### WorldObjectType
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Error Handling

NLM uses Python's `RuntimeError` exception for error conditions. Common error conditions include:

### Configuration Errors
```python
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1)  # Invalid value
    brain = pynlm.createBrain(config)
except RuntimeError as e:
    print(f"Configuration error: {e}")
```

### State Errors
```python
try:
    brain.step(0)  # Must call initialize() first
except RuntimeError as e:
    print(f"State error: {e}")
```

### File Errors
```python
try:
    brain.save("invalid/path/checkpoint.bin")
except RuntimeError as e:
    print(f"File error: {e}")
```

## Type Hints

The Python bindings include type hints for better IDE support and code clarity. All parameters and return values are properly typed according to the C++ types.

## Performance Considerations

### Memory Management
- Brain objects are designed to be efficient but may require significant memory for large networks
- Use `save()` and `load()` to reduce memory footprint during long simulations
- Consider checkpointing at regular intervals

### Computational Complexity
- Neural dynamics scale approximately as O(N^2) for dense connectivity
- Use `brain.step()` with increasing time steps for efficiency
- Enable/disable learning systems based on experimental needs

### Thread Safety
- NLM is not thread-safe
- Ensure only one simulation loop runs per brain instance
- World updates should be sequential

### Memory Usage Estimates
```python
# For typical configurations
brain = pynlm.createBrain(config)
# Memory usage depends on:
# - Neuron count (config.brain.neuron_count)
# - Synapse density (config.brain.synapse_density)
# - Number of regions
# Example: 1000 neurons with 0.1 density ≈ 100 synapses
```

## Configuration Examples

### Minimal Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### High-Performance Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)
config.set("brain.v_thresh", -40.0)
config.set("brain.tau_mem", 10.0)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Developmental Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("development.critical_period_duration", 100.0)
config.set("development.synaptogenesis_rate", 0.01)
config.set("development.pruning_rate", 0.005)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Advanced Patterns

### Multi-Agent Coordination
```python
import pynlm

def create_multi_agent_team(num_agents, world_config):
    brains = []
    agents = []
    
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000 + i * 500)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
        brains.append(brain)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    
    return brains, agents, world

# Initialize all agents
agents = []
for agent in created_agents:
    agent.initialize(shared_world)
```

### Memory-Efficient Training
```python
import pynlm

def run_with_checkpointing(brain, world, agent, num_steps, checkpoint_interval=1000):
    for step in range(num_steps):
        # Run simulation
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Checkpoint periodically
        if step % checkpoint_interval == 0 and step > 0:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            brain.save(checkpoint_file)
            print(f"Checkpoint saved: {checkpoint_file}")
    
    return brain

# Resume from checkpoint
def resume_from_checkpoint(brain, checkpoint_file):
    try:
        brain.load(checkpoint_file)
        print(f"Successfully loaded from {checkpoint_file}")
        return True
    except RuntimeError as e:
        print(f"Failed to load checkpoint: {e}")
        return False
```

### Developmental Tracking
```python
import pynlm

def track_development(brain, agent, num_steps):
    development_log = []
    
    for step in range(num_steps):
        # Update and record development
        if step % 100 == 0:
            dev_stage = brain.getDevelopmentalStage()
            curiosity = agent.getCuriosityLevel()
            prediction_error = agent.getPredictionError()
            firing_rate = brain.getAverageFiringRate()
            
            development_log.append({
                'step': step,
                'stage': dev_stage.value if hasattr(dev_stage, 'value') else str(dev_stage),
                'curiosity': curiosity,
                'prediction_error': prediction_error,
                'firing_rate': firing_rate
            })
    
    return development_log
```

## Performance Benchmarking

See `docs/benchmarking.md` for detailed performance analysis and optimization guidelines.

## Installation Troubleshooting

See `docs/installing.md` for installation issues and solutions.

## Common Problems and Solutions

See `docs/troubleshooting.md` for common issues and their solutions.

## Development

### Building Python Bindings

```bash
# Build from source
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install -e .
```

### Running Tests

```bash
cd /path/to/nlm
python -m pytest tests/ -v
```

### Documentation Generation

This API documentation is generated from the pybind11 bindings. For advanced usage examples, see `docs/python_examples.md`.
