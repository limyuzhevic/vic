# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface to the NLM neural simulation framework. This documentation covers all available classes, methods, and provides practical examples for getting started with NLM simulations.

## Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Or install from PyPI (if available)
pip install pynlm
```

## Basic Usage

### Creating a Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Running a Simulation

```python
import pynlm

# Set up configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Optional: log statistics
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Core Classes

### Brain Class

The `Brain` class represents the complete neural simulation engine. It implements:

- **LIF neuron dynamics** with real membrane potential integration
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms** (STDP, Hebbian, structural)
- **Integrated memory systems** (working, episodic, associative)
- **Neuromodulation systems** (dopamine, curiosity, novelty)
- **Cognitive mechanisms** (planning, concept formation, attention)
- **Development stages** from initial to adult phases

#### Key Methods

- `initialize()` - Initialize brain with configuration
- `step(currentStep, currentTime)` - Run simulation step
- `receiveSensoryInput(input)` - Inject sensory data
- `produceAction()` - Generate motor output
- `reset()` - Reset brain state
- `save(filepath)` - Save checkpoint
- `load(filepath)` - Load checkpoint

#### Example: Complete Simulation Loop

```python
import pynlm

# Set up brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Set up environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation episode
for step in range(1000):
    # Update environment
    world.update(0.15)  # 150ms per step
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get motor command and apply to world
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Optional: neuromodulation based on reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
```

### Config Class

The `Config` class manages neural simulation parameters. It supports:

- **Simple key=value format** for basic configuration
- **JSON format** for structured configuration
- **Command-line arguments** for runtime configuration
- **Programmatic configuration** via Python API

#### Configuration Parameters

Key configuration categories include:

**Brain Configuration:**
- `brain.neuron_count` - Total number of neurons (default: 1000)
- `brain.synapse_density` - Synapse connectivity density (default: 0.1)
- `brain.connection_probability` - Connection probability (default: 0.05)

**Plasticity:**
- `plasticity.stdp.enable` - Enable/disable STDP (default: true)
- `plasticity.stdp.learning_rate` - STDP learning rate (default: 0.001)
- `plasticity.hebbian.enable` - Enable/disable Hebbian learning (default: true)

**Neuromodulation:**
- `neuromod.dopamine.scale` - Dopamine modulation scale (default: 1.0)
- `neuromod.curiosity.enable` - Enable curiosity (default: true)
- `neuromod.novelty.enable` - Enable novelty detection (default: true)

#### Example: Configuration

```python
import pynlm

# Create and configure
config = pynlm.createDefaultConfig()

# Configure brain size
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.15)

# Configure plasticity
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)

# Configure neuromodulation
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.cfg")
```

### AgentBrain Class

The `AgentBrain` class provides a high-level interface for neural agents:

- **Sensory processing** from environments
- **Motor output generation** for action execution
- **Neuromodulation application** based on feedback
- **Development system updates** for learning and maturation
- **Curiosity-driven exploration** for autonomous behavior

#### Example: Agent Configuration

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable subsystems
agent.enableRewardModulation(True)      # Reward learning
agent.enableCuriosity(True)              # Exploration
agent.enableDevelopment(True)            # Maturation
agent.enableStructuralPlasticity(True)  # Morphological changes

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
agent.initialize(world)
```

### SimpleWorld Class

The `SimpleWorld` class represents a 2D environment for agent interaction:

- **Agent state** (position, orientation, velocity)
- **Sensory perception** (vision, touch, internal state)
- **Action execution** (movement, interaction)
- **Object management** (resources, hazards, walls)
- **Reward calculation** based on actions

#### Example: World Interaction

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height= mitten
    visionWidth=8,
    visionHeight=8
)

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Reset to start
world.reset()

# Run environment update
for step in range(500):
    world.update(0.1)  # 100ms per update
    
    # Get agent's current state
    body = world.getAgentBody()
    percept = world.getSensoryPercept()
    
    # Agent can make decisions based on world state
    if percept.getVision() and percept.getVision()[0] > 0.5:
        # There's something in vision
        # Agent might decide to move toward it
        pass
```

### SensoryInput Classes

NLM provides several sensory input types:

#### Vision
Represents visual field data with width, height, and color channels.

```python
import pynlm

# Create vision data
vision_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 64  # 8x8x3 example
vision = pynlm.Vision(8, 8, 3)  # 8x8 pixels, 3 color channels
vision.setData(vision_data)
```

#### Audio
Represents audio signals with sample rate and number of samples.

```python
import pynlm

# Create audio data
audio = pynlm.Audio(sample_rate=44100, num_samples=44100)
sample_data = [0.0] * 44100  # Silence
audio.setData(sample_data)
```

#### InternalSignals
Represents internal neural state information.

```python
import pynlm

# Create internal signals
internal = pynlm.InternalSignals()
internal.addSignal(0.5)  # Some internal state value
internal.addSignal(-0.3)
```

### Action Classes

The `Action` class represents motor commands with type and parameters.

