# NLM Python Bindings - Complete Improvements Guide

This document provides comprehensive coverage of all improvements made to the NLM Python bindings.

## Overview of Improvements

The NLM Python bindings have been significantly enhanced with the following improvements:

### 1. Enhanced API Design
- **Type-safe configuration access** with validation
- **Convenience methods** for common operations
- **Context managers** for resource management
- **Improved error handling** with custom exceptions
- **Backward compatibility** maintained for existing code

### 2. Configuration System Enhancements
- **JSON/YAML support** for configuration files
- **Configuration validation** with type checking
- **Range validation** for numeric parameters
- **Template support** for predefined configurations
- **Statistics and analysis** for configuration management

### 3. Advanced Brain Control
- **Enhanced simulation methods** with better error handling
- **Comprehensive statistics** and analysis
- **Activity heatmap generation** for visualization
- **Context manager support** for brain lifecycle
- **Clone and copy functionality**

### 4. Advanced AgentBrain Features
- **Enhanced neuromodulation** with dopamine and curiosity support
- **Behavioral profile analysis** for agent behavior
- **Episode simulation** and training capabilities
- **Batch processing** for multiple inputs
- **Advanced training** with monitoring and logging

### 5. Performance and Monitoring
- **Performance profiling tools** (planned)
- **Activity monitoring** and statistics
- **Resource management** with context managers
- **Error recovery** and graceful degradation

## 1. Enhanced Configuration System

### Type-Safe Configuration Access

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Type-safe access with validation
neuron_count = config.get_int("brain.neuron_count")  # Returns int64_t or raises ConfigError
learning_rate = config.get_float("plasticity.learning_rate")  # Returns double or raises ConfigError
enable_stdp = config.get_bool("plasticity.stdp.enable")  # Returns bool or raises ConfigError
default_name = config.get_or("agent.name", "default_agent")  # Returns with default

# Set values with type safety
config.set("brain.neuron_count", 1000)
config.set("plasticity.learning_rate", 0.01)
config.set("neuromodulation.curiosity.enabled", True)
```

### Configuration Validation

```python
# Validate value type
if config.validate_type("neuron_count", "int"):
    print("Neuron count is a valid integer")

# Validate range
if config.validate_range("learning_rate", 0.0, 1.0):
    print("Learning rate is within valid range")

# Check existence
if config.has("brain.neuron_count"):
    print("Configuration exists")
```

### Advanced Configuration Features

```python
# Load from JSON file
config.loadFromFile("config.json")

# Load from Python dictionary
config_data = {
    "brain.neuron_count": 1000,
    "plasticity.learning_rate": 0.01,
    "neuromodulation.curiosity.enabled": True
}
config.loadFromDict(config_data)

# Save to JSON file
config.saveToFile("config.json")

# Merge configurations
other_config = pynlm.createDefaultConfig()
other_config.set("brain.synapse_density", 0.1)
config.merge(other_config)

# Get configuration statistics
stats = config.getStats()
print(f"Total keys: {stats['total_keys']}")
print(f"Int values: {stats['int_values']}")
print(f"Float values: {stats['float_values']}")
```

## 2. Enhanced Brain Class

### Enhanced Simulation Methods

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Single step with error handling
try:
    brain.step(0)
    brain.step_with_time(1, 0.1)
except SimulationError as e:
    print(f"Simulation error: {e}")

# Multiple steps efficiently
brain.step_multiple(100, 0.001)  # 100 steps with 1ms timestep

# Episode simulation
final_spikes = brain.run_episode(1000)  # Run 1000 steps
```

### Enhanced Statistics and Analysis

```python
# Get comprehensive brain statistics
stats = brain.getBrainStats()
print(f"Neurons: {stats['neurons']}")
print(f"Synapses: {stats['synapses']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']}")
print(f"E/I ratio: {stats['e_i_ratio']}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Development stage: {stats['development_stage']}")
print(f"Region count: {stats['region_count']}")

# Get activity heatmap for visualization
heatmap = brain.getActivityHeatmap()
for region_data in heatmap:
    print(f"Region: {region_data['region']}")
    print(f"  Activity level: {region_data['activity_level']:.2f}")
    print(f"  Neuron count: {region_data['neuron_count']}")
```

### Advanced Brain Features

