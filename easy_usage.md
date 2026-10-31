# NLM - Comprehensive Usage Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. This guide provides comprehensive examples of how to use the enhanced NLM Python API with improved error handling, context managers, batch operations, and configuration management.

## New API Features

The enhanced NLM API includes:

- **Pythonic Configuration Management**: Chainable setters with validation
- **Context Managers**: Automatic resource management for simulation episodes
- **Error Handling**: Custom exceptions with detailed error messages
- **Batch Operations**: Efficient batch processing for simulation runs
- **Type Safety**: Comprehensive type checking and conversion
- **Logging and Monitoring**: Enhanced logging with performance metrics

## Basic Usage

### Creating a Simple Brain

```python
import pynlm

# Method 1: Using default configuration (recommended)
brain = pynlm.createBrain()
brain.initialize()

# Method 2: Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getNeuronCount()} neurons")
print(f"Brain name: {brain.getName()}")
```

### Running a Simulation

```python
import pynlm

# Create brain and run simple simulation
brain = pynlm.createBrain()
brain.initialize()

# Run 500 simulation steps with progress reporting
for step in range(500):
    brain.step(step)
    
    # Report progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getTotalSpikeCount()} total spikes")

# Get final statistics
print(brain.getStatusReport())
```

## Enhanced Configuration Management

### Pythonic Configuration API

```python
import pynlm

# Create configuration with method chaining
config = pynlm.createDefaultConfig() \
    .set("brain.neuron_count", 1500) \
    .set("brain.synapse_density", 0.12) \
    .set("plasticity.stdp.enable", True) \
    .set("plasticity.hebbian.enable", True) \
    .set("neuromod.curiosity.enable", True) \
    .set("neuromod.novelty.enable", True)

# Configure from dictionary
config_dict = {
    "brain.neuron_count": 2000,
    "simulation_timestep": 0.0005,
    "log.level": "DEBUG",
    "environment.width": 30,
    "environment.height": 30
}

config = pynlm.createDefaultConfig()
config.configureFromDict(config_dict)

# Load and save configurations
config.loadFromFile("my_config.cfg")
config.saveToFile("backup_config.cfg")

# Validate configuration
if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration has validation errors")

# Get configuration values
neuron_count = config.getInt("brain.neuron_count")
synapse_density = config.getFloat("brain.synapse_density")
log_level = config.getString("log.level")
```

## Agent-Based Simulation

### Complete Agent Simulation with Context Manager

```python
import pynlm

# Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createWorld()
agent = pynlm.createAgentBrain(brain)

# Configure the world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

# Initialize components
brain.initialize()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("=== Agent Simulation Started ===")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Brain neurons: {brain.getNeuronCount()}")
print(f"Agent curiosity enabled: {agent.isCuriosityEnabled()}")

# Run simulation with context manager for automatic cleanup
with pynlm.createEpisode(brain, agent, world, episode_length=1000, timestep=0.1) as episode:
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and thinks
        brain.step(step, step * 0.1)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Report progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Neurons: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")

print("=== Simulation Episode Completed ===")
print(episode.getStatistics())
```

## Advanced Features

### Batch Simulation Processing

```python
import pynlm
import time

# Create simulation components
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
brain = pynlm.createBrain(config)
world = pynlm.createWorld()
agent = pynlm.createAgentBrain(brain)

world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.setAgentStart(7.5, 7.5)

brain.initialize()
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Running batch simulation...")
start_time = time.time()

# Run multiple episodes
results = pynlm.runBatchSimulation(
    brain=brain,
    agent=agent,
    world=world,
    num_episodes=10,
    episode_length=500,
    timestep=0.1
)

end_time = time.time()
print(f"\nBatch simulation completed in {end_time - start_time:.2f} seconds")
print(f"Results from {len(results)} episodes:")

for i, result in enumerate(results):
    print(f"\nEpisode {i}:")
    print(result)
```

### Error Handling and Validation

