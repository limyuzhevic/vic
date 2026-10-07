# NLM - Advanced Python Bindings Guide

This document provides comprehensive guidance for using the improved NLM Python bindings, including new features, best practices, and advanced usage examples.

## Overview

The NLM Python bindings now provide a much more Pythonic and feature-rich interface to the neural simulation framework. The new version includes:

- **Enhanced error handling** with custom exception types
- **Pythonic API design** with intuitive method names and convenience functions
- **Comprehensive documentation** with examples and usage patterns
- **Advanced features** for expert users
- **Context managers** for safe resource management
- **Batch processing utilities** for complex simulations
- **Better type safety** and validation

## Quick Start (Improved API)

### Basic Usage with Enhanced Features

```python
import pynlm
import numpy as np

# Create brain with sample configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

# Use context manager for safe operations
with pynlm.BrainLifetimeManager() as brain_manager:
    brain_manager.initialize(brain)
    
    # Run simulation with progress tracking
    for step in range(100):
        # Inject sensory input periodically
        if step % 10 == 0:
            vision = pynlm.Vision(8, 8, 3)
            brain.receiveSensoryInput(vision)
        
        # Run brain step
        brain.step(step)
        
        # Get action and process stats
        if step % 5 == 0:
            action = brain.produceAction()
            stats = brain.getStats()
            print(f"Step {step}: {stats['firing_count']} firing neurons")
            
            # Check if learning is active
            if config.has("plasticity.stdp.enable"):
                print(f"STDP enabled: {config.get('plasticity.stdp.enable')}")

print("Simulation complete!")
```

## Enhanced Configuration System

### Pythonic Configuration Access

```python
import pynlm

# Method 1: Traditional API (still supported)
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.curiosity.enable", True)

# Method 2: Pythonic dictionary-style access (NEW!)
config = pynlm.createDefaultConfig()
config["brain.neuron_count"] = 2000
config["neuromod.curiosity.enable"] = True

# Method 3: Get with default fallback (NEW!)
neuron_count = config.get("brain.neuron_count", 1000)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)

# Method 4: Check existence (NEW!)
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config['brain.neuron_count']}")

# Method 5: Validate configuration (NEW!)
valid, error_msg = pynlm.validateConfig(config)
if not valid:
    print(f"Config error: {error_msg}")

# Method 6: Create sample config (NEW!)
config = pynlm.createSampleConfig()
```

### Configuration Categories

```python
# Brain parameters
brain_config = {
    "neuron_count": 2000,
    "synapse_density": 0.1,
    "connection_probability": 0.05,
    "v_thresh": -50.0,
    "v_rest": -70.0
}

# Plasticity settings
plasticity_config = {
    "stdp": {"enable": True, "learning_rate": 0.001},
    "hebbian": {"enable": True},
    "structural": {"enable": True}
}

# Neuromodulation
neuromod_config = {
    "dopamine": {"scale": 1.0},
    "curiosity": {"enable": True},
    "novelty": {"enable": True}
}
```

## Enhanced Sensory Input System

### Vision Processing with Normalization

```python
import pynlm
import numpy as np

# Create and configure vision
vision = pynlm.Vision(80, 60, 3)  # 80x60 RGB image

# Generate sample pixel data (simulate camera input)
width, height, channels = vision.getShape()
num_pixels = width * height * channels
pixel_data = np.random.random(num_pixels).astype(np.float32)

# Set and normalize vision data
vision.setData(pixel_data)
vision.normalize()  # NEW: Normalize to [0, 1] range

# Access vision information
print(f"Vision shape: {vision.getShape()}")
print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")
```

### Audio Processing with NumPy Integration

```python
import pynlm
import numpy as np

# Create audio stream
audio = pynlm.Audio(44100, 4410)  # 1 second at 44.1kHz

# Generate sample audio data (sine wave)
sample_rate = audio.getSampleRate()
duration = audio.getDuration()
t = np.linspace(0, duration, sample_rate * duration, False)
audio_data = np.sin(2 * np.pi * 440 * t) * 0.5  # 440Hz sine wave

# Set audio data
audio.setData(audio_data)

# Convert to NumPy for signal processing
numpy_array = audio.to_numpy()

# Calculate audio statistics
mean_amp = np.mean(numpy_array)
max_amp = np.max(np.abs(numpy_array))
print(f"Audio: mean amplitude={mean_amp:.3f}, max amplitude={max_amp:.3f}")
```

### Internal Signals

