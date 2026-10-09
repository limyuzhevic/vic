# NLM Python Bindings - Comprehensive Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings.
The bindings provide access to the complete integrated artificial brain system with all its major components.

## Overview

NLM is a research project investigating computational brain-like systems. It implements a neural system that:

- Begins in a primitive developmental state and acquires increasingly complex abilities through interaction
- Uses spiking neural computation with real LIF neuron dynamics
- Integrates memory systems (working, episodic, associative)
- Implements prediction and cognitive systems
- Uses neuromodulation for motivation and learning
- Supports developmental stages and plasticity regulation

## Installation

To install the NLM Python bindings, follow these steps:

```bash
# Clone the NLM repository
cd /path/to
# Build the project with Python bindings
mkdir build
 cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# The Python module will be installed in:
# - Linux: ~/.local/lib/python3.x/site-packages/pynlm.so
# - macOS: ~/Library/Python/3.x/lib/python/site-packages/pynlm.so
# - Windows: C:\Python3.x\Lib\site-packages\pynlm.pyd
```

Alternatively, if you have a package manager like pip, you can install directly:

```bash
pip install pynlm
```

## Basic Usage

### 1. Creating a Brain

The Brain is the central component that implements all neural computations.

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.create_brain()

# Create a brain with custom configuration
config = pynlm.create_default_config()
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
brain = pynlm.create_brain(config)

# Initialize the brain (required before use)
brain.initialize()
print("Brain initialized with", brain.getTotalNeuronCount(), "neurons")
```

### 2. Creating a World

The world simulates the environment in which the agent interacts.

```python
# Create a simple 2D world
world = pynlm.create_simple_world()

# Configure world dimensions and sensors
world.configure(
    width=100,        # World width in meters
    height=100,       # World height in meters
    visionWidth=20,   # Vision sensor resolution (width)
    visionHeight=20   # Vision sensor resolution (height)
)

# Set agent starting position
world.setAgentStart(x=50, y=50)

# Set world properties
world.setMaxEnergy(1.0)
world.setEnergyDecayRate(0.001)
```

### 3. Creating an Agent Brain Interface

The AgentBrain connects the neural brain to the world.

```python
# Create agent brain interface
agent_brain = pynlm.create_agent_brain(brain)

# Initialize with world reference
agent_brain.initialize(world)

# Check expected sensory and motor sizes
print("Expected sensory input size:", agent_brain.getSensoryInputSize())
print("Expected motor output size:", agent_brain.getMotorOutputSize())
```

## Simulation Loop

The basic simulation loop involves:

1. Getting sensory input from the world
2. Processing sensory input through the agent brain
3. Updating the world
4. Getting actions from the brain
5. Applying actions in the world

```python
# Simulation parameters
num_steps = 1000

# Main simulation loop
for step in range(num_steps):
    # 1. Get sensory input from world
    percept = world.getSensoryPercept()
    
    # 2. Process sensory input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # 3. Update world
    world.update(0.001)  # 1ms timestep
    
    # 4. Get action from brain
    action = brain.produceAction()
    
    # 5. Apply action in world
    world.applyMotorCommand(action)
    
    # Optional: Log progress every 100 steps
    if step % 100 == 0:
        status = brain.logStatus()
        print(f"Step {step}: {status}")
```

## Sensory Processing

NLM supports multiple sensory modalities:

### Vision Sensor

Vision represents visual information as a 2D grid of intensities.

```python
# Create vision sensor
from pynlm import Vision

# Create vision with specific dimensions
vision = Vision(width=10, height=10, channels=1)

# Set pixel data (0.0 to 1.0, normalized)
data = []
for y in range(10):
    row = []
    for x in range(10):
        # Center region is brighter
        intensity = 1.0 if (x-5)**2 + (y-5)**2 < 25 else 0.0
        row.append(intensity)
    data.append(row)

# Flatten data for vision sensor
flat_data = [val for row in data for val in row]
vision.setData(flat_data)

print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}")
print(f"Vision data size: {len(vision.getData())}")
```

### Audio Sensor

Audio represents sound information as a time series.

```python
# Create audio sensor
from pynlm import Audio

# Create audio with sample rate and duration
audio = Audio(sampleRate=44100, numSamples=1024)

# Generate simple sine wave
samples = []
for i in range(1024):
    t = i / 44100.0
    sample = 0.5 * (1.0 if (i % 100) < 50 else -1.0)
    samples.append(sample)

audio.setData(samples)

