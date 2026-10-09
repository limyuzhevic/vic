# Python Bindings Documentation - NLM/熙然

## Overview

This document provides comprehensive documentation for the **NLM Python Bindings** (`pynlm` module), which enables Python users to interact with the NLM (Neural Learning Machine) framework through an intuitive and Pythonic API.

The NLM Python bindings provide full access to all NLM functionality while abstracting away the complexity of the underlying C++ implementation. This module enables you to create, configure, and simulate virtual brains that can learn, remember, and make decisions.

## Installation

### Prerequisites

```bash
python >= 3.8
pip
```

### Installation from Source

```bash
# Clone the NLM repository
https://github.com/Kilo-Org/nlm.git

# Navigate to the NLM directory
cd nlm

# Install the Python bindings
pip install -e .

# Or use scikit-build-core (recommended for development)
pip install scikit-build-core pybind11 numpy pytest
pip install -e . --config-settings=cmake.define.NLM_PYTHON=ON
```

### Installation from Binary

If pre-built wheels are available for your platform:

```bash
pip install pynlm
```

### Verification

```python
import pynlm
print(f"NLM version: {pynlm.__version__}")

# Create a basic brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

## Quick Start Examples

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

### Example 2: Complete Agent in World

```python
import pynlm

# Create all components
brain, world, agent = pynlm.createResearchSetup()

# Run simulation with learning enabled
rewards, firing_counts = pynlm.runStandardSimulation(
    brain, world, agent, 200, enableLearning=True, verbose=True
)

# Analyze results
avg_reward = sum(rewards) / len(rewards) if rewards else 0
max_firing = max(firing_counts) if firing_counts else 0
print(f"Average reward: {avg_reward:.3f}, Max firing: {max_firing}")
```

### Example 3: Context Manager Usage

```python
import pynlm

# Use context managers for automatic resource cleanup
with pynlm.createBrainConfig("minimal") as config:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    # Brain and config are automatically cleaned up

# Or with world
with pynlm.createSimpleWorld() as world:
    world.configure(15, 15, 6, 6)
    world.reset()
    # World is automatically cleaned up
```

### Example 4: Advanced Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createBrainConfig("research")
config["brain.plasticity.enabled"] = "true"
config["brain.development.enabled"] = "true"
config["brain.reward.enabled"] = "true"
config["brain.curiosity.enabled"] = "true"

brain = pynlm.createBrain(config)
brain.initialize()

# Use brain with all learning features
with pynlm.createSimpleWorld() as world:
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run learning episode
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Main Classes

### Brain Class

The **Brain** class is the central neural simulation engine that contains neurons, synapses, and regions that work together to simulate intelligent behavior.

#### Creation and Initialization

```python
# Create with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize (required before use)
brain.initialize()

# Create with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuronCount", 1000)
config.set("brain.regionCount", 5)
brain = pynlm.createBrain(config)
brain.initialize(enable_development=True, enable_plasticity=True)
```

#### Simulation Methods

```python
# Run simulation
brain.step(42)                    # Step with default timestamp
brain.step(42, 4.2)               # Step with explicit timestamp

# Get brain statistics
brain.getTotalNeuronCount()       # Total number of neurons
brain.getTotalSynapseCount()      # Total number of synapses
brain.getFiringNeuronCount()     # Neurons currently firing
brain.getAverageFiringRate()      # Average firing rate across all neurons
brain.getExcitationInhibitionRatio()  # E/I balance

# State management
brain.save("checkpoint.pkl")      # Save brain state
brain.load("checkpoint.pkl")      # Load brain state
brain.reset()                     # Reset to initial state
```

#### Input/Output Methods

```python
# Process sensory input
brain.receiveSensoryInput(sensory_input)  # Inject sensory data

# Get motor action
action = brain.produceAction()           # Get motor action with reward
```

#### Region Management

```python
# Add regions
region_id = brain.addRegion("cortex")
region_id = brain.addRegion("")  # Use default name

# Get regions
region = brain.getRegion(region_id)
if region:
    print(f"Region has {region.getNeuronCount()} neurons")

# Get region statistics
brain.getRegionCount()               # Number of regions
brain.getRegionIds()                  # All region IDs
```

### Config Class

The **Config** class provides configuration management for all NLM components using a flexible key-value storage system.

#### Basic Usage

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuronCount"):
    neurons = config.get("brain.neuronCount")
    print(f"Number of neurons: {neurons}")

# Get all configuration keys
keys = config.getKeys()
print(f"Available configuration keys: {keys}")
```

#### Configuration Modification

```python
# Modify configuration
config.set("brain.neuronCount", 1000)
config.set("learning.rate", 0.001)
config["brain.synapseDensity"] = 0.1  # Pythonic access

# Create specialized configurations
config.createDefault()       # Sensible defaults for basic operation
config.createMinimal()       # Minimal configuration
config.createHighPerformance()  # Optimized for performance
config.createExperimental()   # For research
```

#### File I/O Operations

```python
# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Load from command line arguments
import sys
config.loadFromArgs(len(sys.argv), sys.argv)
```

#### Context Manager Support

```python
# Safe configuration management with context managers
with pynlm.createDefaultConfig() as config:
    config.set("brain.neuronCount", 500)
    # Configuration automatically cleaned up
```

### AgentBrain Class

The **AgentBrain** class connects NLM brains to the world environment, providing sensory processing and motor decoding.

#### Creation and Initialization

```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Configure agent subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Sensory Processing

```python
# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Get motor command from brain activity
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development

```python
# Update development
agent.updateDevelopment(0.1)
stage = agent.getDevelopmentalStage()
print(f"Current developmental stage: {stage}")
```

#### Statistics

