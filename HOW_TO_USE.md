# NLM (Neural Learning Machine) - How to Use Guide

## Table of Contents

1. [Installation and Setup](#installation-and-setup)
2. [Core Concepts](#core-concepts)
3. [Basic Usage](#basic-usage)
4. [Advanced Features](#advanced-features)
5. [Configuration Management](#configuration-management)
6. [Performance Optimization](#performance-optimization)
7. [Troubleshooting](#troubleshooting)
8. [Best Practices](#best-practices)
9. [API Reference](#api-reference)
10. [Migration Guide](#migration-guide)

---

## Installation and Setup

### System Requirements

- **Python**: 3.8 or higher
- **Compiler**: C++ compiler supporting C++20
- **Build Tools**: CMake or scikit-build

### Installation Methods

#### Method 1: Standard Installation (Recommended)

```bash
pip install nlm
```

This will install the pre-compiled Python bindings with optimal settings.

#### Method 2: Development Installation

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e .
```

This allows you to modify the source code and see changes immediately.

#### Method 3: Build from Source

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install scikit-build-core pybind11
pip install -e . --no-build-isolation
```

This builds the C++ extension from source for maximum performance.

### Verification

```python
from nlm import __version__
print(f"NLM Version: {__version__}")

from nlm import createBrain
brain = createBrain()
print("✓ NLM imported and initialized successfully!")
```

## Core Concepts

### 1. Brain Architecture

The NLM brain is organized into several key components:

- **Neurons**: Basic computational units that process signals
- **Regions**: Groups of neurons with specific functions
- **Synapses**: Connections between neurons that transmit signals
- **Plasticity Systems**: Mechanisms for modifying connections
- **Memory Systems**: Storage for learned information

### 2. Simulation Timeline

```python
# Simulation steps are discrete and sequential
for step in range(total_steps):
    # 1. Receive sensory input
    percept = world.getSensoryPercept()
    
    # 2. Process with neural network
    brain.receiveSensoryInput(percept)
    
    # 3. Simulate neural dynamics
    brain.step(step, step * timestep)
    
    # 4. Generate action
    action = brain.produceAction()
    
    # 5. Apply action in world
    result = world.applyMotorCommand(action, step)
```

### 3. Learning Mechanisms

NLM implements several forms of learning:

- **STDP (Spike-Timing-Dependent Plasticity)**: Synaptic changes based on timing
- **Hebbian Learning**: "Neurons that fire together, wire together"
- **Structural Plasticity**: Formation and elimination of synapses
- **Reward Modulation**: Neuromodulation based on outcomes

### 4. Developmental Stages

The brain progresses through developmental stages:

1. **Initial**: Basic neural hardware
2. **Critical Period**: High plasticity for rapid learning
3. **Maturation**: Stabilization of important connections
4. **Adult**: Refined performance with maintained plasticity
5. **Aging**: Gradual decline in plasticity and performance

## Basic Usage

### 1. Creating a Brain

```python
from nlm import createBrain, createDefaultConfig

# Create brain with default settings
brain = createBrain()
brain.initialize()

# Create brain with custom configuration
config = createDefaultConfig()
config.setInt("neuron_count", 1000)
config.setDouble("simulation_timestep", 0.001)
brain = createBrain(config)
brain.initialize()
```

### 2. Working with Worlds

```python
from nlm import createSimpleWorld

# Create default world
world = createSimpleWorld()

# Configure world properties
world.configure(width=800, height=600, vision_width=80, vision_height=60)
world.setAgentStart(400, 300)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)

# Reset world for new episode
world.reset()
```

### 3. Agent Brain Integration

```python
from nlm import createAgentBrainWithWorld

# Connect brain to world
agent_brain = createAgentBrainWithWorld(brain, world)

# Initialize with world configuration
agent_brain.initializeWithWorld(world)

# Enable learning capabilities
agent_brain.enableAllFeatures()
```

### 4. Simulation Loop

```python
# Complete simulation example
def run_simulation(brain, world, agent_brain, num_steps=100):
    print(f"Starting simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # 1. Get sensory input
        percept = world.getSensoryPercept()
        
        # 2. Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # 3. Generate action
        action = agent_brain.decodeMotorCommand()
        
        # 4. Apply action
        result = world.applyMotorCommand(action, step)
        
        # 5. Apply learning signals
        if result.success:
            agent_brain.applyModulation(result.reward, result.predicted_reward)
        
        # 6. Update development periodically
        if step % 10 == 0:
            agent_brain.updateDevelopmentSystem(0.1)
        
        # 7. Log progress occasionally
        if step % 20 == 0:
            print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, Firing={brain.getAverageFiringRate():.4f}")
    
    print("Simulation complete!")
    return brain, agent_brain
```

## Advanced Features

### 1. Custom Sensory Processing

```python
from nlm import createBrain

brain = createBrain()

# Create custom sensory input
class CustomSensoryInput:
    def __init__(self, width, height, data):
        self.width = width
        self.height = height
        self.data = data
        self.timestamp = 0
    
    def getData(self):
        return self.data
    
    def getDimensions(self):
        return (self.width, self.height)
    
    def getTimestamp(self):
        return self.timestamp

# Use custom sensory input
custom_sensory = CustomSensoryInput(64, 64, [0.5] * 4096)
brain.receiveSensoryInput(custom_sensory)
```

### 2. Advanced Configuration

```python
from nlm import createDefaultConfig

config = createDefaultConfig()

# Set multiple configuration values
config.setString("experiment_name", "PavlovianConditioning")
config.setInt("max_trials", 100)
config.setDouble("stimulus_duration", 0.5)
config.setDouble("inter_trial_interval", 1.0)
config.setBool("record_traces", True)
config.setBool("enable_debug_logging", False)

# Get configuration summary
print(config.summary())
```

### 3. Checkpoint and Recovery

```python
from nlm import createBrain

brain = createBrain()
brain.initialize()

# Save brain state (checkpoint)
checkpoint_path = "/tmp/neural_checkpoint.bin"
if brain.save(checkpoint_path):
    print(f"✓ Checkpoint saved to {checkpoint_path}")

# Load brain state (recovery)
brain2 = createBrain()
brain2.initialize()
if brain2.load(checkpoint_path):
    print("✓ Checkpoint loaded successfully")
    print(f"  - Restored neurons: {brain2.getTotalNeuronCount()}")
    print(f"  - Restored synapses: {brain2.getTotalSynapseCount()}")
```

### 4. Performance Monitoring

```python
from nlm import createBrain
import time

brain = createBrain()
brain.initialize()

# Monitor performance over time
performance_log = []
start_time = time.time()

for step in range(1000):
    step_start = time.time()
    brain.step(step)
    step_time = time.time() - step_start
    
    performance_log.append({
        'step': step,
        'time': step_time,
        'spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate()
    })

end_time = time.time()
print(f"Total simulation time: {end_time - start_time:.2f} seconds")
print(f"Average step time: {sum(log['time'] for log in performance_log) / len(performance_log):.6f} seconds")
```

## Configuration Management

### 1. Configuration File Operations

```python
from nlm import createDefaultConfig

# Create configuration
config = createDefaultConfig()

# Set various configuration options
config.setString("agent_name", "MyAgent")
config.setInt("brain_size", 500)
config.setDouble("learning_rate", 0.01)
config.setBool("enable_visualization", True)

# Save to file
config.saveToFile("my_config.cfg")

# Load from file
config2 = createDefaultConfig()
config2.loadFromFile("my_config.cfg")
```

### 2. Configuration Validation

```python
from nlm import createDefaultConfig

config = createDefaultConfig()

# Set some configuration
config.setInt("neuron_count", 100)
config.setDouble("simulation_timestep", 0.001)

# Validate configuration
if config.validate():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration has issues")
    # Get specific validation errors
    errors = config.getValidationErrors()
    for error in errors:
        print(f"  - {error}")
```

### 3. Configuration Statistics

```python
from nlm import createDefaultConfig

config = createDefaultConfig()

# Add various configuration entries
config.setString("name", "TestAgent")
config.setInt("neuron_count", 500)
config.setDouble("learning_rate", 0.05)
config.setBool("enable_learning", True)

# Get statistics
print(f"Total entries: {config.entryCount()}")
print(f"Entries by source:")
for source in [ConfigSource::Default, ConfigSource::File, ConfigSource::CommandLine, ConfigSource::Runtime]:
    count = config.entryCount(source)
    if count > 0:
        print(f"  - {source}: {count}")
```

## Performance Optimization

### 1. Memory Management

```python
from nlm import createBrain

brain = createBrain()
brain.initialize()

# Monitor and manage memory
print("Memory usage:")
brain.logStatus()

# Clear working memory when needed
brain.getWorkingMemory().clear()
print("Working memory cleared")
```

### 2. Simulation Speed Optimization

```python
from nlm import createBrain

# Optimize for speed
brain = createBrain()
brain.getConfig().setInt("neuron_count", 200)  # Smaller network
brain.getConfig().setInt("region_count", 1)     # Fewer regions
brain.getConfig().setDouble("simulation_timestep", 0.001)  # Faster timestep
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Profile performance
import cProfile, pstats
from io import StringIO

pr = cProfile.Profile()
pr.enable()

# Run simulation
for step in range(500):
    brain.step(step)

pr.disable()

# Print profile results
s = StringIO()
ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
ps.print_stats()
print(s.getvalue())
```

### 3. Parallel Processing

```python
from nlm import createBrain
import threading
import time

# Create multiple brains for parallel processing
brains = [createBrain() for _ in range(4)]
for brain in brains:
    brain.initialize()

# Function for individual brain simulation
def simulate_brain(brain, steps, thread_id):
    start = time.time()
    for step in range(steps):
        brain.step(step)
    end = time.time()
    print(f"Thread {thread_id}: {end - start:.2f}s")

# Create and start threads
threads = []
for i, brain in enumerate(brains):
    thread = threading.Thread(target=simulate_brain, args=(brain, 100, i))
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

print("All threads completed successfully!")
```

## Troubleshooting

### 1. Common Issues and Solutions

#### Issue: "Brain not initialized"
**Problem**: Trying to use a brain before calling initialize()
**Solution**: Always call initialize() after creating a brain:

```python
brain = createBrain()
brain.initialize()  # Required!
```

#### Issue: "No neurons firing"
**Problem**: Network is not producing any neural activity
**Solution**: Check configuration and increase excitation:

```python
brain = createBrain()
brain.getConfig().setDouble("stdp_ltp_weight", 0.1)  # Increase LTP
brain.getConfig().setDouble("stdp_ltd_weight", 0.01) # Decrease LTD
brain.initialize()
```

#### Issue: "Memory issues"
**Problem**: Memory usage is too high
**Solution**: Monitor and manage memory:

```python
brain = createBrain()
brain.initialize()
brain.logStatus()  # Check memory usage
brain.getWorkingMemory().clear()  # Clear if needed
```

### 2. Error Messages and Fixes

#### Error: "Configuration validation failed"
**Fix**: Check configuration values and fix any inconsistencies:

```python
config = createDefaultConfig()
config.setInt("neuron_count", 0)  # Invalid: cannot be 0
config.validate()  # Will return false
```

**Corrected version**:
```python
config.setInt("neuron_count", 100)  # Valid value
config.validate()  # Will return true
```

#### Error: "Checkpoint save failed"
**Fix**: Ensure write permissions and valid path:

```python
checkpoint_path = "/tmp/neural_checkpoint.bin"
if brain.save(checkpoint_path):
    print("Save successful")
else:
    print("Save failed - check permissions")
```

#### Error: "Step timing too slow"
**Fix**: Optimize simulation configuration:

```python
brain = createBrain()
brain.getConfig().setInt("neuron_count", 100)  # Smaller network
brain.getConfig().setDouble("simulation_timestep", 0.001)  # Faster timestep
brain.initialize()
```

### 3. Debugging Techniques

#### Debug with Logging
```python
from nlm import createBrain

brain = createBrain()
brain.initialize()

# Enable debug logging
brain.getConfig().setBool("enable_debug_logging", True)
brain.initialize()

# Run simulation and observe logs
for step in range(10):
    brain.step(step)
```

#### Debug with Breakpoints
```python
from nlm import createBrain

brain = createBrain()
brain.initialize()

# Set breakpoints for specific steps
for step in range(20):
    if step == 5:
        print(f"Breakpoint at step {step}")
        # Add debugging code here
        pass
    
    brain.step(step)
```

## Best Practices

### 1. Configuration Management

```python
# Always validate configuration
config = createDefaultConfig()
if not config.validate():
    raise ValueError("Invalid configuration")

# Use meaningful configuration names
config.setString("experiment_id", "exp001")
config.setString("agent_name", "LearningAgent")
```

### 2. Error Handling

```python
from nlm import createBrain
import sys

try:
    brain = createBrain()
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Simulation failed: {e}")
    sys.exit(1)
```

### 3. Resource Management

```python
from nlm import createBrain
import atexit

brain = createBrain()
brain.initialize()

# Register cleanup function
@atexit.register
def cleanup():
    print("Cleaning up NLM resources...")
    # Add any cleanup code here

# Run simulation
for step in range(100):
    brain.step(step)
```

### 4. Testing and Validation

```python
from nlm import createBrain, createDefaultConfig

# Unit test for brain initialization
def test_brain_initialization():
    config = createDefaultConfig()
    config.setInt("neuron_count", 100)
    config.setInt("region_count", 1)
    
    brain = createBrain(config)
    assert brain.initialize()
    
    assert brain.getTotalNeuronCount() == 100
    assert brain.getTotalSynapseCount() > 0
    
    print("✓ Brain initialization test passed")

# Run tests
test_brain_initialization()
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()`: Create default configuration
- `config.set(key, value, source)`: Set configuration value
- `config.get(key, default)`: Get configuration value
- `config.has(key)`: Check if key exists
- `config.saveToFile(filepath)`: Save to file
- `config.loadFromFile(filepath)`: Load from file

#### Brain
- `createBrain(config)`: Create brain with configuration
- `brain.initialize()`: Initialize brain
- `brain.step(step, time)`: Perform simulation step
- `brain.receiveSensoryInput(input)`: Receive sensory input
- `brain.produceAction()`: Generate action
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

#### AgentBrain
- `createAgentBrainWithWorld(brain, world)`: Create agent brain
- `agent.initializeWithWorld(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode motor command
- `agent.applyModulation(reward, predicted)`: Apply reward modulation

### Configuration Sources

- `ConfigSource::Default`: Default configuration
- `ConfigSource::File`: File-based configuration
- `ConfigSource::CommandLine`: Command line configuration
- `ConfigSource::Runtime`: Runtime configuration

### Neuron Types

- `NeuronType::Excitatory`: Excitatory neurons
- `NeuronType::Inhibitory`: Inhibitory neurons
- `NeuronType::Modulatory`: Modulatory neurons
- `NeuronType::Sensory`: Sensory neurons
- `NeuronType::Motor`: Motor neurons
- `NeuronType::Internal`: Internal neurons

### Action Types

- `ActionType::MoveForward`: Move forward
- `ActionType::MoveBackward`: Move backward
- `ActionType::MoveLeft`: Move left
- `ActionType::MoveRight`: Move right
- `ActionType::TurnLeft`: Turn left
- `ActionType::TurnRight`: Turn right
- `ActionType::Look`: Look
- `ActionType::LookUp`: Look up
- `ActionType::LookDown`: Look down
- `ActionType::Interact`: Interact
- `ActionType::Eat`: Eat
- `ActionType::Drink`: Drink
- `ActionType::Rest`: Rest
- `ActionType::Wait`: Wait
- `ActionType::Custom`: Custom action

## Migration Guide

### From Previous Versions

#### Upgrading from NLM 0.0.x

1. **Configuration Changes**:
   - New configuration parameters added
   - Some parameter names changed
   - Default values optimized for better performance

2. **API Changes**:
   - New factory functions for better organization
   - Enhanced type safety in configuration
   - Improved error messages and validation

3. **New Features**:
   - Advanced learning mechanisms
   - Developmental stages
   - Personality system
   - Enhanced visualization tools

### Migration Examples

#### Configuration Migration

```python
# Old way
config = createConfig()
config.set("neuronCount", 500)

# New way
config = createDefaultConfig()
config.setInt("neuron_count", 500)
```

#### Brain Creation Migration

```python
# Old way
brain = createBrain()

# New way
brain = createBrain()
```

#### Simulation Loop Migration

```python
# Old way
for i in range(steps):
    brain.step(i)

# New way
for step in range(total_steps):
    brain.step(step, step * timestep)
```

### Breaking Changes

- Configuration parameter names changed to be more consistent
- Some methods renamed for clarity
- New required parameters in some APIs
- Default behavior changed in some edge cases

### Compatibility

- Most existing code will work with minimal changes
- New features are additive
- Backward compatibility maintained for core functionality
- Migration path provided for major changes

## Conclusion

This comprehensive guide covers all aspects of using the NLM neural simulation framework. Follow the best practices, use proper error handling, and take advantage of the advanced features to create sophisticated neural agents and simulations.

For more information, refer to the complete documentation in README.md and explore the examples directory for additional use cases and patterns.

Happy neural simulation! 🚀