# Enhanced Python Bindings for NLM (pynlm)

This document describes the improved Python API for the NLM neural simulation framework.

## Key Improvements Implemented

### 1. Pythonic Property Access

**Before:**
```python
config = brain.getConfig()
brain.initialize()
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
```

**After:**
```python
brain.initialize()
config = brain.config  # Property access
neurons = brain.total_neurons  # Property access
synapses = brain.total_synapses  # Property access
firing = brain.firing_neurons  # Property access
development_stage = brain.development_stage  # Property access
```

### 2. Context Manager Support

**Brain Context Manager:**
```python
with brain as b:
    b.initialize()
    # Automatic cleanup on exit
    pass

with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Brain automatically reset on exit
    pass
```

**World Context Manager:**
```python
with pynlm.createSimpleWorld() as world:
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    # World automatically reset on exit
    pass
```

### 3. Enhanced Error Handling

**Custom Exceptions:**
```python
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors."""
    pass

class SimulationError(NLMError):
    """Simulation-related errors."""
    pass
```

**Better Error Messages:**
```python
# Before: Generic error
try:
    config.loadFromFile("nonexistent.json")
except Exception as e:
    print(f"Error: {e}")

# After: Descriptive error
try:
    config.loadFromFile("nonexistent.json")
except ConfigurationError as e:
    print(f"Configuration error: Cannot load nonexistent.json - file not found")
```

### 4. Convenience Methods

**Batch Operations:**
```python
# Run multiple steps efficiently
brain.run_steps(1000)  # More efficient than manual loop

# Get comprehensive brain statistics
stats = brain.get_statistics()
print(f"Neurons: {stats['neurons']}, Synapses: {stats['synapses']}")
print(f"Firing rate: {stats['firing_rate']:.2f}")
```

**Learning Utilities:**
```python
# Create ready-to-use learning environment
env = pynlm.create_learning_environment(
    brain_neuron_count=1000,
    enable_plasticity=True,
    enable_neuromodulation=True
)

# Run learning episode
results = env.run_learning_episode(max_steps=500)
```

## Migration Guide

### Backward Compatibility
All existing code continues to work unchanged. The improvements are additive, not breaking.

### Gradual Migration
**Step 1:** Start using property access where appropriate
```python
# Old way (still works)
neurons = brain.getTotalNeuronCount()

# New way (recommended)
neurons = brain.total_neurons
```

**Step 2:** Use context managers for resource management
```python
# Old way
brain.initialize()
# ... manual cleanup
brain.reset()

# New way
with brain:
    brain.initialize()
    # ... use brain
# Automatic cleanup
```

**Step 3:** Use high-level APIs for common patterns
```python
# Old way: Multiple lines for complex setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

# New way: Single builder API
brain = (pynlm.BrainBuilder()
         .with_neuron_count(1000)
         .with_plasticity()
         .build())

world = (pynlm.WorldBuilder()
         .width(20).height(20)
         .create())
```

## API Reference

### Core Classes

#### Brain
**Properties:**
- `total_neurons` - Total neuron count
- `total_synapses` - Total synapse count  
- `firing_neurons` - Currently firing neurons
- `average_firing_rate` - Average firing rate
- `development_stage` - Current developmental stage
- `config` - Configuration object
- `total_spikes` - Total spike count
- `excitation_inhibition_ratio` - E/I balance ratio

**Methods:**
- `initialize()` - Initialize brain
- `reset()` - Reset brain state
- `step(step, time=None)` - Perform simulation step
- `run_steps(n)` - Run multiple steps
- `get_statistics()` - Get comprehensive statistics
- `save_checkpoint(filename, metadata=None)` - Save with metadata

#### Config
**Properties:**
- `neuron_count` - Number of neurons
- `synapse_density` - Connection probability
- `random_seed` - Random seed

**Methods:**
- `load_from_file(filepath)` - Load from JSON
- `save_to_file(filepath)` - Save to JSON
- `get(key, default=None)` - Get configuration value

#### SimpleWorld
**Properties:**
- `width`, `height` - World dimensions
- `vision_width`, `vision_height` - Vision dimensions
- `simulation_time` - Current simulation time

**Context Manager:**
```python
with world:
    world.reset()
    # Automatic cleanup on exit
```

#### AgentBrain
**Properties:**
- `is_reward_modulation_enabled` - Reward modulation status
- `is_structural_plasticity_enabled` - Structural plasticity status
- `is_development_enabled` - Development status
- `is_curiosity_enabled` - Curiosity status

