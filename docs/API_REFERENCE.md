# NLM Python API Reference

This document provides a comprehensive reference for the NLM (Neural Learning Machine) Python API.

## Overview

NLM is a brain-inspired spiking neural network simulator that enables the creation of virtual brains capable of learning, memory, and decision-making. The Python API provides access to the full C++ NLM framework through pybind11 bindings.

## Quick Start

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create an agent interface
agent = pynlm.createAgentBrain(brain)

# Create a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Core Classes

### Config

Configuration management for NLM parameters.

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
learning_rate = config.get("plasticity.stdp.learning_rate")

# Check if key exists
if config.has("brain.synapse_density"):
    density = config.get("brain.synapse_density")

# Get all configuration keys
keys = config.getKeys()

# Save to file
config.saveToFile("my_config.json")

# Load from file
config.loadFromFile("my_config.json")

# Configuration summary
summary = config.summary()
```

**Available Configuration Keys:**

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

### Brain

The central neural simulation component.

```python
# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation control
for step in range(1000):
    brain.step(step)  # Simple stepping
    brain.step(step, step * 0.001)  # With timestamp

# Sensory input injection
from pynlm import Vision
sensory_input = Vision()
sensory_input.setData([0.1, 0.2, 0.3, 0.4, 0.5])
brain.receiveSensoryInput(sensory_input)

# Motor action production
action = brain.produceAction()

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
brain.reset()

# Statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")

# Development
from pynlm import DevelopmentalStage
brain.getDevelopmentalStage()  # Get current stage
brain.setDevelopmentalStage(DevelopmentalStage.Adult)  # Set stage

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### AgentBrain

Interface connecting NLM brain to the external world.

```python
# Create agent
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()

# Apply neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(0.1)

# Get status information
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")

# Subsystem control
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Check status
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")
```

### SimpleWorld

2D simulation environment for agent interaction.

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(5.0, 5.0)

# Update world
world.update(timestep=0.1)

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")

# Apply motor commands
from pynlm import MotorCommand
world.applyMotorCommand(MotorCommand.MoveForward, world.getSimulationTime())

# World object management
from pynlm import WorldObject, WorldObjectType
resource = WorldObject(10.0, 10.0, WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Check valid positions
is_valid = world.isValidPosition(5.0, 5.0)
```

### SensoryInput Classes

**Vision:** Visual sensory input
```python
vision = Vision()
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5])
print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
```

**InternalSignals:** Internal state signals
```python
internal = SensoryPercept()
internal.setInternal([0.1, 0.2, 0.3, 0.4])
```

### Action

Motor command representation
```python
# Create actions
action = Action(ActionType.MoveForward)
action.setParameters([1.0, 0.5])  # speed, direction

print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")
```

## Enumerations

### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons  
- `Modulatory` - Neuromodulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look in current direction
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact with object
- `Eat` - Eat consumable
- `Drink` - Drink liquid
- `Rest` - Rest/stop moving
- `Wait` - Wait for next step
- `Custom` - Custom action type

### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact with object
- `Wait` - Wait/ do nothing

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazardous object
- `Wall` - Wall/obstacle
- `Marker` - Marker object

### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical learning period
- `Maturation` - Maturation phase
- `Adult` - Adult stage
- `Aging` - Aging stage

### FiringState
- `Resting` - Neuron at rest
- `Active` - Neuron firing
- `Refractory` - Neuron in refractory period
- `Inhibited` - Neuron inhibited

## Advanced Usage Patterns

### Pattern 1: Agent with Learning Loop

```python
def run_learning_episode(agent, world, num_steps):
    """Run a complete learning episode"""
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get brain response
        agent.decodeMotorCommand()
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward (based on action success)
        reward = 0.0
        if action.getType() == ActionType.Interact:
            reward = 1.0  # Successful interaction
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Prediction Error={agent.getPredictionError():.3f}")
    
    return agent
```

### Pattern 2: Configuration Management

```python
def create_experiment_config(neuron_count, learning_rate, enable_curiosity):
    """Create configuration for specific experiment"""
    
    config = pynlm.createDefaultConfig()
    
    # Set experiment-specific parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("neuromod.curiosity.enable", enable_curiosity)
    
    # Set developmental parameters
    config.set("development.plasticity_rate", 0.1 if enable_curiosity else 0.01)
    config.set("development.critical_period", 100)
    
    return config
```

### Pattern 3: Multi-Agent System

```python
class AgentManager:
    """Manage multiple agent instances"""
    
    def __init__(self):
        self.agents = []
        self.worlds = []
    
    def create_agent(self, config=None):
        """Create new agent"""
        if config is None:
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        self.agents.append(agent)
        
        return agent
    
    def create_shared_world(self):
        """Create world shared by all agents"""
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        self.worlds.append(world)
        return world
    
    def run_parallel_simulation(self, num_steps):
        """Run simulation with multiple agents"""
        
        for world in self.worlds:
            world.reset()
            world.setAgentStart(25.0, 25.0)
        
        for step in range(num_steps):
            for i, (agent, world) in enumerate(zip(self.agents, self.worlds)):
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process with agent
                agent.processSensoryInput(percept)
                
                # Get brain response
                agent.decodeMotorCommand()
                
                # Execute action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward
                reward = calculate_reward(world, action, i)
                agent.applyRewardModulation(reward, agent.getPredictionError())
    
    def get_system_stats(self):
        """Get statistics across all agents"""
        stats = {}
        
        for i, agent in enumerate(self.agents):
            brain = agent.getBrain()
            stats[f"agent_{i}"] = {
                "neurons": brain.getTotalNeuronCount(),
                "spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "prediction_error": agent.getPredictionError()
            }
        
        return stats
```

## Error Handling

The NLM Python API raises standard Python exceptions:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # ... use brain
except RuntimeError as e:
    print(f"NLM runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

Common error conditions:
- Invalid configuration values
- Missing required initialization steps
- File I/O errors
- Parameter out of valid ranges

## Performance Tips

1. **Reuse configurations**: Create configuration objects once and reuse them
2. **Minimize object creation**: Create brain, agent, and world objects once
3. **Batch processing**: Process multiple sensory inputs before running brain steps
4. **Checkpoint frequently**: Save brain state periodically for long simulations
5. **Monitor performance**: Track spike counts and firing rates for performance analysis

## Migration Guide

If upgrading from an older version:

### Version 0.1.0 → 0.2.0
- `ActionType.LookLeft` and `ActionType.LookRight` added
- `MotorCommand.LookLeft` and `MotorCommand.LookRight` added
- Enhanced `AgentBrain` with curiosity and development support
- Improved error handling and documentation

### Version 0.2.0 → 0.3.0
- New configuration options for fine-tuning
- Enhanced neuromodulation system with improved dopamine dynamics
- Better memory management and resource efficiency
- Comprehensive unit test suite added

## Further Reading

For more detailed information:

1. **NLM Architecture**: `docs/ARCHITECTURE.md`
2. **Scientific Background**: `docs/SCIENCE.md`
3. **Experiment Documentation**: `docs/EXPERIMENTS.md`
4. **Performance Analysis**: `docs/PERFORMANCE.md`
5. **Limitations and Known Issues**: `docs/LIMITATIONS.md`

## Acknowledgements

This project builds on the foundations of:
- Biological brain research and neural computation
- Spiking neural network theory
- Neuromodulation and learning systems
- The open-source community and its contributions

## License

MIT License. See `LICENSE` file for details.