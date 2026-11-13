# NLM Python API Reference Guide

Complete reference for using the NLM (Neural Learning Machine) Python bindings.

## Overview

The `pynlm` module provides Python bindings for the NLM C++ neural simulation framework. It allows you to create virtual brains, simulate them in environments, and study learning and cognition.

## Installation

```bash
pip install pynlm
```

## Error Classes

The Python bindings define several specific exception classes for different types of errors:

- **`nlm.RuntimeError`**: General runtime errors
- **`nlm.ConfigurationError`**: Configuration-related errors (invalid values, file not found, etc.)
- **`nlm.SimulationError`**: Simulation errors (neural system failures, invalid operations)
- **`nlm.WorldError`**: World/environment errors
- **`nlm.SensoryError`**: Sensory input processing errors
- **`nlm.MotorError`**: Motor output errors

Example error handling:

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    # ... use brain ...
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
```

## Core Classes

### Config

The `Config` class manages brain configuration parameters.

**Methods:**

- `Config()` - Create a new empty configuration
- `loadFromFile(filepath)` - Load configuration from a JSON/YAML file
- `loadFromArgs(args)` - Load configuration from command line arguments (list of strings)
- `saveToFile(filepath)` - Save configuration to a JSON/YAML file
- `has(key)` - Check if a configuration key exists
- `getOr(key, default_value)` - Get a value with a default if not found
- `set(key, value, source=ConfigSource.Runtime)` - Set a configuration value
- `getKeys()` - Get all configuration keys as a list
- `clear()` - Clear all configuration entries
- `validate()` - Validate all configuration values
- `toPythonDict()` - Convert configuration to Python dictionary

**Key Types:**

- `neuron_count` (int): Number of neurons in the brain
- `region_count` (int): Number of brain regions
- `connection_probability` (float): Probability of synaptic connections (0.0-1.0)
- `simulation_timestep` (float): Time step for simulation (seconds)
- `random_seed` (int64): Random seed for reproducible experiments

**Example:**

```python
import pynlm

# Create default config
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.15)
config.set("random_seed", 123)

# Validate configuration
if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration has errors!")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Check configuration values
print(f"Neuron count: {config2.getOr('neuron_count', 1000)}")
print(f"Has region_count: {config2.has('region_count')}")
```

### Brain

The `Brain` class represents a spiking neural network.

**Methods:**

- `Brain(config)` - Create a new brain with configuration
- `initialize()` - Initialize the brain with neurons and synapses
- `step(step_number)` - Perform one simulation step
- `step(step_number, time)` - Perform one simulation step with timestamp
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add a new neural region
- `getTotalNeuronCount()` - Get total number of neurons
- `getTotalSynapseCount()` - Get total number of synapses
- `getActiveNeuronCount()` - Get count of currently active neurons
- `getFiringNeuronCount()` - Get count of currently firing neurons
- `getTotalSpikeCount()` - Get total spike count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get excitation/inhibition balance
- `getSpikeSystem()` - Get the spike processing system
- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system
- `getPredictionSystem()` - Get prediction system
- `getNeuromodulationLevel()` - Get current neuromodulation level
- `logStatus()` - Log brain status information

**Example:**

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Check statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Add a region
brain.addRegion("SensoryRegion")

# Save and load brain state
brain.save("my_brain.nml")
brain2 = pynlm.createBrain(config)
brain2.load("my_brain.nml")
```

### SimpleWorld

The `SimpleWorld` class represents a 2D simulation environment.

**Methods:**

- `SimpleWorld()` - Create a new world
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `update(time_step)` - Update world physics
- `setAgentStart(x, y)` - Set agent starting position
- `applyMotorCommand(command, time)` - Apply motor command to world
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(object)` - Add object to world
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getMaxEnergy()` - Get maximum energy
- `getEnergyDecayRate()` - Get energy decay rate
- `getSimulationTime()` - Get current simulation time
- `setRandomSeed(seed)` - Set random seed
- `getRandomSeed()` - Get random seed

**Example:**

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20.0, height=20.0, vision_width=8, vision_height=8)
world.setRandomSeed(42)
world.reset()

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Check agent position
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")

# Update world
world.update(0.1)

# Get sensory percept
percept = world.getSensoryPercept()
print(f"Vision size: {percept.getVisionWidth()} x {percept.getVisionHeight()}")
print(f"Vision range: {[f for f in percept.getVision()[:5]]}...")
```

### AgentBrain

The `AgentBrain` class connects a brain to a world and handles sensory processing and motor output.

**Methods:**

- `AgentBrain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain activity into motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `updateDevelopment(time_step)` - Update development system
- `getNeuromodulationLevel()` - Get current neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `getDevelopmentalStage()` - Get current developmental stage
- `reset()` - Reset agent for new episode
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity
- `isRewardModulationEnabled()` - Check if reward modulation is enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity is enabled
- `isDevelopmentEnabled()` - Check if development is enabled
- `isCuriosityEnabled()` - Check if curiosity is enabled

**Example:**

```python
import pynlm

# Create brain, world, and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20.0, 20.0, 16, 16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from results (example)
    agent.applyRewardModulation(world.getAgentBody().energy, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print status periodically
    if step % 20 == 0:
        print(f"Step {step}: "
              f"reward={world.getAgentBody().energy}, "
              f"action={action}, "
              f"neuromod={agent.getNeuromodulationLevel():.3f}, "
              f"curiosity={agent.getCuriosityLevel():.3f}")

print(f"Final developmental stage: {agent.getDevelopmentalStage()}")
```

