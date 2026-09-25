# NLM Comprehensive Guide

## Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator. This guide provides comprehensive information for users of all skill levels, from beginners to advanced users.

## Quick Start

### Installation

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .
```

### Basic Usage Example

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

## Getting Started

### 1. Understanding the Core Components

NLM is built around three main components:

1. **Brain** - The neural processing unit
2. **World** - The environment the brain interacts with
3. **Agent** - The bridge connecting brain to world

### 2. Creating Your First Brain

```python
import pynlm

# Method 1: Simple default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Method 2: Custom configuration with specific parameters
config = pynlm.createConfigWithNeuronCount(2000)
brain = pynlm.createBrain(config)

# Method 3: Learning-optimized configuration
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)

brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### 3. Running a Complete Simulation

```python
import pynlm

def run_complete_simulation():
    # Create and configure brain
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, " +
                  f"Curiosity={agent.getCuriosityLevel():.3f}, " +
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    print("Simulation completed successfully!")

run_complete_simulation()
```

## Advanced Features

### 1. Brain Configuration Templates

```python
import pynlm

# Create different brain configurations for different purposes

templates = {
    'fast': pynlm.createConfigWithPlasticity(True, False, False),  # STDP only
    'stable': pynlm.createConfigWithPlasticity(False, False, False),  # No plasticity
    'learning': pynlm.createLearningConfig(),
    'exploration': pynlm.createExplorationConfig(),
}

# Use template
brain_config = templates['learning']
brain = pynlm.createBrain(brain_config)
```

### 2. Brain Comparison

```python
import pynlm

def compare_brain_configurations():
    # Create two different configurations
    config1 = pynlm.createLearningConfig()
    config2 = pynlm.createExplorationConfig()
    
    brain1 = pynlm.createBrain(config1)
    brain2 = pynlm.createBrain(config2)
    
    brain1.initialize()
    brain2.initialize()
    
    # Run comparison
    pynlm.compareBrains(brain1, brain2, 500)
    
    # Check final statistics
    stats1 = pynlm.getBrainStats(brain1)
    stats2 = pynlm.getBrainStats(brain2)
    
    print(stats1)
    print(stats2)

compare_brain_configurations()
```

### 3. Experiment Running

```python
import pynlm

def run_learning_experiment():
    # Create experiment runner
    experiment = pynlm.createExperimentRunner()
    
    # Create brain for experiment
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Configure for learning
    pynlm.configureBrainForLearning(brain)
    
    # Run experiment
    pynlm.runSimpleExperiment(brain, 2000)
    
    # Get final stats
    stats = pynlm.getBrainStats(brain)
    print(stats)

run_learning_experiment()
```

## Configuration Management

### Configuration Options

```python
import pynlm

config = pynlm.createDefaultConfig()

# Basic settings
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1f)
config.set("connection_probability", 0.05f)

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001f)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Development
config.set("development.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

### Common Configuration Templates

```python
# Quick presets for common use cases