```python
import pynlm

# Create internal signals (brain state)
internal = pynlm.InternalSignals()

# Set various brain states
internal.setSignal(0, 0.8)  # Hunger: 80%
internal.setSignal(1, 0.3)  # Thirst: 30%  
internal.setSignal(2, 0.9)  # Energy: 90%
internal.setSignal(3, 0.5)  # Arousal: 50%
internal.setSignal(4, 0.2)  # Stress: 20%

# Get all signals
signals = internal.getSignals()
print(f"Internal signals: {signals}")

# Check specific signal
hunger = internal.getSignal(0)
print(f"Hunger level: {hunger:.1%}")
```

## Enhanced Action System

### Advanced Action Creation

```python
import pynlm

# Create simple action
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action: {action1.getName()}")

# Create action with parameters (target position)
action2 = pynlm.Action(pynlm.ActionType.Interact, [1.5, 2.0, 0.0])
print(f"Action X position: {action2.getX()}")
print(f"Action Y position: {action2.getY()}")
print(f"Action Z position: {action2.getZ()}")

# Check action type
if action1.isMovement():
    print("This is a movement action")
elif action1.isInteraction():
    print("This is an interaction action")

# Clone actions (NEW!)
action_clone = action1.clone()
print(f"Cloned action type: {action_clone.getType()}")

# Get all action types
action_types = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat,
    pynlm.ActionType.Rest,
    pynlm.ActionType.Wait
]

print("Available action types:")
for action_type in action_types:
    print(f"  - {action_type.name}")
```

## Enhanced Agent System

### Complete Agent Simulation with Learning

```python
import pynlm
import numpy as np

# Setup complete agent with all features
config = pynlm.createDefaultConfig()

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(50, 50, 8, 8)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)      # NEW: Reward-based learning
agent.enableCuriosity(True)             # NEW: Curiosity-driven exploration
agent.enableStructuralPlasticity(True)  # NEW: Connection growth
agent.enableDevelopment(True)           # NEW: Brain development

# Run complete agent episode
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = world.getEnergyDecayRate() * (step % 10)  # Simulated reward
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress periodically
    if step % 50 == 0:
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        dev_stage = agent.getDevelopmentalStage()
        
        print(f"Step {step}:")
        print(f"  Curiosity: {curiosity:.3f}")
        print(f"  Novelty: {novelty:.3f}")
        print(f"  Prediction Error: {prediction_error:.3f}")
        print(f"  Development Stage: {dev_stage}")

print("Agent simulation complete!")
```

### Agent Brain Monitoring

```python
import pynlm

# Create agent and monitor its state
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable monitoring systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Get comprehensive agent status
status = {
    "ready": agent.isReady(),
    "reward_enabled": agent.isRewardModulationEnabled(),
    "curiosity_enabled": agent.isCuriosityEnabled(),
    "structural_plasticity_enabled": agent.isStructuralPlasticityEnabled(),
    "development_enabled": agent.isDevelopmentEnabled(),
    "neuromodulation_level": agent.getNeuromodulationLevel(),
    "curiosity_level": agent.getCuriosityLevel(),
    "novelty_level": agent.getNoveltyLevel(),
    "prediction_error": agent.getPredictionError(),
    "developmental_stage": agent.getDevelopmentalStage()
}

print("Agent Status:")
for key, value in status.items():
    print(f"  {key}: {value}")
```

## Enhanced Brain Class

### Advanced Brain Operations

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Perform enhanced brain operations
print("Brain Statistics:")
stats = brain.getStats()
for key, value in stats.items():
    if isinstance(value, float):
        print(f"  {key}: {value:.3f}")
    else:
        print(f"  {key}: {value}")

# Wait for minimum spike count (NEW!)
print("Waiting for at least 100 spikes...")
try:
    brain.waitForSpikes(100, timeout=30.0)  # Wait up to 30 seconds
    print("Successfully reached 100 spikes!")
except pynlm.SimulationError as e:
    print(f"Timeout: {e}")

# Save and load brain state (checkpointing)
brain.save("checkpoint.bin")
print("Brain state saved to checkpoint.bin")

# Reset and run again
brain.reset()
brain.initialize()
brain.load("checkpoint.bin")
print("Brain state loaded from checkpoint.bin")
```

### Brain Region Management

```python
import pynlm

# Create brain with multiple regions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add different neural regions
cortex_id = brain.addRegion("cortex")
hippocampus_id = brain.addRegion("hippocampus")
amygdala_id = brain.addRegion("amygdala")