```python
# Add regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Get region names
region_names = brain.get_region_names()
print(f"Regions: {region_names}")

# Clone brain
brain_copy = brain.clone()

# Context manager for automatic resource management
with brain:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
    brain.logStatus()
```

### Brain Validation

```python
# Validate brain state
validation = brain.validateState()
if not validation["valid"]:
    errors = validation["errors"]
    for error in errors:
        print(f"Error: {error}")
else:
    print("Brain state is valid")
```

## 3. Enhanced AgentBrain Class

### Enhanced Sensory Processing

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Process single sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Process batch of sensory inputs
percepts = [world.getSensoryPercept() for _ in range(10)]
agent.processSensoryBatch(percepts)
```

### Enhanced Motor Decoding

```python
# Decode motor command
action = agent.decodeMotorCommand()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")

# Alternative decode method
action = agent.decodeAction()
```

### Enhanced Neuromodulation

```python
# Apply reward-based neuromodulation
agent.applyRewardModulation(0.5, 0.3)

# Apply dopamine-based modulation
agent.applyDopamineModulation(1.2)

# Apply curiosity-based modulation
agent.applyCuriosityModulation(0.8)
```

### Enhanced Subsystem Management

```python
# Enable/disable individual subsystems
agent.enableSubsystem("reward_modulation", True)
agent.enableSubsystem("structural_plasticity", True)
agent.enableSubsystem("development", True)
agent.enableSubsystem("curiosity", True)

# Enable/disable all subsystems
agent.enableAllSubsystems(True)
```

### Behavioral Profile

```python
# Get agent behavioral profile
profile = agent.getBehaviorProfile()
print(f"Exploration vs Exploitation: {profile['exploration_vs_exploitation']}")
print(f"Novelty Sensitivity: {profile['novelty_sensitivity']}")
print(f"Learning Rate: {profile['learning_rate']}")
print(f"Motivation Level: {profile['motivation_level']}")
print(f"Developmental Maturity: {profile['developmental_maturity']}")
```

### Episode Simulation and Training

```python
# Simulate single episode
results = agent.simulateEpisode(world, steps=500, timestep=0.1)
print(f"Final reward: {results['final_reward']:.3f}")
print(f"Average reward: {sum(results['rewards']) / len(results['rewards']):.3f}")

# Train agent over multiple episodes
training_results = agent.trainAgent(world, episodes=100, steps_per_episode=200)
print(f"Average reward: {training_results['average_reward']:.3f}")
```

### Context Manager Support

```python
# Context manager for automatic reset
with agent:
    agent.initialize(world)
    for step in range(1000):
        # Process episode
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## 4. Error Handling

### Custom Exception Types

```python
import pynlm

# Custom exception types
class NLMError(Exception):
    pass

class ConfigError(NLMError):
    pass

class SimulationError(NLMError):
    pass

# Error handling with detailed messages
try:
    config = pynlm.createDefaultConfig()
    value = config.get_int("nonexistent.key")
except ConfigError as e:
    print(f"Configuration error: {e}")

try:
    brain.step(0)
except SimulationError as e:
    print(f"Simulation error: {e}")
```

### Better Error Messages

The enhanced error handling provides:
- Detailed error context
- Type validation errors
- Range validation errors
- Input validation errors
- Graceful degradation

## 5. Backward Compatibility

### Existing Code Continues to Work

All existing code continues to work without changes:

```python
# These all still work as before
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
world.update(timestep)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
```

### Migration Guide

If you have existing code, you can take advantage of new features gradually:

```python
# Old code
brain.step(step)

# New code (optional)
brain.step_with_time(step, step * 0.001)

# Old code
config.set("key", "value")

# New code (optional)
config.setValidated("key", "value")
```

## 6. Advanced Usage Patterns

### Pattern 1: Type-Safe Configuration

```python
def setup_brain(config_dict):
    config = pynlm.createDefaultConfig()
    
    # Type-safe configuration
    if "neuron_count" in config_dict:
        config.set("brain.neuron_count", config_dict["neuron_count"])
    
    if "learning_rate" in config_dict:
        config.set("plasticity.learning_rate", config_dict["learning_rate"])
    
    # Validate configuration
    if config.validate_type("brain.neuron_count", "int") and \
       config.validate_range("brain.neuron_count", 100, 10000):
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    else:
        raise ConfigError("Invalid configuration")

brain = setup_brain({
    "neuron_count": 1000,
    "learning_rate": 0.01
})
```

