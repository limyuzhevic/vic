# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. The NLM is an experimental artificial developmental brain project that implements spiking neural networks with learning capabilities.

The Python bindings provide access to the full functionality of the NLM system through pybind11, enabling users to create, simulate, and experiment with artificial brains.

## Quick Start

### Installation

```bash
# Install NLM from source
pip install pynlm

# Or install in development mode
pip install -e .

# Verify installation
import pynlm
print(f"NLM version: {pynlm.__version__}")
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Classes

### Config

The `Config` class manages configuration parameters for the NLM system.

#### Creation

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
```

#### Configuration Operations

```python
# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)

# Get configuration values
neuron_count = config.getOr("brain.neuron_count", 1000)
if config.has("brain.synapse_density"):
    density = config.get("brain.synapse_density")

# Save/load configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")

# List all keys
keys = config.getKeys()
```

#### Built-in Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Brain

The `Brain` class represents the central neural simulation brain with all integrated systems.

#### Creation

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Core Methods

```python
# Simulation control
brain.step(step_number)                    # Run one simulation step
brain.step(step_number, time)              # Run step with timestamp
brain.reset()                              # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)   # Inject sensory data
brain.produceAction()                      # Get motor action

# State management
brain.save("checkpoint.bin")               # Save state to file
brain.load("checkpoint.bin")               # Load state from file

# Region management
region_id = brain.addRegion("cortex")      # Add a new neural region
region = brain.getRegion(region_id)         # Get region by ID
```

#### Statistics and Monitoring

```python
# Basic statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Network properties
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Development status
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Log detailed status
brain.logStatus()
```

### AgentBrain

The `AgentBrain` class provides an interface for connecting the NLM brain to the world environment.

#### Creation and Initialization

```python
import pynlm

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
agent.initialize(world)
```

#### Core Methods

```python
# Sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Motor control
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)

# Subsystem control
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

#### Statistics

```python
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

### SimpleWorld

The `SimpleWorld` class represents a 2D world for NLM simulation.

#### Creation and Configuration

```python
import pynlm

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Reset world
world.reset()
```

#### Core Methods

```python
# Simulation control
world.update(timestep)                   # Update world
world.setRandomSeed(42)                   # Set random seed

# Agent interaction
world.applyMotorCommand(action, time)     # Apply motor command

# State access
percept = world.getSensoryPercept()       # Get sensory percept
body = world.getAgentBody()               # Get agent body

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

#### Properties

```python
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime()}")
print(f"Max energy: {world.getMaxEnergy()}")
```

## Convenience Functions

The API includes several convenience functions to simplify common tasks:

### createSimpleBrain

```python
import pynlm

# Create a simple brain with default settings
brain = pynlm.createSimpleBrain(neuron_count=500)
brain.initialize()
```

### createSimpleWorld

```python
import pynlm

# Create a simple world with custom dimensions
world = pynlm.createSimpleWorld(width=30, height=30)
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
```

### createSimpleAgent

```python
import pynlm

# Create a complete agent setup (brain + world + agent)
brain, world, agent = pynlm.createSimpleAgent(neuron_count=300)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### runSimulation

```python
import pynlm

# Create all components
brain, world, agent = pynlm.createSimpleAgent()
agent.initialize(world)

# Run simulation
pynlm.runSimulation(brain, world, agent, 1000)
```

### runSimulationWithStats

```python
import pynlm

# Create all components
brain, world, agent = pynlm.createSimpleAgent()
agent.initialize(world)

# Run simulation and get statistics
stats = pynlm.runSimulationWithStats(brain, world, agent, 500)

print(f"Average firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Maximum firing rate: {stats['max_firing_rate']:.3f}")
```

## Complete Simulation Example

This example demonstrates how to create a complete agent simulation:

```python
import pynlm
import time

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # 8. Print final results
    print("\n=== Final Simulation Results ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(1000)
```

## Enumerations

The API includes several enumerations for representing different types:

### NeuronType

- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

### SynapseType

- `Excitatory` - Excitatory synapses
- `Inhibitory` - Inhibitory synapses
- `Modulatory` - Modulatory synapses
- `Electrical` - Electrical synapses
- `GapJunction` - Gap junctions

### DevelopmentalStage

- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### ActionType

- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### MotorCommand

- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

### WorldObjectType

