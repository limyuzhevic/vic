"""
NLM Python API Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings.

=== Overview ===

The NLM (Neural Learning Machine) Python bindings provide access to the full C++ neural simulation
framework through a Pythonic API. This documentation covers all the new enhanced features
introduced in the latest version.

=== Key Improvements ===

#### 1. Context Managers for Resource Management

The `BrainContext` class provides automatic resource management:

```python
# Basic usage
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=100)
    # Brain is automatically reset when exiting the context
```

Benefits:
- Automatic brain initialization and cleanup
- Exception-safe resource management
- Cleaner, more readable code

#### 2. Pythonic Convenience Methods

##### Brain Class Enhancements

```python
# Get comprehensive statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']} Hz")

# Run simulations with verbose output
brain.simulate(steps=1000, timestep=0.1, verbose=True)

# Run complete episodes with world and agent
brain.run_episode(world, agent, steps=100)

# Create visualizations
brain.visualize_brain("output.png")
```

##### AgentBrain Class Enhancements

```python
# Get comprehensive agent statistics
agent_stats = agent.get_agent_stats()
print(f"Curiosity level: {agent_stats['curiosity_level']}")
print(f"Novelty level: {agent_stats['novelty_level']}")

# Setup learning with convenience method
agent.setup_learning(reward=True, curiosity=True, development=True)

# Create and run complete episodes
agent.create_episode(world, steps=200)
```

#### 3. Enhanced Configuration API

##### Pythonic Dictionary-Style Access

```python
config = pynlm.createDefaultConfig()

# Get values with type-safe methods
neuron_count = config.get_int("brain.neuron_count", 1000)
synapse_density = config.get_float("brain.synapse_density", 0.1)
is_enabled = config.get_bool("plasticity.stdp.enable", True)

# Python dictionary-style access
try:
    value = config["some.key"]
except KeyError:
    value = "default"

# Set multiple values at once
updates = {
    "brain.neuron_count": 500,
    "plasticity.stdp.learning_rate": 0.002,
    "neuromod.curiosity.enable": True
}
config.update(updates)

# Save configuration as Python dictionary
config_dict = config.save_dict()
for key, value in config_dict.items():
    print(f"{key}: {value}")
```

#### 4. Advanced Error Handling

```python
import pynlm

config = pynlm.createDefaultConfig()

# Safe access with default values
value = config.get_string("nonexistent.key", "default")

# Type checking and conversion
config["test.int"] = 42
config["test.float"] = 3.14
config["test.bool"] = True

# Exception handling
try:
    invalid_value = config["invalid.key"]
except KeyError as e:
    print(f"Key not found: {e}")
except TypeError as e:
    print(f"Type error: {e}")
```

#### 5. Experimental Setup Capabilities

```python
def create_experimental_brain():
    """Create a brain with experimental configuration"""
    config = pynlm.createDefaultConfig()
    
    experimental_config = {
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.12,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "neuromod.curiosity.enable": True,
        "neuromod.curiosity.strength": 0.8,
        "development.enabled": True,
        "debug.enabled": True,
        "debug.log_level": "INFO"
    }
    
    config.update(experimental_config)
    return pynlm.createBrain(config)

# Use context manager for controlled experiments
with pynlm.BrainContext(brain) as experimental_brain:
    experimental_brain.initialize()
    experimental_brain.simulate(steps=1000)
    stats = experimental_brain.get_statistics()
    experimental_brain.visualize_brain("experiment.png")
```

=== Performance Monitoring ===

The enhanced API provides built-in performance monitoring:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with timing
import time
start_time = time.time()
brain.simulate(steps=1000, verbose=True)
end_time = time.time()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Get performance statistics
stats = brain.get_statistics()
print(f"Firing rate: {stats['average_firing_rate']:.3f} Hz")
print(f"Network efficiency: {stats['firing_neurons']/stats['total_neurons']*100:.1f}%")
```

=== Visualization and Debugging ===

```python
# Create basic brain visualization
brain.visualize_brain("brain_state.png")

# Get debugging information
brain.logStatus()
stats = brain.get_statistics()

# Debug agent behavior
agent_stats = agent.get_agent_stats()
print(f"Agent state: {agent_stats}")
```

=== Advanced Features ===

#### 1. Memory Management

Context managers ensure proper memory cleanup:

```python
# Old way (manual management)
brain = pynlm.createBrain(config)
brain.initialize()
# ... use brain ...
brain.reset()

# New way (automatic management)
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    # ... use brain ...
# Brain is automatically cleaned up
```

#### 2. Type Safety

Enhanced type checking and conversion:

```python
# Safe type conversion
int_value = config.get_int("key", 0)
float_value = config.get_float("key", 0.0)
str_value = config.get_string("key", "")
bool_value = config.get_bool("key", False)

# Type checking
if isinstance(value, int):
    # Safe to use as integer
    pass
elif isinstance(value, float):
    # Safe to use as float
    pass
```

#### 3. Configuration Validation

The configuration system provides validation:

```python
# Check if key exists
if config.has("brain.neuron_count"):
    # Key exists, safe to use
    pass

# Remove invalid keys
config.remove("invalid.key")

# Clear entire configuration
config.clear()
```

=== Best Practices ===

1. **Use Context Managers**: Always use `BrainContext` for automatic resource management.

2. **Leverage Convenience Methods**: Use `simulate()`, `get_statistics()`, and `run_episode()` for common operations.

3. **Pythonic Configuration**: Use dictionary-style access for configuration.

4. **Error Handling**: Always handle `KeyError` and other exceptions when accessing configuration.

5. **Performance Monitoring**: Use statistics and timing for performance analysis.

6. **Debugging**: Use `logStatus()` and visualization methods for debugging.

=== Migration Guide ===

#### From Old API to New API

Old way:
```python
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

New way:
```python
with pynlm.BrainContext(pynlm.createBrain(config)) as brain:
    brain.initialize()
    brain.simulate(steps=100, verbose=True)
    stats = brain.get_statistics()
    print(f"Neurons: {stats['total_neurons']}")
```

=== Compatibility ===

All enhancements are backward compatible. Existing code will continue to work, and new features can be used incrementally.

=== Additional Resources ===

- [NLM Python Examples](examples.py)
- [Beginner Examples](beginner_examples.py)
- [NLM Documentation](docs/)
- [GitHub Repository](https://github.com/nlm-project/nlm)

=== License ===

MIT License

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""
