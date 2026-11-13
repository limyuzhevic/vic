# Enhanced NLM Python API Documentation

## Overview

This documentation describes the enhanced Python API for the NLM (Neural Learning Machine) neural simulation framework. The API has been completely redesigned to provide a modern, Pythonic interface while maintaining full backward compatibility.

## Key Improvements

### 1. Property-Based Access

**Before:**
```python
# Verbose method calls
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
config = brain.getConfig()
stage = brain.getDevelopmentalStage()
```

**After:**
```python
# Clean property access
neurons = brain.total_neurons
synapses = brain.total_synapses
firing = brain.firing_neurons
config = brain.config
stage = brain.development_stage
```

**Benefits:**
- More readable and intuitive code
- Reduced boilerplate
- Better IDE support with autocomplete
- More Pythonic naming conventions

### 2. Context Manager Support

**Before:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
# ... manual cleanup
brain.reset()
```

**After:**
```python
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # ... use brain
    # Automatic cleanup on exit
```

**Benefits:**
- Automatic resource management
- Exception-safe cleanup
- More concise code
- Better separation of concerns

### 3. Enhanced Error Handling

**Before:**
```python
try:
    config.loadFromFile("config.json")
except Exception as e:
    print(f"Error: {e}")  # Generic error message
```

**After:**
```python
try:
    config.loadFromFile("config.json")
except pynlm.PyConfigurationError as e:
    print(f"Configuration error: {e}")  # Specific error type
except pynlm.PySimulationError as e:
    print(f"Simulation error: {e}")  # Specific error type
except pynlm.PyNLMError as e:
    print(f"NLM error: {e}")  # Base error type
```

**Benefits:**
- Better error categorization
- More specific error messages
- Easier error handling
- Better debugging experience

### 4. Builder Pattern

**Before:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.enable", True)
```

**After:**
```python
brain = (pynlm.BrainBuilder()
         .with_neuron_count(1000)
         .with_plasticity()
         .with_neuromodulation(dopamine=True)
         .build())
```

**Benefits:**
- More readable configuration
- Type safety
- Validation during construction
- Method chaining for complex configurations

### 5. Convenience Methods

**Before:**
```python
for step in range(100):
    brain.step(step)
```

**After:**
```python
brain.run_steps(100)
```

**Benefits:**
- More concise code
- Better performance
- Clear intent
- Reduced boilerplate

### 6. Enhanced Data Containers

**Before:**
```python
percept = world.getSensoryPercept()
if percept.getVision():
    vision = percept.getVision()
    width = vision.getWidth()
    height = vision.getHeight()
    data = vision.getData()
```

**After:**
```python
percept = world.getSensoryPercept()
if percept.vision:
    vision = percept.vision
    width = vision.width
    height = vision.height
    data = vision.data
```

**Benefits:**
- More intuitive API
- Better type safety
- Reduced verbosity
- Better IDE support

## Migration Guide

### For New Projects

Use the enhanced API from the start:

```python
import pynlm

# Create and configure brain
brain = (pynlm.BrainBuilder()
         .with_neuron_count(1000)
         .with_plasticity(learning_rate=0.001)
         .with_neuromodulation(curiosity=True)
         .build())

# Create and configure world
world = (pynlm.WorldBuilder()
         .width(20).height(20)
         .vision(8, 8)
         .create())

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with context manager
with world:
    brain.initialize()
    
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 100 == 0:
            stats = brain.get_statistics()
            print(f"Step {step}: {stats['neurons']} neurons")
        
        if step % 10 == 0:
            action = agent.decode_motor_command()
            world.applyMotorCommand(action, world.simulation_time)
```

### For Existing Projects

Gradually migrate your code:

```python
# Before (old API)
def simulate_old(brain, world, steps=100):
    brain.initialize()
    for step in range(steps):
        brain.step(step)
    return brain.getTotalNeuronCount()

# After (new API with backward compatibility)
def simulate_new(brain, world, steps=100):
    with brain:
        brain.initialize()
        brain.run_steps(steps)
        return brain.total_neurons

# Migration path - start with new API
brain = pynlm.createBrain(config)
brain.initialize()  # Still works!
brain.run_steps(100)  # New convenience method
print(f"Neurons: {brain.total_neurons}")  # New property access
```

