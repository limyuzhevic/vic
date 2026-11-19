# NLM (Neural Learning Machine) - Comprehensive User Guide

## Overview

NLM is an experimental artificial developmental brain simulator that implements real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons, event-driven spike propagation, and multiple plasticity mechanisms.

## Key Features

- **Real Neural Computation**: LIF neuron dynamics with spike-timing-dependent plasticity (STDP)
- **Integrated Brain Systems**: Memory, prediction, cognition, neuromodulation, and development
- **Python Bindings**: Complete Python API for easy use and integration
- **Phase 6 Integration**: All systems connected into a coherent artificial brain loop
- **Checkpoint/Restart**: Save and load brain state for persistent learning
- **Educational Tool**: Great for understanding brain-inspired computing

## Quick Start

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(1000):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
```

### Complete Agent Simulation

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(1000):
    world.update(0.1)  # Update world physics
    percept = world.getSensoryPercept()  # Get what agent sees
    agent.processSensoryInput(percept)  # Brain processes input
    brain.step(step)  # Brain computes
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action

    # Check reward for learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print("Agent simulation completed!")
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM parameters.

**Methods:**
- `Config()` - Create empty configuration
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `set(key, value)` - Set parameter value
- `get(key)` - Get parameter value
- `getOr(key, default)` - Get with default
- `has(key)` - Check if parameter exists
- `getKeys()` - Get all parameter names

**Common Parameters:**
- `neuron_count` (int): Number of neurons
- `region_count` (int): Number of brain regions
- `connection_probability` (float): Synapse connectivity density
- `stdp_ltp_weight` (float): STDP potentiation weight
- `stdp_ltd_weight` (float): STDP depression weight
- `stdp_tau` (float): STDP time constant

#### Brain
The central neural simulation brain.

**Initialization:**
- `brain = createBrain(config)` - Create brain
- `brain.initialize()` - Initialize with configuration

**Core Methods:**
- `brain.step(step)` - Perform simulation step
- `brain.step(step, time)` - Perform step with timestamp
- `brain.receiveSensoryInput(input)` - Inject sensory data
- `brain.produceAction()` - Get motor output
- `brain.reset()` - Reset to initial state
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state

**Accessors:**
- `brain.getTotalNeuronCount()` - Get neuron count
- `brain.getTotalSynapseCount()` - Get synapse count
- `brain.getFiringNeuronCount()` - Get currently firing neurons
- `brain.getAverageFiringRate()` - Get average firing rate (Hz)
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getWorkingMemory()` - Get working memory system
- `brain.getEpisodicMemory()` - Get episodic memory system
- `brain.getDopamine()` - Get dopamine neuromodulator
- `brain.getCuriosity()` - Get curiosity system

#### SimpleWorld
2D simulation world with physics.

**Configuration:**
- `world = createSimpleWorld()` - Create world
- `world.configure(width, height, visionWidth, visionHeight)` - Set dimensions
- `world.reset()` - Reset to initial state
- `world.setAgentStart(x, y)` - Set agent starting position

**World Methods:**
- `world.update(timestep)` - Update physics
- `world.applyMotorCommand(cmd, time)` - Apply agent action
- `world.getSensoryPercept()` - Get agent's sensory input
- `world.getAgentBody()` - Get agent state
- `world.addObject(obj)` - Add world object

#### AgentBrain
Bridge between brain and world interaction.

**Methods:**
- `agent = createAgentBrain(brain)` - Create agent interface
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process world input
- `agent.decodeMotorCommand()` - Convert brain activity to action
- `agent.applyRewardModulation(reward, predicted)` - Apply learning signal
- `agent.updateDevelopment(timestep)` - Update developmental state

**Learning Subsystem Control:**
- `agent.enableRewardModulation(True/False)` - Enable/disable reward learning
- `agent.enableCuriosity(True/False)` - Enable/disable curiosity-driven exploration
- `agent.enableDevelopment(True/False)` - Enable/disable developmental changes
- `agent.enableStructuralPlasticity(True/False)` - Enable/disable synapse growth

### Important Classes

#### NeuronType Enum
- `Excitatory`: Regular excitatory neurons
- `Inhibitory`: Inhibitory neurons (negative effect)
- `Sensory`: Sensory input neurons
- `Motor`: Motor output neurons
- `Internal`: Internal processing neurons
- `Modulatory`: Neuromodulation neurons