def get_small_brain_config():
    """Config for fast testing and prototyping"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", False)
    return config

def get_large_brain_config():
    """Config for detailed learning and experimentation"""
    config = pynlm.createLearningConfig()
    config.set("brain.neuron_count", 5000)
    return config

def get_reactive_brain_config():
    """Config optimized for reactive behaviors"""
    config = pynlm.createExplorationConfig()
    config.set("brain.neuron_count", 2000)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    return config
```

## Advanced Python Features

### 1. Brain State Management

```python
import pynlm

# Save and load brain states
def save_brain_state(brain, filename, metadata=""):
    """Save brain state with metadata"""
    if pynlm.saveBrainWithMetadata(brain, filename, metadata):
        print(f"Brain saved to {filename}")
    else:
        print(f"Failed to save brain to {filename}")

def load_brain_state(filename, metadata=""):
    """Load brain state with metadata"""
    meta = metadata
    brain = pynlm.loadBrainWithMetadata(filename, meta)
    if brain:
        print(f"Brain loaded from {filename}")
        return brain
    else:
        print(f"Failed to load brain from {filename}")
        return None

# Example usage
brain = pynlm.createBrain(pynlm.createLearningConfig())
brain.initialize()
save_brain_state(brain, "my_brain.bin", "Test run 1")

# Later...
loaded_brain = load_brain_state("my_brain.bin", "Loaded state")
```

### 2. Brain Analysis

```python
import pynlm

# Analyze brain connectivity and properties
brain = pynlm.createBrain(pynlm.createLearningConfig())
brain.initialize()

# Get comprehensive statistics
stats = pynlm.getBrainStats(brain)
print(stats)

# Analyze connectivity
connectivity_data = pynlm.analyzeBrainConnectivity(brain)
print(f"Analyzed connectivity of {len(connectivity_data)} neurons")

# Compare with another brain
brain2 = pynlm.createBrain(pynlm.createExplorationConfig())
brain2.initialize()

pynlm.compareBrains(brain, brain2, 100)
```

### 3. Performance Optimization

```python
import pynlm

# Optimize brain for specific use case
def optimize_for_speed(brain):
    """Disable plasticity for faster performance"""
    pynlm.optimizeBrainForSpeed(brain)
    print("Brain optimized for speed")

def optimize_for_learning(brain):
    """Enable all plasticity mechanisms for better learning"""
    pynlm.optimizeBrainForLearning(brain)
    print("Brain optimized for learning")

brain = pynlm.createBrain(pynlm.createLearningConfig())
brain.initialize()

# Choose optimization based on use case
optimize_for_learning(brain)
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

1. **Brain not firing**
   ```python
   # Check if brain is initialized
   if not brain_initialized:
       brain.initialize()
   
   # Try increasing simulation steps
   for i in range(1000):
       brain.step(i)
   ```

2. **Agent not moving**
   ```python
   # Check world updates
   for step in range(100):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

3. **Memory usage too high**
   ```python
   # Reduce brain size for testing
   config = pynlm.createConfigWithNeuronCount(500)
   brain = pynlm.createBrain(config)
   ```

4. **Simulation too slow**
   ```python
   # Optimize for speed
   pynlm.optimizeBrainForSpeed(brain)
   ```

### Error Handling Pattern

```python
import pynlm

def safe_simulation(brain, steps):
    try:
        # Validate inputs
        if steps <= 0:
            raise ValueError("Steps must be positive")
        
        # Run simulation
        for i in range(steps):
            brain.step(i)
            
    except Exception as e:
        print(f"Simulation error: {e}")
        return False
    
    return True

# Example usage
brain = pynlm.createBrain(pynlm.createLearningConfig())
brain.initialize()

if safe_simulation(brain, 1000):
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

## Best Practices

### 1. Configuration Management

```python
# Use named configurations for different use cases
PROD_CONFIG = pynlm.createConfigWithNeuronCount(2000)
TEST_CONFIG = pynlm.createConfigWithNeuronCount(100)
DEV_CONFIG = pynlm.createLearningConfig()

# Always initialize before use
brain = pynlm.createBrain(TEST_CONFIG)
brain.initialize()
```

### 2. Memory Management

```python
# Clear references when done
brain = None
config = None
world = None
agent = None

# Force garbage collection
import gc
gc.collect()
```

### 3. Simulation Patterns

```python
def run_repeated_episodes(num_episodes, steps_per_episode):
    """Run multiple simulation episodes"""
    for episode in range(num_episodes):
        print(f"Episode {episode + 1}/{num_episodes}")
        
        # Create fresh brain for each episode
        config = pynlm.createLearningConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run episode
        for step in range(steps_per_episode):
            # Your simulation logic here
            brain.step(step)
        
        # Save final state if needed
        if episode % 10 == 0:
            pynlm.saveBrainWithMetadata(brain, f"episode_{episode}.bin", f"Episode {episode}")
```

### 4. Performance Monitoring

```python
import time
import pynlm

brain = pynlm.createBrain(pynlm.createLearningConfig())
brain.initialize()

# Monitor simulation performance
start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1
    
    # Log progress every 100 steps
    if step % 100 == 0:
        elapsed = time.time() - start_time
        steps_per_second = steps_completed / elapsed
        print(f"Step {step}: {steps_per_second:.1f} steps/sec")

elapsed = time.time() - start_time
print(f"Completed {steps_completed} steps in {elapsed:.2f} seconds")
print(f"Average: {steps_completed/elapsed:.1f} steps/sec")
```

## Migration Guide

### From v0.1.0 to v0.1.0+

1. **New configuration helpers** - Use `createLearningConfig()`, `createExplorationConfig()` instead of manual configuration
2. **New simulation utilities** - Use `runSimulation()` and `compareBrains()` for common patterns
3. **Enhanced error handling** - Try-catch blocks are now built into helper functions
4. **Improved documentation** - Comprehensive guides available in this file

## API Reference

### Classes

- **Brain** - Central neural processing unit
- **Config** - Configuration management
- **AgentBrain** - Brain-world interface
- **SimpleWorld** - Environment simulation
- **SensoryInput** - Sensory data base class
- **Vision** - Visual sensory input
- **Audio** - Audio sensory input

### Functions

- **createDefaultConfig()** - Create basic configuration
- **createConfigWithNeuronCount()** - Create config with specific neuron count
- **createLearningConfig()** - Create learning-optimized configuration
- **createExplorationConfig()** - Create exploration-optimized configuration
- **createBrain()** - Create brain with configuration
- **createSimpleWorld()** - Create environment
- **createAgentBrain()** - Create agent interface
- **runSimulation()** - Run simulation with logging
- **configureBrainForLearning()** - Configure brain for learning
- **configureBrainForStability()** - Configure brain for stability
- **getBrainStats()** - Get brain statistics
- **saveBrainWithMetadata()** - Save brain with metadata
- **loadBrainWithMetadata()** - Load brain with metadata

## Examples

See the example directory for more comprehensive examples and tutorials.