### Action

The `Action` class represents a motor action that can be performed in the world.

**Methods:**

- `Action()` - Create an empty action
- `Action(type)` - Create action with type
- `Action(type, parameters)` - Create action with type and parameters
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params)` - Set action parameters
- `getName()` - Get action name
- `clone()` - Create a copy of the action

**Example:**

```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Interact, [0.5, 0.5])
action3 = pynlm.Action(pynlm.ActionType.Look, [0.3])

# Use actions
print(f"Action 1 type: {action1.getType()}")
print(f"Action 1 name: {action1.getName()}")

# Clone action
action_copy = action1.clone()
```

### SensoryInput Classes

Various sensory input types are available:

#### Vision
- `Vision(width, height, channels=3)` - Visual sensory input
- Methods: `setData(data)`, `getWidth()`, `getHeight()`, `getChannels()`

#### Audio
- `Audio(sampleRate, numSamples)` - Audio sensory input
- Methods: `setData(data)`, `setSampleRate(rate)`, `getSampleRate()`, `getNumSamples()`

#### InternalSignals
- `InternalSignals()` - Internal state signals
- Methods: `addSignal(value)`, `clearSignals()`

### Module Functions

#### createDefaultConfig()
- Returns: `Config`
- Creates a new default configuration

#### createBrain(config)
- Parameters: `config` (Config)
- Returns: `Brain`
- Creates a new brain with the given configuration

#### createSimpleWorld()
- Returns: `SimpleWorld`
- Creates a new simple world

#### createAgentBrain(brain)
- Parameters: `brain` (Brain)
- Returns: `AgentBrain`
- Creates an agent brain interface

## Advanced Usage

### Configuration with JSON/YAML

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Modify values
config.set("neuron_count", 1500)
config.set("region_count", 2)
config.set("connection_probability", 0.12)

# Save as JSON
config.saveToFile("config.json")

# Load from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("config.json")

# Convert to Python dict
python_dict = config.toPythonDict()
print(f"Configuration: {python_dict}")
```

### Brain State Management

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps
for i in range(50):
    brain.step(i)

# Save brain state
brain.save("brain_state.nml")

# Create new brain and load state
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.load("brain_state.nml")

# Brain2 now has the same state as brain had after 50 steps
```

### Working with Metrics

```python
import pynlm

# Create complete simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20.0, 20.0, 16, 16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Run simulation and collect metrics
total_reward = 0
steps_per_episode = 100

for step in range(steps_per_episode):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, 0.0)
    
    total_reward += result.reward

# Get brain statistics
print(f"Simulation completed!")
print(f"Total reward: {total_reward:.2f}")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

### Error Handling Best Practices

```python
import pynlm

def safe_simulation():
    try:
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10.0, 10.0, 8, 8)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        return None
    except pynlm.SimulationError as e:
        print(f"Simulation error: {e}")
        return None
    except pynlm.WorldError as e:
        print(f"World error: {e}")
        return None
    except pynlm.SensoryError as e:
        print(f"Sensory error: {e}")
        return None
    except pynlm.MotorError as e:
        print(f"Motor error: {e}")
        return None
    except pynlm.RuntimeError as e:
        print(f"Runtime error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None
    
    print("Simulation completed successfully!")
    return brain

# Run safe simulation
simulation_result = safe_simulation()
if simulation_result:
    print(f"Brain has {simulation_result.getTotalNeuronCount()} neurons")
```

## Performance Tips

### For Large Simulations

1. **Reuse objects**: Create brain/world objects and reset them rather than recreating
2. **Batch operations**: Use checkpoint/restart functionality for long runs
3. **Monitor memory**: Use `getWorkingMemory()` and `getEpisodicMemory()` to track memory usage
4. **Configure appropriately**: Use appropriate neuron counts and connection probabilities

### Memory Management

```python
import pynlm

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Active traces: {working_memory.getActiveTraces()}")
    print(f"Memory capacity: {working_memory.getCapacity()}")

# Monitor episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Stored episodes: {episodic_memory.getEpisodeCount()}")
    print(f"Memory duration: {episodic_memory.getDuration()} steps")

# Clean up when done
# Brain will be automatically cleaned up when it goes out of scope
```

### Development Tips

1. **Enable features gradually**: Start with basic functionality, then add neuromodulation, plasticity, etc.
2. **Use configuration**: Test different configurations before running full simulations
3. **Monitor development**: Track developmental stage and neuromodulation levels
4. **Use verbose output**: Enable verbose mode for debugging

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have enough input to fire

**"The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent has energy

**"Everything is 0"
- Brains need time to "warm up" - try more steps
- Ensure neurons have appropriate input to fire
- Check configuration values are reasonable

**"Memory usage is too high"
- Reduce neuron count
- Use checkpointing for long simulations
- Monitor working memory usage

**"Configuration validation fails"
- Check invalid values (negative numbers, probabilities outside 0-1)
- Verify required configuration keys are present
- Use `config.validate()` to see specific errors

## Next Steps

When you're comfortable with the basics:

1. Read `HOW_TO_USE.md` for more detailed technical documentation
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Check out the demo executables (`nlm_phase3_demo`, `nlm_phase4_demo`, etc.)
5. Study the source code for advanced usage patterns

The NLM system is production-ready with comprehensive functionality for creating and simulating brain-like systems!