print(f"Created regions: Cortex={cortex_id}, Hippocampus={hippocampus_id}, Amygdala={amygdala_id}")

# Access region properties
cortex = brain.getRegion(cortex_id)
stats = brain.getStats()

# Get all region information
region_count = brain.getRegionCount()
region_ids = brain.getRegionIds()
print(f"Total regions: {region_count}")
print(f"Region IDs: {region_ids}")
```

## Enhanced Simple World

### World Management

```python
import pynlm
import numpy as np

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 16, 16)  # Large world with good vision
world.setMaxEnergy(200.0)          # Increased energy capacity
world.setEnergyDecayRate(0.01)     # Slower energy decay
world.setRandomSeed(42)             # Reproducible simulation

# Add objects to world
resource = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, value=50.0)
world.addObject(resource)

hazard = pynlm.WorldObject(30.0, 30.0, pynlm.WorldObjectType.Hazard, value=-20.0, radius=0.8)
world.addObject(hazard)

wall = pynlm.WorldObject(50.0, 50.0, pynlm.WorldObjectType.Wall, radius=0.5)
world.addObject(wall)

# Check object properties
print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
print(f"Agent start: {world.getAgentBody().x}, {world.getAgentBody().y}")

# Test object properties
print(f"Resource is walkable: {resource.isWalkable()}")
print(f"Hazard is walkable: {hazard.isWalkable()}")
print(f"Wall is walkable: {wall.isWalkable()}")

# Get available resources (NEW!)
resource_count = world.getAvailableResources()
hazard_count = world.getHazardCount()
print(f"Available resources: {resource_count}")
print(f"Hazards in world: {hazard_count}")
```

### World Simulation

```python
import pynlm

# Create agent and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(30, 30, 8, 8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run complete simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process input
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = 0.0
    if percept.getInternal().size() > 0:
        reward = percept.getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)

print("World simulation completed!")
```

## Advanced Features

### Context Manager for Brain Lifecycle

```python
import pynlm

# Use context manager for automatic cleanup
print("Running with BrainLifetimeManager...")

with pynlm.BrainLifetimeManager() as brain_manager:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain_manager.initialize(brain)
    
    print("Brain initialized and managed by context manager")
    
    for step in range(50):
        brain.step(step)
    
    print("Simulation complete, cleanup will happen automatically")

print("Exited context manager - brain should be cleaned up")
```

### Batch Processing

```python
import pynlm

# Create multiple brains for batch processing
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500 + i * 200)
    brain = pynlm.createBrain(config)
    brains.append(brain)

# Create corresponding worlds
worlds = []
for brain in brains:
    world = pynlm.createSimpleWorld()
    world.configure(40, 40, 8, 8)
    worlds.append(world)

print(f"Created {len(brains)} brain-world pairs for batch processing")

# Run batch simulation (simplified example)
# results = pynlm.runBatchSimulation(brains, worlds, 100)
# print(f"Batch simulation completed, got {len(results)} result sets")
```

### Random Connectivity Generation

```python
import pynlm

# Generate random connectivity for neural network design
print("Generating random connectivity matrix...")

# Create 100-neuron network with 20% connectivity
connectivity = pynlm.createRandomConnectivity(100, 0.2)

# Analyze connectivity
connected_pairs = 0
total_pairs = 0
for i in range(len(connectivity)):
    for j in range(len(connectivity[i])):
        total_pairs += 1
        if connectivity[i][j] > 0:
            connected_pairs += 1

print(f"Connectivity matrix: {connected_pairs}/{total_pairs} connections ({connected_pairs/total_pairs:.1%})")
print("This represents a random neural network topology")
```

### Legacy Compatibility

```python
import pynlm

# The enhanced bindings maintain full backward compatibility
print("Testing legacy compatibility...")

# Old API still works
config = pynlm.createDefaultConfig()
config.loadFromFile("legacy_config.json")  # File should exist

brain = pynlm.createBrain(config)  # This still works from old API
brain.initialize()

print("Legacy API compatibility verified!")
```

## Exception Handling

### Custom Exceptions

```python
import pynlm

# Catch specific exceptions for better error handling
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # This would raise InvalidArgumentException if invalid
    # brain.step(-1)  # Example of invalid argument
    
except pynlm.NLMException as e:
    print(f"NLM Error: {e}")
    
except pynlm.InvalidArgumentException as e:
    print(f"Invalid Argument: {e}")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration Error: {e}")
    
