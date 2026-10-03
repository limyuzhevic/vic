# NLM Python Bindings - Quick Start Guide

## Overview

Welcome to the enhanced NLM (Neural Learning Machine) Python bindings!

This guide provides a quick introduction to the enhanced API, which offers:
- Improved Pythonic interface with type safety
- Comprehensive error handling and validation
- Convenience methods for common operations
- State conversion utilities
- Better documentation and examples
- Integration with all NLM subsystems

## Installation

### Quick Installation

```bash
# Install the package
pip install pynlm
```

### Development Installation

```bash
# Clone the repository and install in development mode
cd /path/to/nlm
pip install -e .
```

### Build from Source

```bash
# Build the C++ components
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .
```

## First Example: Simple Brain Simulation

Let's create a simple neural simulation step by step.

### Step 1: Import the Module

```python
import pynlm
```

### Step 2: Create and Initialize a Brain

```python
# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Step 3: Run a Simulation

```python
# Run 100 simulation steps
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Example

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

## Second Example: Complete Agent with World

This example shows how to create an agent that interacts with a simulated environment.

### Step 1: Create All Components

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create world environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent with world
agent.initialize(world)
```

### Step 2: Enable Learning Subsystems

```python
# Enable all learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

### Step 3: Run the Simulation Loop

```python
# Run simulation for 500 steps
for step in range(500):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get what the agent sees
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks
    brain.step(step)
    
    # 5. Get action from brain
    action = agent.decodeMotorCommand()
    
    # 6. Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Apply reward modulation
    if agent.isRewardModulationEnabled():
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
```

### Complete Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
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

# Run the simulation
run_agent_simulation(1000)
```

## Key Features

### 1. Enhanced Configuration Management

```python
import pynlm

# Create default config
config = pynlm.createDefaultConfig()

# Set values with type safety
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)

# Get values with validation
try:
    neuron_count = config.getInt("brain.neuron_count")
    print(f"Neuron count: {neuron_count}")
except pynlm.InvalidArgument as e:
    print(f"Error: {e}")
```

### 2. Brain State Inspection

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few steps
for i in range(10):
    brain.step(i)

# Get brain statistics
brain_state = brain.toDict()
print("Brain statistics:")
for key, value in brain_state.items():
    print(f"  {key}: {value}")
```

### 3. Agent State Management

```python
import pynlm

# Create agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Get agent state
agent_state = agent.toDict()
print("Agent statistics:")
for key, value in agent_state.items():
    print(f"  {key}: {value}")
```

### 4. Error Handling

```python
import pynlm

# Create brain and config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
config = brain.getConfig()

# Test error handling
try:
    # This will throw an error
    invalid_value = config.getInt("nonexistent_key")
except pynlm.InvalidArgument as e:
    print(f"Caught error: {e}")
```

### 5. Convenience Functions

```python
import pynlm

# Run complete episode in one call
pynlm.run_simulation(brain, world, agent, 1000)

# Create default config with custom parameters
config = pynlm.createDefaultConfig(
    neuron_count=2000,
    connection_probability=0.05
)
```

## API Reference

### Core Classes

#### Brain
- **`createBrain(config)`**: Create a new brain with configuration
- **`initialize()`**: Initialize the brain with configuration
- **`step(step, time)`**: Perform a simulation step
- **`to_dict()`**: Convert brain state to dictionary
- **`getTotalNeuronCount()`**: Get total neuron count
- **`getTotalSpikeCount()`**: Get total spike count
- **`getFiringNeuronCount()`**: Get count of currently firing neurons

#### Config
- **`createDefaultConfig()`**: Create a default configuration
- **`createDefaultConfig(neuron_count, connection_probability)`**: Create with custom parameters
- **`has(key)`**: Check if configuration key exists
- **`getInt(key)`, `getFloat(key)`, `getBool(key)`, `getString(key)`**: Type-safe getters
- **`set(key, value)`**: Set configuration value
- **`loadFromFile(filepath)`, `saveToFile(filepath)`**: File operations

#### AgentBrain
- **`createAgentBrain(brain)`**: Create agent brain interface
- **`initialize(world)`**: Initialize with world environment
- **`processSensoryInput(percept)`**: Process sensory input
- **`decodeMotorCommand()`**: Decode motor command
- **`applyRewardModulation(reward, predicted_reward)`**: Apply reward modulation
- **`to_dict()`**: Convert agent state to dictionary

#### SimpleWorld
- **`createSimpleWorld()`**: Create a new world
- **`configure(width, height, visionWidth, visionHeight)`**: Configure world dimensions
- **`reset()`**: Reset world to initial state
- **`update(timestep)`**: Update world physics
- **`applyMotorCommand(cmd, currentTime)`**: Apply motor command
- **`getSensoryPercept()`**: Get sensory percept

### Enumerations

- **`NeuronType`**: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- **`DevelopmentalStage`**: Initial, CriticalPeriod, Maturation, Adult, Aging
- **`ActionType`**: Movement, interaction, consumption, etc.
- **`WorldObjectType`**: Empty, Resource, Hazard, Wall, Marker

## Advanced Features

### 1. Memory Systems Access

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
```

### 2. Plasticity Systems Access

```python
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()
```

### 3. Neuromodulation

```python
agent = pynlm.createAgentBrain(brain)

print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

### 4. State Conversion

```python
import pynlm

# Convert percept to dictionary
percept = world.getSensoryPercept()
percept_dict = percept.toDict()

# Create percept from dictionary
restored_percept = pynlm.SensoryPercept.fromDict(percept_dict)

# Convert action to dictionary
action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.5])
action_dict = action.toDict()
```

## Examples Directory

Run the examples to see the API in action:

```bash
python python/nlm_examples.py
```

This will run 10 comprehensive examples covering:
- Basic brain simulation
- Complete agent with world
- Custom configuration
- Memory systems
- Plasticity systems
- Neuromodulation
- State conversion
- Performance benchmarks
- Error handling
- Advanced features

## Quick Commands

### Common Operations

```bash
# Install package
pip install pynlm

# Run examples
python python/nlm_examples.py

# Check documentation
pydoc pynlm

# Get help
python -c "import pynlm; help(pynlm)"
```

### Development

```bash
# Install in development mode
pip install -e .

# Build C++ components
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Support

### Getting Help

1. **Documentation**: Check the `python/README.md` file
2. **Examples**: Run `python python/nlm_examples.py`
3. **GitHub Issues**: Report bugs and feature requests
4. **Community**: Join the NLM community for support

### Common Issues

1. **ImportError**: Ensure package is properly installed
   ```bash
   pip install --force-reinstall .
   ```

2. **Segmentation Fault**: Ensure proper initialization order
   ```python
   brain.initialize()  # Must be called before step()
   brain.reset()      # Must be called before re-running
   ```

3. **Configuration Errors**: Check key spelling and types
   ```python
   if config.has("brain.neuron_count"):
       value = config.getInt("brain.neuron_count")
   ```

## Future Improvements

The enhanced NLM Python API continues to evolve. Future improvements may include:

1. **Type Hints**: Full type hints for better IDE support
2. **Async Support**: Asynchronous simulation capabilities
3. **Visualization**: Built-in plotting and visualization
4. **Distributed Computing**: Multi-process simulation support
5. **Advanced Interfaces**: High-level APIs for common use cases

## License

This enhanced NLM Python API is licensed under the MIT License.

Enjoy exploring the neural simulation framework with the enhanced Python bindings!