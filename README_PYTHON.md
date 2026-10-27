# Python Interface Documentation for NLM

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that provides a Python interface for creating and simulating neural systems. The Python bindings are available through the `pynlm` package and provide access to advanced neural simulation capabilities including memory systems, neuromodulation, prediction, and learning.

## Installation

### Prerequisites
- Python 3.8 or higher
- CMake 3.16 or higher (for building from source)

### Installing from pip
```bash
pip install pynlm
```

### Building from source
```bash
# Clone the repository
cd /path/to/nlm

# Build the Python bindings
python -m pip install .
```

## Basic Usage

### Quick Start

The quickest way to start using NLM is to create a simple brain and simulate it:

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Basic Agent-World Interaction

A more practical example shows how to use an agent in a world:

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create and initialize agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input through agent
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show progress every 20 steps
    if step % 20 == 0:
        stats = agent.get_stats()
        print(f"Step {step}: Action={action}, Neuromod={stats[0]:.3f}, Curiosity={stats[1]:.3f}")

print("Simulation finished!")
```

## Core Classes

### pynlm.Brain
Represents a complete neural brain with all integrated systems.

**Key Methods:**
- `initialize()`: Initialize the brain with configuration
- `step(step_number)`: Perform one simulation step
- `receiveSensoryInput(percept)`: Inject sensory data
- `produceAction()`: Generate motor output
- `getTotalSpikeCount()`: Get total spike count
- `getFiringNeuronCount()`: Get count of firing neurons
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

**Properties:**
- `config`: Configuration object
- `total_neurons`: Total number of neurons
- `total_synapses`: Total number of synapses

### pynlm.AgentBrain
Connects a brain to a world, handling sensory transduction and motor decoding.

**Key Methods:**
- `initialize(world)`: Set up agent with world
- `processSensoryInput(percept)`: Convert world percept to brain input
- `decodeMotorCommand()`: Convert brain activity to actions
- `applyRewardModulation(reward, predicted_reward)`: Apply learning signals
- `updateDevelopment(timestep)`: Update developmental stage

**Convenience Methods:**
- `simulate_step(world, timestep)`: Update world, process input, get action
- `simulate_step_with_stats(world, timestep)`: Simulate with statistics
- `get_stats()`: Get current agent statistics
- `enable_all_features()`: Enable all advanced features

### pynlm.SimpleWorld
A 2D simulation environment for agents.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)`: Set world dimensions
- `reset()`: Reset world state
- `update(timestep)`: Advance world simulation
- `applyMotorCommand(action, time)`: Apply agent action
- `getSensoryPercept()`: Get sensory input for agent
- `getAgentBody()`: Get agent position and state

### pynlm.Config
Configuration system for customizing brain parameters.

**Key Methods:**
- `loadFromFile(filepath)`: Load configuration from file
- `loadFromArgs(argc, argv)`: Load from command line
- `has(key)`: Check if key exists
- `getOr(key, default_value)`: Get value with default

## Advanced Usage

### Working with Different Sensor Modalities

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

# Add objects to interact with
agent_body = world.getAgentBody()
world.addObject(pynlm.WorldObject(agent_body.x + 5, agent_body.y + 5, 
                                 pynlm.WorldObjectType.Resource, 1.0, 0.5))

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all sensory processing
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print(f"Expected sensory input size: {agent.getSensoryInputSize()}")
print(f"Expected motor output size: {agent.getMotorOutputSize()}")
```

### Learning and Development

```python
import pynlm

# Create developmental brain
config = pynlm.createDefaultConfig()
# Configure for developmental learning
config.set("enable_development", True)
config.set("plasticity_rate", 0.8)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Track developmental stages over time
for episode in range(3):
    agent.reset()
    agent.enableDevelopment(True)
    
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    stage = agent.getDevelopmentalStage()
    plasticity = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    
    print(f"Episode {episode + 1}: Stage={stage}, Plasticity={plasticity:.3f}, Curiosity={curiosity:.3f}")
