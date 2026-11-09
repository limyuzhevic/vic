# NLM Python Bindings - Improved API Documentation

## Overview

This document provides comprehensive documentation for the improved NLM (Neural Learning Machine) Python bindings. The bindings have been enhanced with Pythonic APIs, convenience methods, error handling, and better usability for both beginners and advanced users.

## Quick Start

For a quick introduction to using the NLM Python bindings, see `easy_usage.md` and `HOW_TO_USE.md` in the project root.

## Key Improvements

### 1. Enhanced Configuration Class

The `Config` class now provides:

#### New Pythonic Methods

- **`loadFromDict(config_dict)`**: Load configuration from a Python dictionary
  ```python
  config = pynlm.createDefaultConfig()
  config.loadFromDict({
      "brain.neuron_count": 2000,
      "brain.synapse_density": 0.15,
      "plasticity.stdp.enable": True
  })
  ```

- **`set(key, value)`**: Type-safe configuration setting
  ```python
  config.set("brain.neuron_count", 1500)
  config.set("brain.v_thresh", -50.0)
  config.set("learning.rate", 0.001)
  ```

- **`get(key)`**: Type-safe configuration retrieval
  ```python
  neurons = config.get("brain.neuron_count")  # Returns 1500 or None
  firing_rate = config.get("brain.v_thresh")  # Returns -50.0 or None
  ```

- **`getOr(key, default_value)`**: Get value with default
  ```python
  neuron_count = config.getOr("brain.neuron_count", 1000)  # Returns 1500 or 1000
  ```

### 2. Enhanced Brain Class

The `Brain` class now provides:

#### New Convenience Methods

- **`runSimulation(num_steps)`**: Run a complete simulation
  ```python
  brain.runSimulation(1000)  # Initialize and run 1000 steps
  ```

- **`getStats()`**: Get brain statistics as a dictionary
  ```python
  stats = brain.getStats()
  print(f"Neurons: {stats['total_neurons']}")
  print(f"Firing rate: {stats['firing_rate']:.2f} Hz")
  ```

- **`getBrainInfo()`**: Get comprehensive brain information
  ```python
  info = brain.getBrainInfo()
  print(f"Regions: {info['regions']}")
  print(f"Spike count: {info['spike_count']}")
  ```

- **`runWithInput(input, num_steps)`**: Run simulation with sensory input
  ```python
  brain.runWithInput(vision_input, 500)
  ```

- **`injectCurrentSafe(neuron_id, current)`**: Safe current injection
  ```python
  brain.injectCurrentSafe(neuron_id, 0.5)
  ```

### 3. Enhanced AgentBrain Class

The `AgentBrain` class now provides:

#### New Convenience Methods

- **`runAgentSimulation(world, num_steps)`**: Run a complete agent simulation
  ```python
  agent.runAgentSimulation(world, 100)
  ```

- **`getAgentStats()`**: Get agent statistics as a dictionary
  ```python
  agent_stats = agent.getAgentStats()
  print(f"Curiosity: {agent_stats['curiosity']:.3f}")
  print(f"Novelty: {agent_stats['novelty']:.3f}")
  ```

- **`processInputAndValidate(percept)`**: Process input with validation
  ```python
  agent.processInputAndValidate(percept)
  ```

- **`decodeMotorAndValidate()`**: Decode motor command with validation
  ```python
  action = agent.decodeMotorAndValidate()
  ```

- **`enableAllFeatures()`**: Enable all learning features at once
  ```python
  agent.enableAllFeatures()
  ```

## API Reference

### Config Class

| Method | Description | Example |
|--------|-------------|---------|
| `loadFromDict(dict)` | Load from Python dictionary | `config.loadFromDict({"key": "value"})` |
| `set(key, value)` | Set configuration value | `config.set("brain.neuron_count", 1000)` |
| `get(key)` | Get configuration value | `config.get("brain.neuron_count")` |
| `getOr(key, default)` | Get with default value | `config.getOr("key", "default")` |
| `loadFromFile(path)` | Load from file | `config.loadFromFile("config.json")` |
| `saveToFile(path)` | Save to file | `config.saveToFile("config.json")` |

### Brain Class