```python
import pynlm

print("=== Error Handling Examples ===")

# Example 1: Configuration validation
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -100)  # Invalid value
    print("Configuration validation should have failed")
except pynlm.ConfigurationError as e:
    print(f"Caught configuration error: {e}")

# Example 2: Simulation error handling
try:
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Simulate an error
    for step in range(10):
        brain.step(step)
    
    # Try to step with invalid parameters
    brain.step(-1)  # Should be caught and handled
    
except pynlm.SimulationError as e:
    print(f"Caught simulation error: {e}")
except Exception as e:
    print(f"Caught unexpected error: {e}")

# Example 3: Context manager error handling
try:
    brain = pynlm.createBrain()
    world = pynlm.createWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize components
    brain.initialize()
    agent.initialize(world)
    
    # Create and run episode with error handling
    episode = pynlm.createEpisode(brain, agent, world, 100, 0.1)
    episode.enter()
    
    # Simulate error during episode
    for step in range(50):
        world.update(0.1)
        # Simulate error
        if step == 25:
            raise pynlm.SimulationError("Simulated error during episode")
        
    episode.run()
    
except pynlm.SimulationError as e:
    print(f"Episode simulation error: {e}")
finally:
    if 'episode' in locals():
        episode.exit(None)
```

### Memory and Performance Monitoring

```python
import pynlm
import time

# Create a larger brain for performance testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
world = pynlm.createWorld()
agent = pynlm.createAgentBrain(brain)

print("=== Performance Monitoring ===")
brain.initialize()
agent.initialize(world)

# Memory usage tracking
start_memory = brain.getNeuronCount()
start_time = time.time()

# Run intensive simulation
for step in range(2000):
    brain.step(step, step * 0.001)
    
    # Monitor every 200 steps
    if step % 200 == 0:
        current_memory = brain.getNeuronCount()
        current_time = time.time() - start_time
        spikes_per_second = brain.getTotalSpikeCount() / current_time
        
        print(f"Step {step}: "
              f"Memory: {current_memory - start_memory} neurons allocated, "
              f"Time: {current_time:.2f}s, "
              f"Spikes/sec: {spikes_per_second:.0f}")

# Get final performance statistics
print(f"\nFinal Performance Statistics:")
print(f"Total simulation time: {time.time() - start_time:.2f} seconds")
print(f"Steps per second: {2000 / (time.time() - start_time):.0f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get detailed status report
print(f"\nDetailed Status Report:")
print(brain.getStatusReport())
```

### Development System Control

```python
import pynlm

# Create brain with development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Development System Control ===")
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Control developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"Changed to: {brain.getDevelopmentalStage()}")

# Run some steps to see development effects
for step in range(100):
    brain.step(step)

print(f"After development: {brain.getDevelopmentalStage()}")

# Access development system directly
development_system = brain.getDevelopmentSystem()
if development_system:
    print(f"Development system active: {development_system.isActive()}")
```

### Memory System Access

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Memory System Access ===")

# Access different memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory active: {working_memory.getActiveTraces()} traces")
    print(f"Working memory capacity: {working_memory.getCapacity()} items")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory: {episodic_memory.getEpisodeCount()} episodes")
    print(f"Episodic memory capacity: {episodic_memory.getCapacity()} items")

associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print(f"Associative memory: {associative_memory.getAssociationCount()} associations")

prediction_system = brain.getPredictionSystem()
if prediction_system:
    print(f"Prediction system: {prediction_system.getPredictionCount()} predictions")

# Use prediction system
if prediction_system:
    prediction = prediction_system.predictNextState()
    print(f"Predicted state: {prediction}")
```

## Advanced Configuration Examples

### Environment-Specific Configurations

```python
import pynlm

# Small robot simulation
config_small = pynlm.createDefaultConfig() \
    .set("brain.neuron_count", 500) \
    .set("environment.width", 5.0) \
    .set("environment.height", 5.0) \
    .set("simulation_timestep", 0.01) \
    .set("log.level", "WARNING")