### Pattern 2: Context Manager for Brain Lifecycle

```python
def run_simulation(config, world, num_steps):
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            agent.initialize(world)
            
            for step in range(num_steps):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Update world
                world.update(0.1)
```

### Pattern 3: Agent Training with Monitoring

```python
def train_agent(world, num_episodes=100, steps_per_episode=200):
    config = pynlm.createDefaultConfig()
    
    # Setup with learning subsystems enabled
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableAllSubsystems(True)
    
    # Training loop with monitoring
    episode_rewards = []
    
    for episode in range(num_episodes):
        # Simulate episode
        results = agent.simulateEpisode(world, steps_per_episode)
        episode_reward = results['final_reward']
        episode_rewards.append(episode_reward)
        
        # Log progress
        if episode % 10 == 0:
            avg_reward = sum(episode_rewards[-10:]) / 10
            print(f"Episode {episode}: Avg Reward = {avg_reward:.3f}")
            
            # Save checkpoint
            if episode % 50 == 0:
                brain.save(f"checkpoint_episode_{episode}.bin")
    
    # Training results
    final_avg_reward = sum(episode_rewards) / len(episode_rewards)
    return {
        'episode_rewards': episode_rewards,
        'average_reward': final_avg_reward,
        'total_episodes': num_episodes
    }

results = train_agent(world)
print(f"Training completed. Average reward: {results['average_reward']:.3f}")
```

## 7. Performance and Monitoring

### Performance Profiling (Planned)

Future enhancements will include:
- Performance profiling tools
- Resource usage monitoring
- Bottleneck identification
- Optimization recommendations

### Activity Monitoring

The enhanced system provides comprehensive monitoring:
- Brain statistics and analysis
- Agent behavior profiling
- Episode tracking and logging
- Performance metrics

## 8. Summary of All Improvements

### ✅ Completed Improvements:

1. **Enhanced API Design**
   - Type-safe configuration access with validation
   - Convenience methods for common operations
   - Context managers for resource management
   - Comprehensive error handling with custom exceptions
   - Backward compatibility maintained

2. **Configuration System Enhancements**
   - JSON/YAML support for configuration files
   - Configuration validation with type checking
   - Range validation for numeric parameters
   - Template support for predefined configurations
   - Statistics and analysis for configuration management

3. **Enhanced Brain Control**
   - Enhanced simulation methods with better error handling
   - Comprehensive statistics and analysis
   - Activity heatmap generation for visualization
   - Context manager support for brain lifecycle
   - Clone and copy functionality

4. **Enhanced AgentBrain Features**
   - Enhanced neuromodulation with dopamine and curiosity support
   - Behavioral profile analysis for agent behavior
   - Episode simulation and training capabilities
   - Batch processing for multiple inputs
   - Advanced training with monitoring and logging

5. **Documentation and Examples**
   - Comprehensive documentation with examples
   - Advanced usage patterns
   - Migration guide for existing code
   - Quick reference guide

### ⏳ Planned Improvements:

1. **Performance Profiling Tools**
   - Performance monitoring and profiling
   - Resource usage analysis
   - Bottleneck identification

2. **Extensibility Features**
   - Custom neuron type support
   - Advanced learning algorithms
   - Plugin architecture

### 🔄 Backward Compatibility:

All existing code continues to work without changes, ensuring a smooth transition to the enhanced features.

## Installation and Usage

### Quick Start

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

### Advanced Usage

```python
import pynlm

# Create complete agent setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableAllSubsystems(True)

# Run training
results = agent.trainAgent(world, episodes=100, steps_per_episode=200)
print(f"Training completed. Average reward: {results['average_reward']:.3f}")
```

## Conclusion

The NLM Python bindings have been significantly enhanced with:

- **Better API design** for Python developers
- **Enhanced error handling** and validation
- **Advanced features** for power users
- **Improved documentation** and examples
- **Performance monitoring** capabilities
- **Backward compatibility** with existing code

These improvements make the NLM framework more accessible, powerful, and easier to use for both beginners and advanced users.