- `Empty` - Empty space
- `Resource` - Resource
- `Hazard` - Hazard
- `Wall` - Wall
- `Marker` - Marker

## Error Handling

The NLM Python API includes comprehensive error handling:

### Common Exceptions

- `RuntimeError` - General runtime errors
- `std::invalid_argument` - Invalid arguments

### Error Handling Patterns

```python
import pynlm

# Error handling example
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Error initializing brain: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Considerations

### Memory Management

The NLM Python API uses shared pointers for automatic memory management:

```python
# Components are automatically managed by shared pointers
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# No manual memory management needed
# Components will be automatically deallocated
```

### Simulation Performance

- Use `runSimulationWithStats` for performance monitoring
- Enable specific subsystems only when needed
- Use efficient data types for configuration values
- Consider parallel processing for large networks

## Configuration Files

### Simple Format (.cfg)

The simple configuration format uses key=value pairs:

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
```

### JSON Format (.json)

The JSON format provides more sophisticated configuration:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "scale": 1.0
        }
    }
}
```

## Advanced Usage

### Checkpoint Management

The API supports checkpoint management for saving and loading brain states:

```python
# Save brain state
brain.save("checkpoint1.bin")

# Load brain state
brain.load("checkpoint2.bin")
```

### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.002)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Multiple Regions

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add multiple regions
region1_id = brain.addRegion("Cortex")
region2_id = brain.addRegion("Thalamus")
region3_id = brain.addRegion("Hippocampus")

# Get regions
region1 = brain.getRegion(region1_id)
region2 = brain.getRegion(region2_id)
region3 = brain.getRegion(region3_id)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain doesn't fire

**Solution:**

```python
# Ensure brain is properly initialized
brain = pynlm.createBrain(config)
brain.initialize()

# Inject current to stimulate neurons
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)

# Run simulation
brain.step(step)
```

#### Issue: ImportError - No module named 'pynlm'

**Solution:**

```bash
# Install properly
pip install --force-reinstall .

# Or install from source
pip install -e .
```

#### Issue: Segmentation fault

**Solution:**

```python
# Ensure proper initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Call reset before re-running
brain.reset()
brain.initialize()
```

## Best Practices

### 1. Initialize Before Use

Always initialize components before using them:

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
```

### 2. Use Convenience Functions

Leverage the convenience functions for common tasks:

```python
# Instead of manual setup
brain, world, agent = pynlm.createSimpleAgent()
agent.initialize(world)

# Use runSimulation with stats
stats = pynlm.runSimulationWithStats(brain, world, agent, 100)
```

### 3. Monitor Performance

Use the statistics functions to monitor simulation performance:

```python
stats = pynlm.runSimulationWithStats(brain, world, agent, 1000)
print(f"Average firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Total spikes: {stats['total_spikes']}")
```

### 4. Error Handling

Always include proper error handling:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # Your simulation code here
except pynlm.RuntimeError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 5. Configuration Management

Use configuration files for complex setups:

```python
# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config.loadFromFile("my_config.cfg")
```

## Further Reading

### Related Documentation

- [NLM Architecture Documentation](docs/ARCHITECTURE.md) - System architecture overview
- [NLM Science Documentation](docs/SCIENCE.md) - Scientific background
- [NLM Experiment Documentation](docs/EXPERIMENTS.md) - Experiment descriptions

### Advanced Topics

- Neural prediction and concept formation
- Neuromodulation and learning
- Memory systems and consolidation
- Developmental processes
- Social learning and interaction

## Version Information

- **Version**: 0.1.0
- **Author**: NLM Authors
- **License**: MIT
- **Project**: NLM (Neural Learning Machine)

## Contact and Support

For issues, questions, or feature requests:

1. Check the [NLM GitHub repository](https://github.com/nlm-project/nlm)
2. Review the documentation
3. Check the examples and tests
4. Submit an issue on GitHub

---

*This documentation is continuously updated. Please refer to the latest documentation for the most current information.*

## API Reference

For the complete API reference, including all method signatures and parameters, please refer to the individual class documentation above or consult the source code with appropriate IDE documentation tools.

The NLM Python API provides a comprehensive interface for creating, simulating, and experimenting with artificial neural systems. The convenience functions and improved error handling make it easier to get started with NLM while still providing access to all the advanced features.