```python
# Get agent statistics
agent.getCuriosityLevel()           # Current curiosity level
agent.getNoveltyLevel()             # Current novelty level
agent.getPredictionError()          # Prediction error signal
agent.getNeuromodulationLevel()     # Overall neuromodulation level
```

### SimpleWorld Class

The **SimpleWorld** class represents a 2D simulated environment where agents interact with the world.

#### Basic Configuration

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions and agent sensing
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)
```

#### World Operations

```python
# Update world physics
world.update(timestep=0.1)

# Apply motor command to world
world.applyMotorCommand(motor_cmd, world.getSimulationTime())

# Get sensory percept for agent
percept = world.getSensoryPercept()
vision = percept.getVision()

# Get agent body state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
```

#### World Object Management

```python
# Add objects to world
world.addObject(pynlm.WorldObject(x=5, y=5, type=pynlm.WorldObjectType.Resource, value=10.0))

# Remove objects
world.removeObject(x=5, y=5)

# Check if position is valid
if world.isValidPosition(x=3, y=4):
    world.addObject(obj)
```

#### World Properties

```python
# Get world properties
world.getWidth()
world.getHeight()
world.getMaxEnergy()
world.getEnergyDecayRate()
world.getSimulationTime()
world.getRandomSeed()

# Set world properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(42)
```

#### Convenience Methods

```python
# Create and add objects conveniently
resource = world.createResource(x=5, y=5, value=20.0)
hazard = world.createHazard(x=3, y=3, value=2.0)
wall = world.createWall(x=2, y=2, radius=1.0)

# Get objects by type
resources = world.getObjectsByType(pynlm.WorldObjectType.Resource)

# Get object at position
obj_at = world.getResourceAt(x=5, y=5)

# Run complete episode
events = world.runEpisode(numSteps=50, verbose=True)
for event in events:
    print(event)
```

## Enumerations

### NeuronType

```python
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Sensory       # Sensory neurons
pynlm.NeuronType.Motor         # Motor neurons
pynlm.NeuronType.Internal     # Internal neurons
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial        # Initial state
pynlm.DevelopmentalStage.CriticalPeriod # Critical learning period
pynlm.DevelopmentalStage.Maturation     # Maturation phase
pynlm.DevelopmentalStage.Adult         # Adult stage
pynlm.DevelopmentalStage.Aging         # Aging phase
```

### ActionType

```python
pynlm.ActionType.MoveForward   # Move forward
pynlm.ActionType.MoveBackward  # Move backward
pynlm.ActionType.MoveLeft      # Move left
pynlm.ActionType.MoveRight     # Move right
pynlm.ActionType.TurnLeft      # Turn left
pynlm.ActionType.TurnRight     # Turn right
pynlm.ActionType.Look          # Look forward
pynlm.ActionType.LookUp        # Look up
pynlm.ActionType.LookDown      # Look down
pynlm.ActionType.Interact     # Interact with objects
pynlm.ActionType.Eat          # Eat food
pynlm.ActionType.Drink        # Drink water
pynlm.ActionType.Rest         # Rest/remain still
pynlm.ActionType.Wait         # Wait for next step
pynlm.ActionType.Custom       # Custom action
```

### MotorCommand

```python
pynlm.MotorCommand.MoveForward # Move forward
pynlm.MotorCommand.MoveBackward # Move backward
pynlm.MotorCommand.TurnLeft     # Turn left
pynlm.MotorCommand.TurnRight    # Turn right
pynlm.MotorCommand.LookLeft     # Look left
pynlm.MotorCommand.LookRight    # Look right
pynlm.MotorCommand.Interact    # Interact
pynlm.MotorCommand.Wait        # Wait
```

### WorldObjectType

```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource (food, water, etc.)
pynlm.WorldObjectType.Hazard   # Hazardous object
pynlm.WorldObjectType.Wall      # Wall/obstacle
pynlm.WorldObjectType.Marker    # Marker/target
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuronCount` | int | 1000 | Total number of neurons |
| `brain.synapseDensity` | float | 0.1 | Synapse connectivity density |
| `brain.connectionProbability` | float | 0.05 | Probability of connection between neurons |
| `brain.initialWeightMean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initialWeightStd` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learningRate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tauPlus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tauMinus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.predictionError.enable` | bool | true | Enable prediction error computation |

## Performance Considerations

- **Resource Management**: Use context managers (`with` statements) for automatic cleanup
- **Batch Operations**: Process multiple steps before logging for better performance
- **Memory Usage**: Monitor memory usage with `brain.getBrainState()`
- **Verbose Mode**: Set `verbose=False` for production simulations
- **Configuration**: Use minimal configurations for faster initialization

## Error Handling

The bindings use standard Python exception handling:

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Error: {e}")
except pynlm.KeyError as e:
    print(f"Configuration key not found: {e}")
except pynlm.ValueError as e:
    print(f"Invalid parameter value: {e}")
```

Common errors include:
- `RuntimeError`: General runtime errors
- `KeyError`: Configuration key not found
- `ValueError`: Invalid parameter values

## Further Reading

For more detailed information, see:
- `README.md`: Basic setup and installation
- `HOW_TO_USE.md`: Comprehensive usage guide
- `easy_usage.md`: Quick start guide for beginners
- `docs/`: Technical documentation

## API Reference

For complete API documentation, use Python's help system:

```python
import pynlm
help(pynlm.Brain)
help(pynlm.AgentBrain)
help(pynlm.SimpleWorld)
help(pynlm.Config)
```

## Additional Resources

- **Documentation**: Check the `docs/` directory for comprehensive technical documentation
- **Tutorials**: Refer to `easy_usage.md` for beginner-friendly tutorials
- **Examples**: See `HOW_TO_USE.md` for practical examples and usage patterns

## License

MIT License. See `LICENSE` file for details.

## Authors

NLM Research Project. For more information, visit the project documentation.