# Large scale simulation  
config_large = pynlm.createDefaultConfig() \
    .set("brain.neuron_count", 10000) \
    .set("brain.synapse_density", 0.02) \
    .set("environment.width", 100.0) \
    .set("environment.height", 100.0) \
    .set("simulation_timestep", 0.001) \
    .set("log.level", "INFO") \
    .set("visualization.enabled", True)

# Learning-focused configuration
config_learning = pynlm.createDefaultConfig() \
    .set("plasticity.stdp.learning_rate", 0.05) \
    .set("plasticity.hebbian.enable", True) \
    .set("neuromod.curiosity.enable", True) \
    .set("neuromod.novelty.enable", True) \
    .set("neuromod.dopamine.scale", 2.0)

print("Configuration examples created successfully!")
```

## Troubleshooting

### Common Issues and Solutions

1. **Brain not initialized**
   ```python
   # Problem: Calling step() without initialization
   brain.step(0)  # Error!
   
   # Solution:
   brain.initialize()
   brain.step(0)
   ```

2. **Configuration validation errors**
   ```python
   # Problem: Invalid configuration values
   config.set("brain.neuron_count", -100)  # Error!
   
   # Solution:
   config.set("brain.neuron_count", 1000)
   ```

3. **Memory access after reset**
   ```python
   # Problem: Accessing brain state after reset
   brain.reset()
   print(brain.getTotalSpikeCount())  # Will be 0
   
   # Solution:
   brain.initialize()  # Reset and reinitialize
   ```

4. **Episode context manager errors**
   ```python
   # Problem: Not using context manager properly
   episode = pynlm.createEpisode(brain, agent, world, 100, 0.1)
   episode.run()  # Manual management
   
   # Solution: Use context manager
   with pynlm.createEpisode(brain, agent, world, 100, 0.1) as episode:
       episode.run()
   ```

## API Reference

### Core Classes

- **Config**: Configuration management with validation
- **Brain**: Neural simulation core
- **AgentBrain**: Agent-environment interface
- **SimpleWorld**: Simulation environment
- **Action**: Motor commands
- **SensoryPercept**: Sensory input data
- **AgentBody**: Agent state and position
- **WorldObject**: Objects in the world

### Exception Types

- **NLMError**: Base exception for all NLM errors
- **ConfigurationError**: Configuration-related errors
- **SimulationError**: Simulation-related errors
- **ValidationError**: Validation-related errors

### Development Stages

- **Initial**: Brain development begins
- **CriticalPeriod**: High plasticity, rapid learning
- **Maturation**: Stabilization of neural connections
- **Adult**: Adult neural properties
- **Aging**: Age-related changes

### Action Types

Common motor actions include:
- MoveForward, MoveBackward
- TurnLeft, TurnRight
- Look, LookUp, LookDown
- Interact, Eat, Drink
- Rest, Wait

## Performance Tips

1. **Use batch operations** for multiple simulation steps
2. **Pre-allocate larger brain** if running multiple episodes
3. **Adjust simulation timestep** based on precision needs
4. **Use logging levels** appropriately to reduce overhead
5. **Validate configurations** before running simulations

## Migration Guide

If upgrading from the original NLM API:

### Original API
```python
# Old way
brain = pynlm.createBrain()
config = pynlm.createDefaultConfig()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
```

### New API
```python
# New way (more Pythonic)
brain = pynlm.createBrain()
config = pynlm.createDefaultConfig()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createWorld()

# New features available
brain.setName("MyBrain")
with pynlm.createEpisode(brain, agent, world, 100, 0.1) as episode:
    episode.run()
```

## Conclusion

The enhanced NLM Python API provides a comprehensive, Pythonic interface for creating and running neural simulations. With improved error handling, context managers, batch operations, and comprehensive configuration management, users can now build sophisticated neural simulations with ease and confidence.

The enhanced API maintains full backward compatibility while adding powerful new features that make NLM more accessible to both beginners and advanced users.