#### ActionType Enum
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Rotate left
- `TurnRight`: Rotate right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact with nearby object
- `Eat`: Consume resource
- `Drink`: Drink water
- `Rest`: Stop moving
- `Wait`: Do nothing

#### MotorCommand Enum
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact
- `Wait`: Wait (do nothing)

#### DevelopmentalStage Enum
- `Initial`: Initial developmental stage (high plasticity)
- `CriticalPeriod`: Critical period for learning
- `Maturation`: Maturation phase
- `Adult`: Adult with stable properties
- `Aging`: Aging effects

## Best Practices

### 1. Start with Default Configuration
Always use `createDefaultConfig()` as a starting point. This ensures sensible defaults that work well for most use cases.

### 2. Understand the Simulation Loop
The main simulation loop typically follows this pattern:
1. Update world physics
2. Get sensory input from world
3. Process sensory input in brain
4. Execute brain computation step
5. Decode motor output from brain
6. Apply action to world
7. Provide reward signal for learning
8. Update developmental state

### 3. Monitor Progress
Use the brain's statistics to monitor simulation progress:
```python
print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
      f"{brain.getAverageFiringRate():.1f} Hz average rate")
```

### 4. Handle Errors Gracefully
Always check that operations succeed and handle errors:
```python
if not brain.initialize():
    print("Failed to initialize brain")
    return 1
```

### 5. Use Appropriate Simulation Length
- Short simulations (100-1000 steps): Good for testing
- Medium simulations (1000-10000 steps): Good for learning behavior
- Long simulations (10000+ steps): Good for stable behavior and development

## Troubleshooting

### "My brain isn't doing anything"
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that sensory neurons have input
- Verify that motor neurons are producing output

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Ensure sensory input is connected to the brain
- Check that action effects are properly connected

### "Agent isn't moving"
- Ensure `world.applyMotorCommand()` is being called
- Check that `world.update()` is being called before applying actions
- Verify that motor commands are being decoded correctly

### "ImportError: No module named 'pynlm'"
```bash
pip install -e .
```

## Advanced Usage

### Running Multiple Brains
```python
brains = []
for i in range(10):
    config = pynlm.createDefaultConfig()
    config.set("random_seed", i)  # Different seeds for variety
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)
```

### Checkpoint-Based Learning
```python
# Save progress
brain.save("brain_checkpoint_1000.bin")

# Later, load the saved brain
brain2 = pynlm.createBrain(config)
brain2.initialize()
brain2.load("brain_checkpoint_1000.bin")
```

### Comparing Different Configurations
```python
configs = [
    {"neuron_count": 100, "connection_probability": 0.1},
    {"neuron_count": 500, "connection_probability": 0.05},
    {"neuron_count": 1000, "connection_probability": 0.15},
]

results = []
for params in configs:
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", params["neuron_count"])
    config.set("connection_probability", params["connection_probability"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
    
    results.append({
        "params": params,
        "spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "neurons_firing": brain.getFiringNeuronCount()
    })
```

## Example: Learning to Navigate a World

Here's a complete example showing how an NLM brain learns to navigate a world and collect resources while avoiding hazards:

```python
import pynlm

# Set up the environment
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(15.0, 15.0)

# Create brain and agent
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("connection_probability", 0.08)
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Initialize and enable learning
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning episode
for step in range(2000):
    # Update world physics
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input and get action
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide reward and update development
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 200 == 0:
        print(f"Step {step}: {agent.getCuriosityLevel():.2f} curiosity, "
              f"{brain.getAverageFiringRate():.1f} Hz firing rate, "
              f"energy: {world.getAgentBody().energy:.1f}")

print("Navigation learning completed!")
print(f"Final curiosity: {agent.getCuriosityLevel():.2f}")
print(f"Total learning steps: {step}")
```

## Performance Tips

### For Faster Simulations
- Use fewer neurons (100-500 instead of 1000+)
- Reduce connection probability (0.05-0.1 instead of 0.2)
- Run shorter simulations (1000 steps instead of 10000)
- Disable non-essential features if not needed

### For More Realistic Behavior
- Use larger neuron counts (500-2000)
- Increase connection probability (0.1-0.2)
- Enable all neuromodulation systems
- Run longer simulations for development and learning

## License

MIT License - See LICENSE file for details.

## Authors

NLM is a research project investigating computational brain-like systems. See docs/ for scientific background and implementation details.