```

### Performance Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Performance monitoring
start_time = time.time()
steps_completed = 0
spike_threshold_reached = False

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    steps_completed += 1
    
    # Monitor for interesting events
    if brain.getTotalSpikeCount() > 1000 and not spike_threshold_reached:
        spike_threshold_reached = True
        print(f"Reached 1000 spikes at step {step}")

elapsed = time.time() - start_time
print(f"Performance: {steps_completed/elapsed:.1f} steps/sec")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Multi-Agent Scenarios

```python
import pynlm
import random

# Create shared brain configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create multiple agents sharing the same brain
agents = []
worlds = []
for i in range(3):
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.setAgentStart(random.uniform(5, 45), random.uniform(5, 45))
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agents.append(agent)
    worlds.append(world)

# Run competition simulation
for step in range(200):
    for i, (agent, world) in enumerate(zip(agents, worlds)):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: Brains have {brain.getTotalNeuronCount()} neurons")
```

## Tutorials

### Tutorial 1: Getting Started
**Objective:** Create and simulate a simple brain

1. **Import the library:**
   ```python
   import pynlm
   ```

2. **Create a brain:**
   ```python
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   ```

3. **Initialize the brain:**
   ```python
   brain.initialize()
   ```

4. **Run a simulation:**
   ```python
   for i in range(100):
       brain.step(i)
   ```

5. **Monitor results:**
   ```python
   print(f"Total spikes: {brain.getTotalSpikeCount()}")
   print(f"Firing neurons: {brain.getFiringNeuronCount()}")
   ```

### Tutorial 2: Agent in Environment
**Objective:** Create an agent that interacts with a world

1. **Set up world:**
   ```python
   world = pynlm.createSimpleWorld()
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   ```

2. **Create agent:**
   ```python
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   ```

3. **Enable learning features:**
   ```python
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   ```

4. **Run interaction loop:**
   ```python
   for step in range(100):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

### Tutorial 3: Advanced Simulation
**Objective:** Create a complex learning scenario

1. **Configure for developmental learning:**
   ```python
   config = pynlm.createDefaultConfig()
   config.set("plasticity_rate", 1.0)
   config.set("enable_structural_plasticity", True)
   ```

2. **Create agent with all features:**
   ```python
   agent.enable_all_features()
   ```

3. **Track learning progress:**
   ```python
   for episode in range(10):
       agent.reset()
       # ... run episode ...
       stats = agent.get_stats()
       print(f"Episode {episode}: Neuromod={stats[0]:.3f}, Curiosity={stats[1]:.3f}")
   ```

## API Reference

### Class: Brain
- `__init__(config)`: Create brain with configuration
- `initialize()`: Initialize brain
- `step(step)`: Perform simulation step
- `step(step, time)`: Perform simulation step with time
- `receiveSensoryInput(percept)`: Inject sensory input
- `injectCurrent(neuron_id, current)`: Inject current into neuron
- `injectCurrentToNeurons(neuron_type, current)`: Inject to all neurons of type
- `produceAction()`: Generate motor action
- `applyNeuromodulation(signal)`: Apply neuromodulator
- `updatePlasticity()`: Update plasticity rules
- `develop()`: Apply developmental changes
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- All property getters (getTotalNeuronCount, getTotalSynapseCount, etc.)

### Class: AgentBrain
- `__init__(brain)`: Create agent with brain
- `initialize(world)`: Set up agent
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward
- `updateDevelopment(timestep)`: Update development
- `get_stats()`: Get agent statistics
- `simulate_step(world, timestep)`: Convenience simulation
- `simulate_step_with_stats(world, timestep)`: Simulation with stats
- `enable_all_features()`: Enable all features