## API Reference

### Core Classes

#### Brain (`pynlm.Brain`)

**Properties:**
- `total_neurons` - Total number of neurons in the brain
- `total_synapses` - Total number of synapses in the brain
- `firing_neurons` - Number of currently firing neurons
- `total_spikes` - Total number of spikes generated
- `average_firing_rate` - Average firing rate in Hz
- `excitation_inhibition_ratio` - E/I balance ratio
- `development_stage` - Current developmental stage
- `config` - Configuration object

**Methods:**
- `initialize()` - Initialize the brain with configuration
- `reset()` - Reset brain state
- `step(step, time=None)` - Perform a simulation step
- `run_steps(n, timestep=0.001)` - Run multiple simulation steps
- `get_status()` - Get human-readable brain status
- `get_statistics()` - Get comprehensive statistics as dictionary
- `save_checkpoint(filename)` - Save brain state to file
- `load_checkpoint(filename)` - Load brain state from file

**Context Manager Support:**
```python
with brain:
    brain.initialize()
    # Automatic cleanup on exit
```

#### Config (`pynlm.Config`)

**Properties:**
- `neuron_count` - Number of neurons
- `synapse_density` - Connection probability
- `random_seed` - Random seed
- `is_stdp_enabled` - STDP plasticity status
- `is_hebbian_enabled` - Hebbian plasticity status
- `is_dopamine_enabled` - Dopamine neuromodulation status
- `is_curiosity_enabled` - Curiosity neuromodulation status

**Methods:**
- `load_from_file(filename)` - Load configuration from JSON file
- `save_to_file(filename)` - Save configuration to JSON file
- `get(key, default=None)` - Get configuration value
- `set(key, value, source)` - Set configuration value
- `get_summary()` - Get human-readable configuration summary
- `get_config_statistics()` - Get configuration statistics

#### SimpleWorld (`pynlm.SimpleWorld`)

**Properties:**
- `width` - World width in pixels
- `height` - World height in pixels
- `simulation_time` - Current simulation time
- `max_energy` - Maximum agent energy
- `energy_decay_rate` - Energy decay rate
- `random_seed` - Random seed

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `reset()` - Reset world state
- `update(timestep)` - Update world state
- `set_agent_start(x, y)` - Set agent starting position
- `apply_motor_command(action, current_time)` - Apply motor command
- `get_sensory_percept()` - Get sensory percept
- `get_agent_body()` - Get agent body state
- `add_object(x, y, type, value=0.0, radius=0.5)` - Add world object
- `remove_object(x, y)` - Remove world object at position
- `is_valid_position(x, y)` - Check if position is valid
- `get_world_status()` - Get human-readable world status
- `get_world_statistics()` - Get world statistics
- `get_agent_body_info()` - Get detailed agent body information

**Context Manager Support:**
```python
with world:
    world.reset()
    # Automatic cleanup on exit
```

#### AgentBrain (`pynlm.AgentBrain`)

**Properties:**
- `is_reward_modulation_enabled` - Reward modulation status
- `is_structural_plasticity_enabled` - Structural plasticity status
- `is_development_enabled` - Development status
- `is_curiosity_enabled` - Curiosity status