```python
import pynlm

# Create simple actions
move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
move_left = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])
interact = pynlm.Action(pynlm.ActionType.Interact, [0.8, 0.2])

# Set action parameters
move_forward.setParameters([0.7, 0.3])
print(f"Move forward speed: {move_forward.getParameters()[0]}")
```

## Advanced Usage

### Learning with Neuromodulation

```python
import pynlm

# Create learning environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=7, visionHeight=7)

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Learning loop
for episode in range(10):
    agent.reset()
    
    for step in range(200):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Print learning progress
        if step % 50 == 0:
            print(f"Episode {episode}, Step {step}:")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Reward: {reward:.3f}")
```

### Development and Maturation

```python
import pynlm

# Create developing agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

print("Development progression:")
for development_step in range(5):
    # Update development
    agent.updateDevelopment(100.0)
    
    # Print developmental stage
    stage = agent.getDevelopmentalStage()
    print(f"Stage {development_step}: {stage}")
    
    # Development affects behavior
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        print("  -> Enhanced learning and plasticity")
    elif stage == pynlm.DevelopmentalStage.Maturation:
        print("  -> Stabilization of learned behaviors")
    elif stage == pynlm.DevelopmentalStage.Adult:
        print("  -> Adult behavior patterns")
```

### Memory Systems

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

print(f"Working memory capacity: {working_memory.getCapacity()}")
print(f"Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")

# Simulate and create memories
for step in range(100):
    brain.step(step)
    
    # Working memory updates
    if step % 10 == 0:
        working_memory.storeToNeuron(brain.getRegionIds()[0], 1.0)
    
    # Episodic memory updates
    if step % 50 == 0 and step > 0:
        episode = episodic_memory.createEpisode()
        episode.positionX = 10.0 + step * 0.1
        episode.positionY = 10.0 + step * 0.1
        episode.action = pynlm.ActionType.Interact
        episode.reward = 1.0
        episodic_memory.storeEpisode(episode)

print(f"Total episodic memories: {len(episodic_memory.getEpisodes())}")
```

### Checkpointing

```python
import pynlm

# Create and train first brain
config1 = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config1)
brain1.initialize()

for step in range(200):
    brain1.step(step)

print(f"Training completed: {brain1.getTotalSpikeCount()} spikes")

# Save checkpoint
checkpoint_file = "my_brain_checkpoint.nlm"
if brain1.save(checkpoint_file):
    print(f"Saved checkpoint to {checkpoint_file}")

# Load checkpoint
config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)
brain2.initialize()

if brain2.load(checkpoint_file):
    print(f"Loaded checkpoint: {brain2.getTotalSpikeCount()} spikes")
    print("Behavior preserved across reload!")
```

## Configuration File Formats

### Simple Format (.cfg)

Basic key=value format for quick configuration:

```
# NLM Configuration File
# Generated by pynlm.createDefaultConfig()

brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
```

### JSON Format (.json)

Structured JSON configuration:

```json
{
    "brain": {
        "neuron_count": 500,
        "synapse_density": 0.15,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    }
}
```

## Performance Tips

1. **Use appropriate timestep**: For real-time applications, use 1-10ms timesteps
2. **Monitor memory usage**: Large brains require careful memory management
3. **Use checkpointing**: Save progress periodically during long simulations
4. **Configure for your use case**: Adjust neuron count and connection density
5. **Enable/disable subsystems**: Only enable what's needed for your task

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**
   - Ensure pynlm is installed: `pip install pynlm`
   - Try installing in development mode: `pip install -e .`

2. **Segmentation fault**
   - Check that `initialize()` is called before `step()`
   - Ensure configuration is valid
   - Reduce neuron count for testing

3. **Configuration file errors**
   - Use JSON format for complex configurations
   - Check syntax and indentation
   - Ensure all required parameters are present

4. **Performance issues**
   - Reduce neuron count
   - Use less frequent logging
   - Disable unnecessary plasticity

### Debugging

```python
import pynlm

# Enable verbose logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create configuration and check for errors
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Smaller for debugging

brain = pynlm.createBrain(config)
if brain.initialize():
    print("Brain initialized successfully")
    for step in range(10):
        brain.step(step)
else:
    print("Brain initialization failed")
```

## API Reference

For complete API documentation, refer to the pybind11 module documentation and the source code headers. The following is a summary of the most important classes:

- **Brain**: Main neural simulation engine
- **Config**: Configuration management
- **AgentBrain**: Agent interface
- **SimpleWorld**: Environment simulation
- **Vision**, **Audio**, **InternalSignals**: Sensory input types
- **Action**: Motor output representation

## Examples Directory

The `examples/` directory contains additional example scripts:

- `nlm_python_examples.py` - Comprehensive examples (this file)
- Other examples for specific use cases

## Future Improvements

The NLM Python API continues to evolve. Future improvements include:

- **Neural network layers**: Predefined network architectures
- **Transfer learning**: Save and load trained behaviors
- **Multi-agent systems**: Coordination between multiple agents
- **Real-time visualization**: Live plotting of neural activity
- **Web interface**: Browser-based interaction
- **Reinforcement learning integration**: Better reward systems

## License

NLM is licensed under the MIT License. See LICENSE file for details.