### Class: SimpleWorld
- `__init__()`: Create world
- `configure(width, height, visionWidth, visionHeight)`: Set dimensions
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world
- `applyMotorCommand(action, time)`: Apply action
- `getSensoryPercept()`: Get sensory input
- `getAgentBody()`: Get agent state
- `addObject(object)`: Add world object
- `removeObject(x, y)`: Remove object

### Class: Config
- `__init__()`: Create empty config
- `loadFromFile(filepath)`: Load from file
- `loadFromArgs(argc, argv)`: Load from args
- `has(key)`: Check key existence
- `getOr(key, default)`: Get value with default
- `set(key, value)`: Set value

## Configuration Options

Common configuration keys:
- `random_seed`: Random seed for reproducibility
- `simulation_timestep`: Simulation time step (seconds)
- `neuron_count`: Number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Synaptic connection probability
- `enable_development`: Enable developmental processes
- `enable_structural_plasticity`: Enable structural changes
- `enable_reward_modulation`: Enable reward learning
- `enable_curiosity`: Enable curiosity-driven exploration
- `plasticity_rate`: Base plasticity rate
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

## Troubleshooting

### Common Issues and Solutions

**Issue:** "My brain isn't doing anything"
**Solution:**
- Ensure `brain.initialize()` is called before stepping
- Try increasing the number of steps
- Check if neurons have sufficient input

**Issue:** "The agent isn't moving"
**Solution:**
- Ensure `world.update()` is being called
- Check that `agent.decodeMotorCommand()` returns valid actions
- Verify motor neuron groups are properly initialized

**Issue:** "Everything is 0"
**Solution:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if development has started to create connectivity

### Debugging Tips

1. **Enable verbose logging:**
   ```python
   import pynlm
   # Configure logger to show debug messages
   ```

2. **Check neuron states:**
   ```python
   for neuron in brain.getAllNeurons():
       print(f"Neuron {neuron.getId()}: V={neuron.getMembranePotential()}")
   ```

3. **Monitor spike activity:**
   ```python
   print(f"Total spikes: {brain.getTotalSpikeCount()}")
   print(f"Firing neurons: {brain.getFiringNeuronCount()}")
   ```

## Performance Considerations

### Simulation Speed
- Larger neuron counts = slower simulation
- Complex world geometries = slower updates
- High-frequency sensory input = more computation

### Memory Usage
- Brain memory scales with neuron count
- World memory scales with area resolution
- Configuration files can become large with many parameters

### Optimization Tips
1. Use appropriate world sizes for your simulation
2. Configure neurons and synapses for your task complexity
3. Use checkpointing for long-running simulations
4. Profile your specific use case

## Advanced Features

### Custom Sensory Inputs
```python
# Create custom sensory input
vision = pynlm.Vision(16, 16, 3)
data = [random.random() for _ in range(16 * 16 * 3)]
vision.setData(data)

# Create percept and process
percept = pynlm.SensoryPercept()
percept.setVision(vision)
agent.processSensoryInput(percept)
```

### Motor Command Customization
```python
# Custom action with parameters
action = pynlm.Action(pynlm.ActionType.Eat, [0.5, 0.3, 0.8])
world.applyMotorCommand(action, time)
```

### Parallel Processing
```python
# For high-performance computing
import concurrent.futures

# Run multiple simulations in parallel
with concurrent.futures.ThreadPoolExecutor() as executor:
    futures = [executor.submit(run_simulation, config) for _ in range(10)]
    results = [future.result() for future in concurrent.futures.as_completed(futures)]
```

## License and Credits

This project is part of the NLM (Neural Learning Machine) research project.

For more information, visit:
- Documentation: https://nlm.readthedocs.io
- GitHub: https://github.com/nlm-project/nlm
- Research papers: Available on the project website

## Support

For questions and support:
1. Check the documentation at https://nlm.readthedocs.io
2. Search for existing issues in the GitHub repository
3. File new issues with detailed error messages and reproduction steps
4. Consult the NLM research team for academic support