**Learning Utilities:**
- `enable_all_learning()` - Enable all learning systems
- `disable_all_learning()` - Disable all learning systems

## Examples

### Basic Usage
```python
import pynlm

# Create and configure brain
brain = (pynlm.BrainBuilder()
         .with_neuron_count(1000)
         .with_stdp(learning_rate=0.001)
         .with_neuromodulation(curiosity=True)
         .build())

# Create world
world = (pynlm.WorldBuilder()
         .width(20).height(20)
         .vision(8, 8)
         .create())

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_all_learning()

# Run simulation
with world:
    brain.initialize()
    
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get statistics periodically
        if step % 100 == 0:
            stats = brain.get_statistics()
            print(f"Step {step}: {stats['neurons']} neurons, "
                  f"{stats['spikes']} spikes")
        
        # Take action if needed
        if step % 10 == 0:
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.simulation_time)
```

### Learning Environment
```python
def create_training_environment():
    """Create a ready-to-use learning environment."""
    # Configure brain
    brain = (pynlm.BrainBuilder()
             .with_neuron_count(500)
             .with_plasticity()
             .with_neuromodulation()
             .build())
    
    # Create world
    world = (pynlm.WorldBuilder()
             .width(10).height(10)
             .vision(6, 6)
             .max_energy(50.0)
             .create())
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Set up learning
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    return pynlm.LearningEnvironment(brain, world, agent)

# Use the environment
env = create_training_environment()
results = env.run_learning_episode(max_steps=500)
print(f"Learning completed: {results['final_curiosity']:.2f} curiosity")
```

### Configuration Management
```python
import pynlm
import json

# Create configuration with builder
config = (pynlm.ConfigBuilder()
          .with_brain(neurons=1000, synapse_density=0.1)
          .with_plasticity(stdp=0.001, hebbian=0.01)
          .with_neuromodulation(dopamine=True, curiosity=True)
          .build())

# Save configuration
config.save_to_file("my_brain_config.json")

# Load configuration
config = pynlm.ConfigBuilder().from_file("my_brain_config.json").build()

# Get statistics
stats = config.get_statistics()
print(f"Config: {stats['neuron_count']} neurons, "
      f"{stats['plasticity_enabled']} plasticity systems")
```

## Performance Considerations

- **Property Access**: Property access is nearly as fast as direct method calls
- **Context Managers**: Overhead is minimal (single method call per context)
- **Builder Pattern**: Slightly more object creation but improves code readability
- **Memory Management**: Context managers ensure proper cleanup even on exceptions

## Testing

All improvements include comprehensive tests:

```python
def test_property_access():
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Test property access
    assert brain.total_neurons == brain.getTotalNeuronCount()
    assert brain.total_synapses == brain.getTotalSynapseCount()
    assert brain.firing_neurons == brain.getFiringNeuronCount()
    
    # Test property setters where appropriate
    brain.development_stage = pynlm.DevelopmentalStage.Maturation

def test_context_manager():
    brain = pynlm.createBrain()
    
    # Test context manager
    with brain:
        brain.initialize()
        # Brain automatically reset on exit
        assert brain.total_neurons > 0
    
    # Brain should be reset after context exit
    assert brain.total_neurons == 0

def test_builder_pattern():
    brain = (pynlm.BrainBuilder()
             .with_neuron_count(1000)
             .with_plasticity()
             .build())
    
    brain.initialize()
    assert brain.total_neurons == 1000
    assert brain.is_structural_plasticity_enabled()
```

## Future Enhancements

The improved API provides a foundation for future features:

1. **Event-Driven Architecture**: Observable states and event streams
2. **Plugin Framework**: Dynamic loading of custom components
3. **Advanced Visualization**: Real-time plotting and analysis tools
4. **Distributed Computing**: Multi-process and distributed simulations

## Summary

The improved NLM Python API transforms the low-level C++ bindings into a modern, Pythonic interface that:

- **Improves Developer Experience** with property access and context managers
- **Enhances Code Readability** with builder patterns and data containers
- **Increases Productivity** with high-level convenience methods
- **Maintains Backward Compatibility** while adding modern Python features
- **Provides Comprehensive Testing** for all new functionality

This makes the NLM framework more accessible to Python developers while maintaining full access to the powerful C++ simulation capabilities.