| Method | Description | Example |
|--------|-------------|---------|
| `runSimulation(steps)` | Run simulation | `brain.runSimulation(100)` |
| `getStats()` | Get statistics | `brain.getStats()` |
| `getBrainInfo()` | Get comprehensive info | `brain.getBrainInfo()` |
| `runWithInput(input, steps)` | Run with sensory input | `brain.runWithInput(vision, 100)` |
| `injectCurrentSafe(neuron, current)` | Safe current injection | `brain.injectCurrentSafe(neuron_id, 0.5)` |
| `saveAndValidate(path)` | Save with validation | `brain.saveAndValidate("checkpoint.bin")` |
| `loadAndValidate(path)` | Load with validation | `brain.loadAndValidate("checkpoint.bin")` |

### AgentBrain Class

| Method | Description | Example |
|--------|-------------|---------|
| `runAgentSimulation(world, steps)` | Run agent simulation | `agent.runAgentSimulation(world, 100)` |
| `getAgentStats()` | Get agent statistics | `agent.getAgentStats()` |
| `processInputAndValidate(percept)` | Process input with validation | `agent.processInputAndValidate(percept)` |
| `decodeMotorAndValidate()` | Decode motor command | `action = agent.decodeMotorAndValidate()` |
| `enableAllFeatures()` | Enable all features | `agent.enableAllFeatures()` |
| `runCompleteEpisode(world, steps)` | Run complete episode | `agent.runCompleteEpisode(world, 500)` |

## Error Handling and Validation

The improved bindings include comprehensive error handling:

### Type Safety

All methods include type checking and validation:

```python
# Safe type checking
if config.get("brain.neuron_count") is not None:
    neuron_count = config.get("brain.neuron_count")
    print(f"Neuron count: {neuron_count}")

# Safe dictionary access
stats = brain.getStats()
if "total_neurons" in stats:
    print(f"Total neurons: {stats['total_neurons']}")
```

### Exception Handling

The bindings include proper exception handling for invalid operations:

```python
try:
    brain.runSimulation(-1)  # Invalid number of steps
except Exception as e:
    print(f"Error: {e}")

# Safe file operations
try:
    brain.saveAndValidate("checkpoint.bin")
    print("Save successful")
except Exception as e:
    print(f"Save failed: {e}")
```

## Example Usage

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.runSimulation(100)

# Get statistics
stats = brain.getStats()
print(f"Simulation completed!")
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['firing_rate']:.2f} Hz")
print(f"Total spikes: {stats['total_spikes']}")
```

### Example 2: Agent with World Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning features
agent.enableAllFeatures()

# Run complete episode
agent.runCompleteEpisode(world, 200)

# Get statistics
agent_stats = agent.getAgentStats()
brain_stats = brain.getStats()

print("Episode completed!")
print(f"Agent curiosity: {agent_stats['curiosity']:.3f}")
print(f"Agent novelty: {agent_stats['novelty']:.3f}")
print(f"Brain firing rate: {brain_stats['firing_rate']:.2f} Hz")
```

### Example 3: Configuration Management

```python
import pynlm

# Create and configure
config = pynlm.createDefaultConfig()
config.loadFromDict({
    "brain.neuron_count": 3000,
    "brain.v_thresh": -45.0,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True
})

brain = pynlm.createBrain(config)
brain.initialize()

# Use configuration
settings = {
    "neuron_count": config.getOr("brain.neuron_count", 1000),
    "threshold": config.getOr("brain.v_thresh", -50.0),
    "stdp_enabled": config.getOr("plasticity.stdp.enable", False)
}

print("Current settings:")
for key, value in settings.items():
    print(f"  {key}: {value}")

# Save configuration
config.saveToFile("my_config.json")
print("Configuration saved!")
```

## Advanced Usage

### Custom Configuration Loading

For advanced configuration management, you can combine file and dictionary loading:

```python
import pynlm

# Load from file
config = pynlm.createDefaultConfig()
config.loadFromFile("base_config.json")

# Update with dictionary
config.loadFromDict({
    "brain.neuron_count": 5000,  # Override
    "development.enabled": True   # Add new setting
})

# Save updated configuration
config.saveToFile("final_config.json")
```

### Batch Processing

For batch processing with multiple brains:

```python
import pynlm
import time

def create_and_run_simulation(neuron_counts, steps):
    results = []
    
    for neuron_count in neuron_counts:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        brain.runSimulation(steps)
        end_time = time.time()
        
        stats = brain.getStats()
        results.append({
            "neurons": neuron_count,
            "steps": steps,
            "duration": end_time - start_time,
            "firing_rate": stats["firing_rate"],
            "total_spikes": stats["total_spikes"]
        })
    
    return results

# Run batch simulation
neuron_counts = [500, 1000, 2000, 5000]
steps = 100
batch_results = create_and_run_simulation(neuron_counts, steps)

for result in batch_results:
    print(f"Neurons: {result['neurons']}, "
          f"Duration: {result['duration']:.2f}s, "
          f"Firing rate: {result['firing_rate']:.2f} Hz")
```