print(f"Audio sample rate: {audio.getSampleRate()}")
print(f"Audio duration: {len(samples)/audio.getSampleRate():.3f} seconds")
```

### Internal Signals

Internal signals represent the agent's internal state (energy, hormones, etc.).

```python
from pynlm import InternalSignals

internal = InternalSignals()

# Add various internal signals
internal.addSignal(0.8)  # Energy level
internal.addSignal(0.3)  # Hormone level
internal.addSignal(0.9)  # Motivation level

# Check current signals
data = internal.getData()
print(f"Internal signals: {data}")
```

## Action Generation

The brain produces actions based on neural activity patterns.

```python
# Get action from brain
action = brain.produceAction()

# Check action properties
print(f"Action type: {action.type}")
print(f"Action name: {action.getName()}")

# Check if action has parameters
if action.parameters:
    print(f"Action parameters: {action.parameters}")
```

### Action Types

NLM supports various action types:

```python
from pynlm import ActionType

# Movement actions
action = ActionType.MoveForward
print(f"Action name: {action.getName()}")

# Orientation actions
turn_left = ActionType.TurnLeft
print(f"Action: {turn_left.getName()}")

# Sensory actions
look = ActionType.Look
print(f"Action: {look.getName()}")

# Interaction actions
interact = ActionType.Interact
print(f"Action: {interact.getName()}")

# State actions
rest = ActionType.Rest
wait = ActionType.Wait
print(f"Rest: {rest.getName()}, Wait: {wait.getName()}")
```

## Memory Systems

NLM integrates three memory systems:

### Working Memory

Working memory maintains current information and active representations.

```python
# Access working memory
working_memory = brain.getWorkingMemory()

# Store a pattern
pattern = [0.1, 0.5, 0.9, 0.2, 0.7]
working_memory.store(pattern, strength=1.0)

# Retrieve current memory
retrieved = working_memory.retrieve()
print(f"Working memory: {retrieved}")

# Check capacity
print(f"Memory capacity: {working_memory.getCapacity()}")
print(f"Active traces: {working_memory.getActiveTraces()}")
```

### Episodic Memory

Episodic memory stores experiences and events.

```python
# Access episodic memory
episodic_memory = brain.getEpisodicMemory()

# Store an episode
episode = {
    'sensoryState': [0.2, 0.6, 0.8, 0.4],
    'positionX': 50.0,
    'positionY': 50.0,
    'orientation': 0.0,
    'action': ActionType.MoveForward,
    'reward': 0.8,
    'energy': 0.9,
    'novelty': 0.3,
    'resultingSensoryState': [0.3, 0.7, 0.9, 0.5],
    'resultingReward': 0.6
}

# In practice, episodic memories are created automatically during simulation
# and stored in the brain's episodic memory system
```

### Associative Memory

Associative memory forms connections between experiences and patterns.

```python
# Access associative memory
associative_memory = brain.getAssociativeMemory()

# Associate patterns
pattern_a = [0.1, 0.2, 0.3, 0.4]
pattern_b = [0.5, 0.6, 0.7, 0.8]

associative_memory.associate(pattern_a, pattern_b, strength=0.8)

# Retrieve associated patterns
results = associative_memory.retrieve(pattern_a, maxResults=3)
print(f"Associated patterns: {len(results)}")
```

## Prediction System

The prediction system predicts future sensory states and computes prediction errors.

```python
# Access prediction system
prediction_system = brain.getPredictionSystem()

if prediction_system:
    print(f"Prediction error: {prediction_system.getPredictionError():.4f}")
    print(f"Prediction confidence: {prediction_system.getConfidence():.4f}")
    
    # Check error history
    error_history = prediction_system.getErrorHistory()
    if error_history:
        print(f"Recent prediction errors: {error_history[-5:]}")
```

## Neuromodulation

Neuromodulation affects neural excitability and plasticity.

```python
# Access neuromodulation systems
brain_instance = brain  # Get brain instance

# Check neuromodulation levels
if hasattr(brain_instance, 'getCuriosityLevel'):
    print(f"Curiosity level: {brain_instance.getCuriosityLevel():.4f}")
    
if hasattr(brain_instance, 'getNoveltyLevel'):
    print(f"Novelty level: {brain_instance.getNoveltyLevel():.4f}")
    
if hasattr(brain_instance, 'getNeuromodulationLevel'):
    print(f"Neuromodulation level: {brain_instance.getNeuromodulationLevel():.4f}")