except pynlm.SimulationError as e:
    print(f"Simulation Error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Monitoring

### Brain Performance Analysis

```python
import pynlm
import time

# Create brain and monitor performance
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Measure step performance
start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1

end_time = time.time()
elapsed = end_time - start_time
steps_per_second = steps_completed / elapsed

print(f"Performance: {steps_per_second:.1f} steps/second")
print(f"Total neurons: {brain.getStats()['neuron_count']}")
print(f"Total spikes: {brain.getStats()['total_spikes']}")

# Memory usage analysis (NEW!)
import sys
brain_size = sys.getsizeof(brain)
print(f"Brain object size: {brain_size} bytes")
```

## Migration Guide

### From Old to New API

| Old API | New API | Notes |
|---------|---------|-------|
| `config.get("key")` | `config.get("key", "default")` | Added default parameter |
| Basic error handling | Custom exceptions | Better error messages |
| Simple validation | `validateConfig()` | Comprehensive validation |
| Manual cleanup | Context managers | Automatic resource management |
| Basic stats | `getStats()` | Comprehensive statistics |
| Limited enums | Enhanced enums | Better documentation |
| Simple IDs | Enhanced ID methods | `to_int()`, `from_int()` methods |

### Upgrading Steps

1. **Add new imports**: `import pynlm` (no changes needed)
2. **Update configuration access**: Add defaults where needed
3. **Add error handling**: Catch specific exception types
4. **Use context managers**: Wrap brain operations for safety
5. **Explore new features**: Use enhanced methods for better performance

## Development Best Practices

### 1. Always Use Context Managers

```python
# RECOMMENDED: Use context managers for resource safety
with pynlm.BrainLifetimeManager() as brain_manager:
    brain = pynlm.createBrain(config)
    brain_manager.initialize(brain)
    # ... simulation code

# Or use brain lifecycle methods
brain_manager = pynlm.BrainLifetimeManager()
brain = pynlm.createBrain(config)
brain_manager.initialize(brain)
# ... simulation
brain_manager.reset()  # Explicit cleanup
```

### 2. Validate Configuration

```python
# GOOD: Always validate configuration
config = pynlm.createDefaultConfig()
valid, error_msg = pynlm.validateConfig(config)
if not valid:
    raise ValueError(f"Invalid configuration: {error_msg}")

# Even better: Use sample config as template
sample_config = pynlm.createSampleConfig()
```

### 3. Handle Exceptions Properly

```python
# GOOD: Catch specific exception types
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # ... simulation
except pynlm.ConfigurationError as e:
    print(f"Configuration issue: {e}")
    # Fix configuration and retry
except pynlm.SimulationError as e:
    print(f"Simulation failed: {e}")
    # Log error and continue or retry
```

### 4. Use Convenience Methods

```python
# GOOD: Use convenience methods for common tasks
stats = brain.getStats()
print(f"Neurons: {stats['neuron_count']}, Firing: {stats['firing_count']}")

# GOOD: Use batch processing for multiple simulations
# results = pynlm.runBatchSimulation(brains, worlds, steps)
```

### 5. Monitor Performance

```python
# GOOD: Monitor simulation performance
import time
start = time.time()
# ... simulation code
end = time.time()
print(f"Simulation took {end-start:.2f} seconds")
```

## Common Patterns

### Pattern 1: Complete Agent Simulation

```python
def run_agent_simulation(brain_config, world_config, num_steps=1000):
    """Complete agent simulation with all features enabled."""
    # Create brain and agent
    config = brain_config or pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return agent
```

### Pattern 2: Brain Training with Curriculum

```python
def curriculum_learning(config, base_steps=100):
    """Brain training with progressive difficulty."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Phase 1: Simple learning
    for step in range(base_steps):
        brain.step(step)
    
    # Phase 2: Complex learning with sensory input
    for step in range(base_steps):
        # Inject varied sensory input
        vision = pynlm.Vision(8, 8, 3)
        brain.receiveSensoryInput(vision)
        brain.step(step)
    
    # Phase 3: Advanced tasks
    for step in range(base_steps):
        # Complex tasks requiring planning
        brain.step(step)
        
        # Get action and apply
        action = brain.produceAction()
        # ... apply action to world
    
    return brain
```

### Pattern 3: Multi-Agent Coordination

```python
def run_multi_agent_simulation(agent_configs, num_steps=500):
    """Run multiple agents in shared environment."""
    agents = []
    brains = []
    
    # Create multiple agents
    for config in agent_configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
        brains.append(brain)
    
    # Shared world
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 16, 16)
    
    # Initialize all agents
    for agent in agents:
        agent.initialize(world)
        agent.enableRewardModulation(True)
    
    # Run multi-agent simulation
    for step in range(num_steps):
        world.update(0.1)
        
        # Each agent perceives and acts
        for agent in agents:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain = agent.getBrain()
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    return agents
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain initialization fails**
```python
# Solution: Check configuration and initialize properly
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
if brain.getStats()['neuron_count'] == 0:
    print("Warning: Brain has no neurons!")
    config.set("brain.neuron_count", 100)  # Set reasonable value
    brain = pynlm.createBrain(config)

brain.initialize()  # Always call initialize!
```

**Issue: Simulation hangs**
```python
# Solution: Add timeout and check for progress
import time

timeout = 30.0  # 30 seconds max
start_time = time.time()
steps_without_progress = 0

for step in range(10000):
    brain.step(step)
    
    # Check if simulation is making progress
    if brain.getTotalSpikeCount() > step * 0.9:
        steps_without_progress = 0
    else:
        steps_without_progress += 1
    
    # Check for timeout
    if time.time() - start_time > timeout:
        raise pynlm.SimulationError(f"Simulation timed out after {timeout} seconds")
    
    # Detect stuck simulation
    if steps_without_progress > 1000:
        print("Warning: Simulation appears to be stuck")
        break
```

**Issue: Memory usage too high**
```python
# Solution: Monitor and optimize memory usage
import psutil
import os

process = psutil.Process(os.getpid())
memory_mb = process.memory_info().rss / 1024 / 1024

print(f"Current memory usage: {memory_mb:.1f} MB")

if memory_mb > 1000:  # 1 GB limit
    print("Warning: High memory usage, consider reducing brain size")
    config.set("brain.neuron_count", min(config.get("brain.neuron_count"), 1000))
```

## Performance Tips

### 1. Optimize Configuration

```python
# Use sample config as starting point
config = pynlm.createSampleConfig()

# Only modify parameters you need
# Don't set unnecessary parameters
```

### 2. Use Appropriate Step Sizes

```python
# For long simulations, use appropriate time steps
for step in range(10000):
    world.update(0.01)  # Small time step
    brain.step(step)
    # ...
```

### 3. Batch Similar Operations

```python
# Group similar operations for better cache performance
# Process all sensory input at once
# Apply all actions in sequence
# Record all stats together
```

### 4. Monitor and Adjust

```python
# Continuously monitor performance
for step in range(total_steps):
    # Check performance metrics
    if step % 100 == 0:
        stats = brain.getStats()
        print(f"Step {step}: {stats['firing_count']} firing neurons")
        
        # Adjust based on performance
        if stats['average_firing_rate'] < 0.1:
            print("Low firing rate, consider increasing input")
```

## Migration Examples

### Example 1: Simple Simulation

**Old Code:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    
print("Done!")
```

**New Code:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

with pynlm.BrainLifetimeManager() as brain_manager:
    brain_manager.initialize(brain)
    
    for step in range(100):
        brain.step(step)
        
print("Done!")
```

### Example 2: Agent with Learning

**Old Code:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)
agent.enableRewardModulation(True)

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
```

**New Code:**
```python
import pynlm

# Create complete agent with all features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(50, 50, 8, 8)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress occasionally
    if step % 100 == 0:
        curiosity = agent.getCuriosityLevel()
        print(f"Step {step}: curiosity={curiosity:.3f}")
```

## Conclusion

The enhanced NLM Python bindings provide a much more Pythonic and feature-rich interface to the neural simulation framework. Key improvements include:

1. **Better API Design**: Pythonic method names and dictionary-style access
2. **Enhanced Error Handling**: Specific exception types for different error conditions
3. **Comprehensive Documentation**: Detailed docstrings with examples
4. **Advanced Features**: Context managers, batch processing, advanced utilities
5. **Improved Performance**: Optimized wrappers and memory management
6. **Backward Compatibility**: Old code continues to work without changes

The new bindings maintain full backward compatibility while providing a much better experience for both new users and existing users who want to upgrade to the enhanced features.

For the most up-to-date information and examples, refer to the documentation and examples in this guide. The NLM Python bindings are now a robust, production-ready library suitable for both research and production environments.

## Support

For issues, questions, or feature requests:
- Check the NLM documentation for usage examples
- Review the documentation in this file for comprehensive guidance
- Report bugs through the NLM issue tracker
- Contribute enhancements to the Python bindings

The enhanced NLM Python bindings are ready to use and significantly improve the developer experience while maintaining full compatibility with existing code.