## Performance Tips

### 1. Memory Management

- Use `getBrainInfo()` instead of multiple individual calls
- Reuse configuration objects when creating multiple brains
- Monitor memory usage when running long simulations

### 2. Simulation Optimization

```python
# Pre-allocate configuration for batch processing
base_config = pynlm.createDefaultConfig()
base_config.set("plasticity.stdp.enable", True)

# Create multiple brains with same settings
brains = []
for i in range(10):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000 + i * 100)
    brains.append(pynlm.createBrain(config))

# Run simulations
for brain in brains:
    brain.initialize()
    brain.runSimulation(50)
```

### 3. Error Prevention

```python
# Always initialize before use
brain.initialize()  # Important!

# Use safe methods for critical operations
brain.saveAndValidate("checkpoint.bin")  # Safer than brain.save()

# Check for valid state
if brain.getTotalNeuronCount() > 0:
    print("Brain is ready for simulation")
```

## Migration Guide

### From Old API to New API

If you're migrating from the old API, here are the key changes:

#### Config Class

**Old:**
```python
config = pynlm.createDefaultConfig()
config.set("key", "value")  # Generic set
val = config.get("key")    # Generic get
```

**New:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Type-specific
val = config.get("brain.neuron_count")  # Type-safe
```

#### Brain Class

**Old:**
```python
brain.initialize()
for i in range(100):
    brain.step(i)
stats = brain.getStats()
```

**New:**
```python
brain.initialize()
brain.runSimulation(100)  # All-in-one method
stats = brain.getStats()
```

#### AgentBrain Class

**Old:**
```python
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
```

**New:**
```python
agent.initialize(world)
agent.processInputAndValidate(percept)
action = agent.decodeMotorAndValidate()
```

## Troubleshooting

### Common Issues and Solutions

1. **Brain not firing**
   - Ensure `initialize()` is called before simulation
   - Check configuration settings
   - Verify sensory input is connected

2. **Memory issues**
   - Monitor simulation length
   - Use `getBrainInfo()` instead of multiple individual calls
   - Consider running smaller simulations

3. **Configuration errors**
   - Use `loadFromDict()` for complex configurations
   - Validate keys before setting
   - Check file permissions for save/load operations

4. **Type errors**
   - Use type-safe methods like `config.getOr()`
   - Check for `None` values when retrieving optional settings
   - Use proper data types in dictionaries

### Error Messages

The bindings provide clear error messages:

- `"Invalid configuration key"` - When setting invalid configuration keys
- `"Failed to load configuration"` - When file operations fail
- `"Brain not initialized"` - When trying to run simulation without initialization
- `"Invalid simulation steps"` - When providing invalid step counts

## Best Practices

### 1. Always Initialize

```python
# Always initialize before use
brain.initialize()
```

### 2. Use Safe Methods

```python
# Use safe methods for critical operations
brain.saveAndValidate("checkpoint.bin")
brain.loadAndValidate("checkpoint.bin")
```

### 3. Validate Inputs

```python
# Validate inputs before use
if num_steps > 0 and neuron_count > 0:
    brain.runSimulation(num_steps)
else:
    print("Invalid parameters")
```

### 4. Monitor Resources

```python
# Monitor resource usage
stats = brain.getStats()
if stats["total_spikes"] > 1000000:
    print("Warning: High spike count detected")
```

### 5. Use Appropriate Methods

```python
# Use appropriate methods for different scenarios
brain.runSimulation(100)  # For simple simulations
brain.runWithInput(input, 100)  # For simulations with input
agent.runCompleteEpisode(world, 100)  # For agent episodes
```

## Conclusion

The improved NLM Python bindings provide a comprehensive, Pythonic API for neural simulation. The enhancements include:

- **Type-safe configuration management**
- **Convenience methods for common operations**
- **Error handling and validation**
- **Better memory management**
- **Performance optimizations**
- **Comprehensive documentation**

These improvements make the NLM library more accessible to Python users while maintaining the power and flexibility of the original C++ implementation.

For more detailed information, refer to:
- `easy_usage.md` - Beginner-friendly guide
- `HOW_TO_USE.md` - Comprehensive usage guide
- `docs/ARCHITECTURE.md` - Technical architecture details
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions