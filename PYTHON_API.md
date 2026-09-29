# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python bindings provide a high-level interface to the C++ neural simulation framework. This documentation provides comprehensive information about the Python API, including usage examples and best practices.

## Installation

```bash
pip install pynlm
```

Or build from source:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd python
pip install .
```

## Main Modules

The main modules available in the pynlm package are:

### 1. Core Brain Components

#### Brain Class
**File:** `pynlm.Brain`

Represents the central neural simulation brain with all its components.

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(currentStep, currentTime)` - Advance the simulation
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Generate motor output
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Example:**
```python
from pynlm import createDefaultConfig, createBrain, createSimpleWorld

# Create configuration
config = createDefaultConfig()

# Set parameters
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1f)
config.set("random_seed", 42)

# Create brain
brain = createBrain(config)
brain.initialize()
```

#### Config Class
**File:** `pynlm.Config`

Configuration management for the NLM system.

**Key Methods:**
- `loadFromFile(filepath)` - Load from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary

**Example:**
```python
from pynlm import Config

config = Config()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.set("stdp_ltp_weight", 0.01f)

print(config.summary())
```

### 2. Sensory Processing

#### Vision Class
**File:** `pynlm.Vision`

Visual sensory input with configurable dimensions.

**Key Methods:**
- `setData(data)` - Set visual data
- `getWidth()`, `getHeight()`, `getChannels()` - Get dimensions

**Example:**
```python
from pynlm import Vision

vision = Vision(width=16, height=16, channels=3)
data = [0.0] * 16 * 16 * 3  # Normalized RGB data
vision.setData(data)
```

#### Audio Class
**File:** `pynlm.Audio`

Audio sensory input for sound processing.

**Key Methods:**
- `setData(data)` - Set audio samples
- `setSampleRate(rate)` - Set sampling rate
- `getSampleRate()`, `getNumSamples()` - Get audio properties

**Example:**
```python
from pynlm import Audio

audio = Audio(sampleRate=44100, numSamples=1024)
samples = [0.0] * 1024
for i in range(len(samples)):
    samples[i] = 0.5 * math.sin(2 * math.pi * 440 * i / 44100)
audio.setData(samples)
audio.setSampleRate(44100)
```

#### InternalSignals Class
**File:** `pynlm.InternalSignals`

Internal physiological signals for interoception.

**Key Methods:**
- `addSignal(value)` - Add a signal value
- `clearSignals()` - Clear all signals

**Example:**
```python
from pynlm import InternalSignals

internal = InternalSignals()
for i in range(10):
    internal.addSignal(0.5 * math.sin(2 * math.pi * i / 10))
```

### 3. World Interaction

#### SimpleWorld Class
**File:** `pynlm.SimpleWorld`

2D simulation environment for agent interaction.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world size
- `reset()` - Reset world state
- `update(timestep)` - Update world dynamics
- `applyMotorCommand(cmd, currentTime)` - Apply agent action
- `getSensoryPercept()` - Get current sensory input
- `getAgentBody()` - Get agent physical state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position

**Example:**
```python
from pynlm import createSimpleWorld

world = createSimpleWorld()
world.configure(width=20.0, height=20.0, visionWidth=16, visionHeight=16)
world.setRandomSeed(42)

# Add some resources
from pynlm import WorldObject, WorldObjectType
resource = WorldObject(10.0, 10.0, WorldObjectType.Resource, 1.0, 0.5)
world.addObject(resource)
```

### 4. Agent Brain Interface

#### AgentBrain Class
**File:** `pynlm.AgentBrain`

Connects NLM brain to the world, handling sensory processing and motor output.

**Key Methods:**
- `initialize(world)` - Initialize with world environment
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command from brain activity
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get current developmental stage

**Complete Example:**
```python
from pynlm import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
)

# Create all components
config = createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

brain = createBrain(config)
world = createSimpleWorld()
world.configure(20.0, 20.0, 16, 16)

agent = createAgentBrain(brain)
agent.initialize(world)

# Enable key systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
```

### 5. Action and State Management

#### Action Class
**File:** `pynlm.Action`

Represents motor commands and their parameters.

**Key Methods:**
- `getType()`, `setType(type)` - Get/set action type
- `getParameters()`, `setParameters(params)` - Get/set action parameters
- `getName()` - Get human-readable action name
- `clone()` - Create a copy

**Example:**
```python
from pynlm import Action, ActionType

# Create action by type
action = Action(ActionType.MoveForward)
print(action.getName())  # "MoveForward"

# Create action with parameters
params = [0.5, 0.5]  # x, y movement
move_action = Action(ActionType.Custom, params)
action.setParameters(params)
```

#### ActionResult Class
**File:** `pynlm.ActionResult`

Result of applying a motor command to the world.

**Key Fields:**
- `reward` - Reward received
- `success` - Whether action succeeded
- `message` - Description of outcome

**Example:**
```python
from pynlm import ActionResult

result = ActionResult(reward=1.0, success=True, message="Resource collected")
print(f"Reward: {result.reward}")
print(f"Success: {result.success}")
```

### 6. State and Identification

#### NeuronId, SynapseId, RegionId, PopulationId Classes
**File:** `pynlm` (various)

Unique identifiers for neural components.

**Key Features:**
- Value storage
- Index calculation
- Equality comparison
- Hashing for dictionaries/sets
- String representation

**Example:**
```python
from pynlm import NeuronId, SynapseId, RegionId, PopulationId

neuron_id = NeuronId(123)
synapse_id = SynapseId(456)
region_id = RegionId(789)
population_id = PopulationId(999)

print(neuron_id.index())  # Get index for array access
```

#### AgentBody Class
**File:** `pynlm.AgentBody`

Physical state of the agent in the world.

**Key Fields:**
- `x`, `y` - Position
- `orientation` - Facing direction
- `velocityX`, `velocityY` - Linear velocity
- `angularVelocity` - Rotational velocity
- `energy`, `health` - Biological state
- `age` - Developmental age

**Example:**
```python
from pynlm import AgentBody

body = AgentBody()
body.x = 10.0
body.y = 10.0
body.orientation = 0.0
body.velocityX = 0.5
body.energy = 1.0
body.health = 1.0
body.age = 0.0
```

## Advanced Usage Patterns

### 1. Complete Simulation Loop

```python
import time
import math
from pynlm import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    ActionResult
)

def run_simulation(duration_seconds=60):
    # Setup
    config = createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("max_steps", int(duration_seconds * 10))  # 10 Hz
    
    brain = createBrain(config)
    world = createSimpleWorld()
    world.configure(20.0, 20.0, 16, 16)
    world.setRandomSeed(42)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    start_time = time.time()
    step = 0
    total_reward = 0.0
    
    while time.time() - start_time < duration_seconds and step < config.get("max_steps"):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Advance brain (10 neural steps per world step)
        for neural_step in range(10):
            brain.step(neural_step, step * 0.1)  # 100ms world timestep
        
        # Get motor command
        from pynlm import MotorCommand
        cmd = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(cmd, step * 0.1)
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        total_reward += result.reward
        step += 1
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}, Reward: {result.reward:.3f}, "
                  f"Total: {total_reward:.3f}, "
                  f"Neuromod: {agent.getNeuromodulationLevel():.3f}")
    
    # Statistics
    print(f"Simulation completed: {step} steps, "
          f"Total reward: {total_reward:.3f}")
    print(f"Final brain state: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    
    return brain, agent, world, step

# Run simulation
brain, agent, world, steps = run_simulation(30)  # 30 seconds
```

### 2. Checkpoint and Resume

```python
from pynlm import createBrain, createDefaultConfig

# Save brain state
config = createDefaultConfig()
config.set("neuron_count", 1000)
brain = createBrain(config)
brain.initialize()

# Run some steps
for step in range(1000):
    brain.step(step, step * 0.001)

# Save checkpoint
checkpoint_file = "my_brain_checkpoint.bin"
if brain.save(checkpoint_file):
    print(f"Checkpoint saved to {checkpoint_file}")
else:
    print("Failed to save checkpoint")

# Later, resume from checkpoint
brain2 = createBrain(config)
brain2.initialize()
if brain2.load(checkpoint_file):
    print("Checkpoint loaded successfully")
    print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
else:
    print("Failed to load checkpoint")
```

### 3. Multi-Agent Simulation

```python
from pynlm import createBrain, createSimpleWorld, createAgentBrain, createDefaultConfig

def create_agent(id, config):
    brain = createBrain(config)
    brain.set("random_seed", id * 1000 + 42)  # Different seed per agent
    brain.initialize()
    return brain

def run_multi_agent_simulation(num_agents=5, steps=500):
    config = createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 2)
    
    world = createSimpleWorld()
    world.configure(50.0, 50.0, 16, 16)
    
    agents = []
    for i in range(num_agents):
        brain = create_agent(i, config)
        agent = createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agents.append(agent)
        
        # Place agents at different locations
        body = world.getAgentBody()
        body.x = 5 + i * 8
        body.y = 5
    
    # Simulation loop
    for step in range(steps):
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            for neural_step in range(5):
                agent.getBrain().step(neural_step, step * 0.1)
            
            cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(cmd, step * 0.1)
            agent.applyRewardModulation(0.1, 0.0)  # Fixed small reward
            agent.updateDevelopment(0.1)
    
    return agents

# Run multi-agent simulation
agents = run_multi_agent_simulation(3, 200)
print(f"Simulation completed with {len(agents)} agents")
```

## Error Handling

### Common Exceptions

The Python bindings can raise the following exceptions:

1. `RuntimeError` - General runtime errors from C++ code
2. `ValueError` - Invalid parameter values
3. `TypeError` - Wrong types in function calls

### Error Handling Example

```python
from pynlm import createBrain, createDefaultConfig

config = createDefaultConfig()
brain = createBrain(config)

try:
    brain.initialize()
    print("Brain initialized successfully")
except RuntimeError as e:
    print(f"Error initializing brain: {e}")
except ValueError as e:
    print(f"Invalid configuration: {e}")
```

## Performance Tips

1. **Reuse Objects**: Create brain and agent objects once and reuse them
2. **Batch Operations**: Process multiple sensory inputs in bulk when possible
3. **Memory Management**: Monitor memory usage for long-running simulations
4. **Configuration Caching**: Reuse configuration objects across brain instances
5. **Parallel Processing**: Consider using multiple threads for independent simulations

## Troubleshooting

### Common Issues and Solutions

1. **Import Error**: If `pynlm` is not found:
   - Install with `pip install pynlm`
   - Build from source if using custom version

2. **Missing Dependencies**: Ensure pybind11 and Python development headers are installed

3. **Simulation Slow**: Increase simulation timestep or reduce neuron count

4. **Memory Issues**: Monitor memory usage and save checkpoints regularly

### Getting Help

- Check the NLM documentation at https://nlm.readthedocs.io
- GitHub issues at https://github.com/nlm-project/nlm/issues
- Community forums for user discussions

## Version Information

This API documentation is for the NLM Python bindings. Please check the version of pynlm you're using:

```python
import pynlm
print(f"pynlm version: {pynlm.__version__}")
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.
