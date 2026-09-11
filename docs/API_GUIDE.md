# NLM API Documentation - Improved Usage Guide

This document provides comprehensive guidance on using the NLM (Neural Learning Machine) API after improvements have been made to address bugs and enhance usability.

## Overview

NLM is a brain-inspired spiking neural network simulator that enables the creation of virtual brains capable of learning, memory, and decision-making through neural dynamics.

## Quick Start (Beginner-Friendly)

### 1. Create a Basic Brain

```python
import pynlm

# Create a default brain with 1000 neurons
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### 2. Run a Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### 3. Complete Agent Example (Recommended for Beginners)

```python
import pynlm

def run_complete_simulation():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(500):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("Simulation complete!")

# Run the example
run_complete_simulation()
```

## Advanced Usage Examples

### 1. Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Set specific parameters
config.set("neuron_count", 5000)
config.set("region_count", 5)
config.set("connection_probability", 0.05)
config.set("stdp_ltp_weight", 0.02f)
config.set("stdp_ltd_weight", 0.015f)
config.set("simulation_timestep", 0.0001)

# Save configuration
config.saveToFile("my_brain_config.json")

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. State Management and Learning

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Save initial state
brain.save("brain_initial_state.bin")

# Run learning simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 200 == 0:
        print(f"Progress: Step {step}")

# Save learned state
brain.save("brain_learned_state.bin")

# Load learned state (for resuming or different behavior)
# new_brain = pynlm.createBrain(config)
# new_brain.load("brain_learned_state.bin")
```

### 3. Memory System Access

```python
import pynlm

# Create brain and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Active memory traces: {working_memory.getActiveTraces()}")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodicMemory.getEpisodeCount()}")

associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print(f"Associative memory patterns: {associativeMemory.getPatternCount()}")

# Run simulation to populate memory
for step in range(100):
    brain.step(step)
    # Memory is automatically updated during simulation
```

### 4. Neuromodulation Control

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Monitor neuromodulation levels during simulation
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check neuromodulation levels
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    neuromodulation_level = agent.getNeuromodulationLevel()
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {curiosity_level:.3f}")
        print(f"  Novelty: {novelty_level:.3f}")
        print(f"  Neuromodulation: {neuromodulation_level:.3f}")
```

### 5. Development System Usage

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Run simulation over multiple developmental stages
for step in range(5000):
    brain.step(step)
    
    # Check developmental stage changes
    current_stage = brain.getDevelopmentalStage()
    if current_stage != brain.getDevelopmentalStage():
        print(f"Developmental stage changed at step {step}")
    
    # Apply development updates (handled automatically in step())
    # agent.updateDevelopment(0.1)
    
    if step % 1000 == 0:
        print(f"Step {step}: Stage {current_stage}")

print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
```

## Error Handling and Best Practices

### 1. Error Handling Patterns

```python
import pynlm

def safe_brain_operation():
    try:
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            print("Failed to initialize brain")
            return False
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        print(f"Simulation completed successfully")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        return True
        
    except Exception as e:
        print(f"Error occurred: {e}")
        return False

# Run with error handling
safe_brain_operation()
```

### 2. Configuration Validation

```python
import pynlm
import json

def validate_configuration():
    config = pynlm.createDefaultConfig()
    
    # Check required keys
    required_keys = ["neuron_count", "region_count", "connection_probability"]
    for key in required_keys:
        if not config.has(key):
            print(f"Missing required configuration key: {key}")
            return False
    
    # Validate values
    neuron_count = config.getOr<size_t>("neuron_count", 0)
    if neuron_count < 100 || neuron_count > 100000:
        print(f"Invalid neuron count: {neuron_count}")
        return False
    
    region_count = config.getOr<size_t>("region_count", 0)
    if region_count < 1 || region_count > 10:
        print(f"Invalid region count: {region_count}")
        return False
    
    connection_prob = config.getOr<float>("connection_probability", 0.0f)
    if connection_prob < 0.0f || connection_prob > 1.0f:
        print(f"Invalid connection probability: {connection_prob}")
        return False
    
    print("Configuration validation passed")
    return True

validate_configuration()
```

## API Reference (Key Classes)