```

## Development

The brain develops through different stages with changing plasticity.

```python
# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Set developmental stage
brain.setDevelopmentalStage(ActionType.Maturation)
```

## Checkpoint and Save/Load

The brain can save and load its state for persistence and resuming simulations.

```python
# Save brain state
brain.save("brain_checkpoint.nlm")
print("Brain state saved to brain_checkpoint.nlm")

# Load brain state
# brain.load("brain_checkpoint.nlm")
# print("Brain state loaded")
```

## Advanced Usage

### Custom Actions

Create custom actions with specific parameters:

```python
from pynlm import Action

# Create custom action
custom_action = Action(
    type=ActionType.Custom,
    parameters=[0.5, 0.8, 0.3, 0.9]
)

print(f"Custom action: {custom_action.getName()}")
print(f"Parameters: {custom_action.getParameters()}")
```

### Multiple Agents

Create multiple agents that share the same brain:

```python
# Create shared brain
shared_brain = pynlm.create_brain()
shared_brain.initialize()

# Create multiple agent brains
agent1 = pynlm.create_agent_brain(shared_brain)
agent2 = pynlm.create_agent_brain(shared_brain)

# Create different worlds
world1 = pynlm.create_simple_world()
world2 = pynlm.create_simple_world()

# Initialize both agents
agent1.initialize(world1)
agent2.initialize(world2)
```

### Simulation with Rewards

Implement a reward-based learning scenario:

```python
# Create brain with reward modulation
brain = pynlm.create_brain()
brain.initialize()

# Enable reward modulation
# Note: This depends on the specific brain implementation
# brain.enableRewardModulation(True)

# Run simulation loop with rewards
for step in range(1000):
    # Get sensory input and process
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    
    # Execute world update
    world.update(0.001)
    
    # Get action
    action = brain.produceAction()
    world.applyMotorCommand(action)
    
    # Simulate reward (in practice, this comes from environment)
    # reward = world.getReward()
    # brain.applyRewardModulation(reward)
```

## API Reference

### Core Classes

- **Brain**: The central neural simulation brain class
- **Config**: Configuration system for simulation parameters
- **AgentBrain**: Interface between brain and world
- **SimpleWorld**: Simple 2D world simulation
- **Vision**: Visual sensory input
- **Audio**: Audio sensory input
- **InternalSignals**: Internal state signals
- **Action**: Motor actions and behaviors
- **SensoryPercept**: Combined sensory input from all modalities

### Helper Functions

- **create_default_config()**: Create default configuration
- **create_brain()**: Create brain with default configuration
- **create_simple_world()**: Create simple world
- **create_agent_brain()**: Create agent brain interface
- **to_float_list()**: Convert Python list to NLM data format
- **create_action()**: Create action from Python data
- **create_vision()**: Create vision sensor from Python data
- **create_audio()**: Create audio sensor from Python data

### Constants

- **INVALID_NEURON_ID**: Invalid neuron identifier
- **INVALID_SYNAPSE_ID**: Invalid synapse identifier
- **INVALID_REGION_ID**: Invalid region identifier
- **INVALID_POPULATION_ID**: Invalid population identifier

## Troubleshooting

### Common Issues

**Issue 1: Module not found**
```python
# Solution: Ensure the Python module is in your Python path
import sys
print(sys.path)
```

**Issue 2: Import errors**
```python
# Solution: Check if dependencies are installed
# and that the C++ module compiled successfully
```

**Issue 3: Wrong Python version**
```python
# Solution: Ensure you're using the correct Python version
# that was used to build the module
import sys
print(f"Python version: {sys.version}")
```

**Issue 4: Memory issues**
```python
# Solution: Check if you're using too much memory
# and consider reducing simulation complexity
import tracemalloc
tracemalloc.start()
# ... run simulation ...
print(tracemalloc.get_traced_memory())
```

### Debugging Tips

1. **Enable logging**: Use the brain's logStatus() method to see current state
2. **Check parameters**: Verify all configuration values are set correctly
3. **Monitor memory**: Track memory usage during long simulations
4. **Validate inputs**: Ensure all input data is properly formatted
5. **Test components**: Test each component separately before integrating

## Future Enhancements

The Python bindings are under active development. Future enhancements may include:

- More sophisticated sensory modalities
- Advanced action spaces
- Learning algorithms integration
- Visualization tools
- Performance optimizations
- Multi-agent simulation support
- Real-world environment integration

## License

This software is licensed under the MIT License. See the LICENSE file for more information.

## Authors

NLM (Neural Learning Machine) is a research project investigating computational brain-like systems.
For more information, visit the project documentation and source code.