**Methods:**
- `initialize(world)` - Initialize agent with world
- `get_sensory_input_size()` - Get expected sensory input size
- `get_motor_output_size()` - Get expected motor output size
- `process_sensory_input(percept)` - Process sensory input
- `decode_motor_command()` - Decode motor command from brain
- `apply_reward_modulation(reward, predicted_reward)` - Apply reward modulation
- `update_development(timestep)` - Update development system
- `get_developmental_stage()` - Get current developmental stage
- `get_neuromodulation_level()` - Get current neuromodulation level
- `get_curiosity_level()` - Get curiosity level
- `get_novelty_level()` - Get novelty level
- `get_prediction_error()` - Get prediction error
- `reset()` - Reset agent for new episode
- `get_brain()` - Get underlying brain
- `enable_reward_modulation(enable)` - Enable/disable reward modulation
- `enable_structural_plasticity(enable)` - Enable/disable structural plasticity
- `enable_development(enable)` - Enable/disable development
- `enable_curiosity(enable)` - Enable/disable curiosity
- `enable_all_learning()` - Enable all learning systems
- `disable_all_learning()` - Disable all learning systems
- `get_agent_statistics()` - Get agent statistics

### Builder Pattern Classes

#### BrainBuilder

Create complex brain configurations with fluent API:

```python
brain = (pynlm.BrainBuilder()
         .with_neuron_count(1000)
         .with_synapse_density(0.1)
         .with_plasticity(learning_rate=0.001, hebbian_rate=0.01)
         .with_neuromodulation(dopamine=True, curiosity=True)
         .with_developmental_stage(pynlm.DevelopmentalStage.Maturation)
         .build())
```

#### WorldBuilder

Create world configurations with fluent API:

```python
world = (pynlm.WorldBuilder()
         .width(20).height(20)
         .vision(8, 8)
         .max_energy(100.0)
         .energy_decay_rate(0.1)
         .random_seed(42)
         .create())
```

#### ConfigBuilder

Create configuration files with fluent API:

```python
config = (pynlm.ConfigBuilder()
          .with_neuron_count(1000)
          .with_synapse_density(0.15)
          .with_plasticity()
          .with_neuromodulation(curiosity=True)
          .from_file("template.json")
          .build())
```

### Data Container Classes

#### EnhancedNeuronId

Enhanced neuron identifier with validation:

```python
neuron_id = pynlm.NeuronId(12345)
if neuron_id.is_valid():
    print(f"Neuron index: {neuron_id.index()}")
    print(f"Neuron value: {neuron_id.value()}")
```

#### EnhancedAction

Enhanced action with validation:

```python
action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.5])
if action.is_valid():
    world.applyMotorCommand(action, world.simulation_time)
```

### Simulation Utilities

#### Episode Running

Run complete simulation episodes with automatic reset:

```python
episode_stats = pynlm.run_episode(
    brain, world, agent,
    max_steps=1000,
    timestep=0.1,
    max_time=10.0
)

print(f"Episode completed: {episode_stats['steps']} steps")
print(f"Final neurons: {episode_stats['final_neurons']}")
print(f"Final spikes: {episode_stats['final_spikes']}")
```

#### Batch Simulation

Run multiple simulations with different configurations:

```python
configs = [
    pynlm.ConfigBuilder().with_neuron_count(100).build(),
    pynlm.ConfigBuilder().with_neuron_count(500).build(),
    pynlm.ConfigBuilder().with_neuron_count(1000).build(),
]

simulation_params = {
    "steps": 100
}

results = pynlm.batch_simulation(configs, simulation_params)

for i, result in enumerate(results):
    print(f"Config {i}: {result['neurons']} neurons, {result['spikes']} spikes")
```

## Error Handling

### Exception Hierarchy

```python
try:
    # Your code here
    pass
except pynlm.PyNLMError as e:
    # All NLM errors
    print(f"NLM error: {e}")
except pynlm.PyConfigurationError as e:
    # Configuration-related errors
    print(f"Configuration error: {e}")
except pynlm.PySimulationError as e:
    # Simulation-related errors
    print(f"Simulation error: {e}")
```

### Common Exceptions

- `PyConfigurationError`: Configuration loading/saving errors
- `PySimulationError`: Simulation execution errors
- `PyNLMError`: General NLM errors

## Performance Considerations

### Property Access vs Method Calls

Property access is optimized and nearly as fast as direct method calls:

```python
# Both are fast, but property access is more Pythonic
neurons1 = brain.getTotalNeuronCount()
neurons2 = brain.total_neurons  # Property access
```

