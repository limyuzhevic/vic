# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to a sophisticated computational brain simulation framework. This documentation covers all available Python classes, functions, and provides usage examples.

## Installation

```bash
# Clone the repository
cgit clone https://github.com/nlm-project/nlm.git

# Build the Python bindings
cd nlm
mkdir build
cd build
cmake ..
make -j4
```

The Python module `pynlm` will be installed in the Python site-packages directory.

## Module Structure

### Core Classes

#### Config
Configuration management for NLM simulation parameters.

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load configuration from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

**Example:**
```python
import pynlm

# Create default configuration
config = pynlm.Config()

# Set parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)

# Save configuration
config.saveToFile("brain_config.json")
```

#### Brain
Central neural simulation brain class implementing real spiking neural computation.

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(currentStep)` - Perform simulation step
- `step(currentStep, currentTime)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input into brain
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current into neurons of specific type
- `produceAction()` - Produce motor action based on neural activity

**Properties:**
- `config` - Get the configuration object

**Example:**
```python
import pynlm

# Create configuration
config = pynlm.Config()
config.set("neuron_count", 500)
config.set("region_count", 1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)
```

#### SimpleWorld
Simple 2D world for NLM simulation with agent interaction.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept from world
- `getAgentBody()` - Get agent body state

**Properties:**
- `width` - World width
- `height` - World height

**Example:**
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)

# Set agent start
world.setAgentStart(50, 50)

# Reset and get percept
world.reset()
percept = world.getSensoryPercept()
```

#### AgentBrain
Interface connecting NLM brain to world with sensory-motor processing.

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain activity into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system

**Properties:**
- `brain` - Get the underlying brain object

**Example:**
```python
import pynlm

# Create components
config = pynlm.Config()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)

# Initialize system
agent_brain.initialize(world)
brain.initialize()

# Process sensory input
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Produce action
action = brain.produceAction()
```

#### Action
Motor action representation for neural output.

**Attributes:**
- `type` - Action type (from ActionType enum)
- `parameters` - Action parameters (vector of floats)

**Example:**
```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.2, 0.1])
```

#### Sensory Percept
Container for all sensory signals from environment.

**Key Methods:**
- `getVision()`, `setVision()` - Vision data
- `getTouch()`, `setTouch()` - Proprioceptive data
- `getInternal()`, `setInternal()` - Internal signals
- `getAudio()`, `setAudio()` - Audio data

**Example:**
```python
import pynlm

# Create and manipulate percept
percept = pynlm.SensoryPercept()

# Set vision data
vision = percept.getVision()
# ... process vision ...
percept.setVision(vision)
```

#### Agent Body
Physical representation of agent in world.

**Attributes:**
- `x, y` - Position
- `orientation` - Facing direction (radians)
- `velocityX, velocityY` - Linear velocity
- `angularVelocity` - Angular velocity
- `energy` - Energy level
- `health` - Health status
- `age` - Age
- `isMoving` - Movement state
- `isTurning` - Turning state

**Example:**
```python
import pynlm

body = pynlm.AgentBody()
print(f"Position: ({body.x}, {body.y})")
print(f"Energy: {body.energy}")
```

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Neuromodulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### ActionType
Complete action type enumeration with over 20 types:
- Movement actions (MoveForward, MoveBackward, etc.)
- Looking actions (Look, LookUp, LookDown, etc.)
- Interaction actions (Interact, Eat, Drink, etc.)
- State actions (Rest, Wait, Custom)

#### NeuronStatus
- `Resting` - Resting state
- `Active` - Firing state
- `Refractory` - Refractory period
- `Inhibited` - Inhibited state

### Factory Functions

#### createDefaultConfig()
```python
config = pynlm.createDefaultConfig()
```

#### createBrain(config)
```python
brain = pynlm.createBrain(config)
```

#### createSimpleWorld()
```python
world = pynlm.createSimpleWorld()
```

#### createAgentBrain(brain)
```python
agent_brain = pynlm.createAgentBrain(brain)
```

### Convenience Functions

#### INVALID_ constants
```python
# Invalid neuron/synapse/region/population identifiers
neuron_id = pynlm.INVALID_NEURON_ID
synapse_id = pynlm.INVALID_SYNAPSE_ID
region_id = pynlm.INVALID_REGION_ID
population_id = pynlm.INVALID_POPULATION_ID
```

## Advanced Usage

### Batch Operations
```python
# Process multiple sensory inputs in batch
percepts = [world.getSensoryPercept() for _ in range(10)]
for percept in percepts:
    agent_brain.processSensoryInput(percept)
```

### Simulation Control
```python
# Custom simulation loop with logging
for step in range(1000):
    current_time = step * config.getOr<double>("simulation_timestep", 0.001)
    
    # Get world percept
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    # Step brain
    brain.step(step, current_time)
    
    # Apply neuromodulation
    if step % 100 == 0:
        agent_brain.applyRewardModulation(1.0, 0.5)
```

### Development and Learning
```python
# Enable learning systems
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Run learning episode
for step in range(500):
    # Reset and reinitialize
    brain.reset()
    brain.initialize()
    
    # Process episode
    for t in range(100):
        # ... simulation step ...
        
    # Update development
    agent_brain.updateDevelopment(0.001)
```

## Troubleshooting

### Common Issues

1. **Module not found**
   ```bash
   pip install pynlm
   ```

2. **Missing dependencies**
   Ensure all C++ dependencies are installed (CMake 3.16+, Python development headers, pybind11)

3. **Import errors**
   Check that the module was built correctly

## Examples Directory

Complete examples are available in the examples/ directory:
- `basic_usage.py` - Basic simulation setup
- `sensory_mactor.py` - Sensory-motor loop example
- `learning_demo.py` - Learning and development demonstration

## Further Reading

For more detailed information about the underlying C++ implementation and theory:
- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Guide](docs/ARCHITECTURE.md)

## License

MIT License - See LICENSE file for details.