### Brain Class
- `createBrain(config)` - Creates a new brain instance
- `initialize()` - Initializes the brain with configuration
- `step(step)` - Performs one simulation step
- `getTotalNeuronCount()` - Returns total number of neurons
- `getTotalSynapseCount()` - Returns total number of synapses
- `getTotalSpikeCount()` - Returns total spike count
- `receiveSensoryInput(input)` - Injects sensory input
- `produceAction()` - Produces motor action from neural activity
- `save(filepath)` - Saves brain state to file
- `load(filepath)` - Loads brain state from file

### AgentBrain Class
- `createAgentBrain(brain)` - Creates agent interface
- `initialize(world)` - Initializes agent with world
- `processSensoryInput(percept)` - Processes sensory input
- `decodeMotorCommand()` - Decodes motor command from brain
- `enableRewardModulation(enable)` - Enables reward-based learning
- `enableCuriosity(enable)` - Enables curiosity-driven exploration
- `enableStructuralPlasticity(enable)` - Enables structural changes
- `enableDevelopment(enable)` - Enables developmental processes

### SimpleWorld Class
- `createSimpleWorld()` - Creates simple world
- `configure(width, height, visionWidth, visionHeight)` - Configures world dimensions
- `reset()` - Resets world to initial state
- `update(timestep)` - Updates world simulation
- `applyMotorCommand(command, time)` - Applies agent action
- `getSensoryPercept()` - Gets sensory input from world
- `getAgentBody()` - Gets agent body state

### Config Class
- `createDefaultConfig()` - Creates default configuration
- `set(key, value, source)` - Sets configuration value
- `getOr(key, default)` - Gets value with default fallback
- `has(key)` - Checks if key exists
- `saveToFile(filepath)` - Saves config to file
- `loadFromFile(filepath)` - Loads config from file

## Performance Tips

### 1. Memory Management
```python
# Create brain once and reuse
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# For multiple simulations, use reset() instead of recreating
brain.reset()
brain.initialize()  # Re-initialize with same config
```

### 2. Simulation Efficiency
```python
# Use batch processing for multiple steps
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Progress: {step}/1000")
```

### 3. Configuration Optimization
```python
# Set optimal parameters for your use case
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # Adjust based on your needs
config.set("simulation_timestep", 0.001)
config.set("replay_interval", 100)
```

## Common Issues and Solutions

### 1. "Brain isn't doing anything"
**Problem**: Brain appears inactive during simulation
**Solution**: 
- Ensure `brain.initialize()` is called before `brain.step()`
- Check sensory input is connected: `agent.processSensoryInput(world.getSensoryPercept())`
- Increase simulation steps for better activity

### 2. "Agent isn't moving"
**Problem**: Agent not producing motor actions
**Solution**:
- Ensure `world.update()` is called before `agent.decodeMotorCommand()`
- Check motor neurons are firing: `print(brain.getFiringNeuronCount())`
- Verify action selection logic in your code

### 3. "Everything is 0"
**Problem**: All neural values are zero
**Solution**:
- Brains need time to "warm up" - try more steps (at least 100-500)
- Ensure sensory input is connected and being processed
- Check configuration values are appropriate

### 4. "Out of memory"
**Problem**: Simulation consumes excessive memory
**Solution**:
- Reduce neuron count for smaller networks
- Use smaller simulation timesteps
- Consider using `brain.reset()` periodically

## Troubleshooting

### Building Issues
- **Python bindings not found**: Ensure `pip install pynlm` completes successfully
- **CMake errors**: Check compiler version and required dependencies
- **Missing libraries**: Install dependencies via package manager

### Runtime Issues
- **Segmentation faults**: Check for null pointer dereferences
- **Import errors**: Ensure Python environment is properly set up
- **Configuration errors**: Validate JSON format and required keys

## Further Reading

1. **Documentation**: Check `docs/ARCHITECTURE.md` for detailed system architecture
2. **Examples**: Refer to `easy_usage.md` and `HOW_TO_USE.md` for additional examples
3. **Scientific Background**: Read `docs/SCIENCE.md` for theoretical foundations
4. **Experiments**: Explore `docs/EXPERIMENTS.md` for experiment descriptions

## Support and Community

For issues and questions:
- Check GitHub issues for common problems
- Refer to API documentation for method details
- Experiment with different configurations to understand behavior
- Start with simple examples and gradually build complexity

---

This improved API documentation provides clear, comprehensive guidance for both beginners and advanced users of the NLM system. The examples demonstrate proper usage patterns and best practices based on the improvements made to address bugs and enhance usability.