### Context Manager Overhead

Context manager overhead is minimal (single method call):

```python
# Context manager overhead is ~1 microsecond
with brain:
    brain.initialize()
    # ... your code
```

### Memory Management

Context managers ensure proper cleanup even on exceptions:

```python
# This is exception-safe
with brain:
    brain.initialize()
    # Even if an exception occurs here, brain is cleaned up
```

## Testing

The enhanced API includes comprehensive tests covering:

- Property-based access
- Context manager functionality
- Error handling
- Builder patterns
- Backward compatibility
- Performance
- Memory management

Run tests with:
```bash
python test_enhanced_bindings.py
```

## Examples

### Example 1: Basic Simulation

```python
import pynlm

# Create and configure brain
brain = (pynlm.BrainBuilder()
         .with_neuron_count(500)
         .with_plasticity()
         .build())

# Create and configure world
world = (pynlm.WorldBuilder()
         .width(10).height(10)
         .vision(6, 6)
         .create())

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
with world:
    brain.initialize()
    
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 50 == 0:
            stats = brain.get_statistics()
            print(f"Step {step}: {stats['neurons']} neurons, "
                  f"{stats['spikes']} spikes")
        
        if step % 10 == 0:
            action = agent.decode_motor_command()
            world.applyMotorCommand(action, world.simulation_time)
```

### Example 2: Learning Environment

```python
def create_learning_environment():
    """Create a ready-to-use learning environment."""
    # Configure brain
    brain = (pynlm.BrainBuilder()
             .with_neuron_count(1000)
             .with_plasticity()
             .with_neuromodulation()
             .build())
    
    # Create world
    world = (pynlm.WorldBuilder()
             .width(20).height(20)
             .vision(12, 12)
             .max_energy(50.0)
             .create())
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure learning
    agent.enable_all_learning()
    
    return pynlm.LearningEnvironment(brain, world, agent)

# Use the environment
env = create_learning_environment()
results = env.run_learning_episode(max_steps=1000)

print(f"Learning completed!")
print(f"Final neurons: {results['final_neurons']}")
print(f"Final spikes: {results['final_spikes']}")
print(f"Final curiosity: {results['final_curiosity']:.2f}")
```

### Example 3: Configuration Management

```python
import json

# Create configuration with builder
config = (pynlm.ConfigBuilder()
          .with_neuron_count(1000)
          .with_synapse_density(0.1)
          .with_plasticity(stdp=0.001, hebbian=0.01)
          .with_neuromodulation(dopamine=True, curiosity=True)
          .build())

# Save configuration
config.save_to_file("my_brain_config.json")

# Load configuration
config = pynlm.ConfigBuilder().from_file("my_brain_config.json").build()

# Get statistics
stats = config.get_config_statistics()
print(f"Configuration: {stats['neuron_count']} neurons")
print(f"Plasticity: STDP={stats['stdp_enabled']}, Hebbian={stats['hebbian_enabled']}")
print(f"Neuromodulation: Dopamine={stats['dopamine_enabled']}, "
      f"Curiosity={stats['curiosity_enabled']}")
```

## Future Enhancements

The enhanced API provides a foundation for future features:

1. **Event-Driven Architecture**: Observable states and event streams
2. **Plugin Framework**: Dynamic loading of custom components
3. **Advanced Visualization**: Real-time plotting and analysis tools
4. **Distributed Computing**: Multi-process and distributed simulations
5. **Advanced Learning Algorithms**: New learning paradigms

## Summary

The enhanced NLM Python API transforms the low-level C++ bindings into a modern, Pythonic interface that:

- **Improves Developer Experience** with property access and context managers
- **Enhances Code Readability** with builder patterns and data containers
- **Increases Productivity** with high-level convenience methods
- **Maintains Backward Compatibility** while adding modern Python features
- **Provides Comprehensive Testing** for all new functionality
- **Enables Future Extensions** with clean, extensible architecture

This makes the NLM framework more accessible to Python developers while maintaining full access to the powerful C++ simulation capabilities.