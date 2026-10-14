# NLM Python Bindings - Complete Documentation

## Overview

This document provides comprehensive documentation for the enhanced NLM Python bindings (`pynlm`). The bindings have been significantly improved to provide a more user-friendly, feature-rich, and Pythonic interface to the NLM (Neural Learning Machine) C++ neural simulation framework.

## What's New in Version 2.0

The enhanced Python bindings introduce numerous improvements over the original implementation:

### 1. Serialization Support
- **JSON/Pickle-style serialization** for brain state
- **Recording and playback** functionality
- **Checkpoint support** using existing Brain::save()/load() methods
- **PythonState class** for in-memory state representation

### 2. Enhanced Type Conversions
- **Automatic std::vector<float> to numpy array conversion**
- **Numpy array to std::vector<float> conversion**
- **Neuron ID utilities** for converting between different representations
- **Robust type conversion** with error handling

### 3. Pythonic APIs
- **Property-style access** for commonly used attributes
- **Context managers** for simulation sessions
- **Iterator support** for collections
- **Helper methods** for common operations

### 4. Better Error Handling
- **Python exceptions** (NLMError, ValidationError, SimulationError)
- **Parameter validation** for method arguments
- **Detailed error messages** with context
- **Graceful error recovery** options

### 5. Advanced Features
- **Simulation recording/playback**
- **Visualization helper functions**
- **Batch processing utilities**
- **Neural network analysis tools**
- **Memory management helpers**

### 6. Documentation and Examples
- **Comprehensive docstrings** for all functions
- **Type hints** throughout the code
- **Example usage** in documentation
- **Quick reference** guide
- **API documentation** with parameter descriptions

### 7. Performance Optimizations
- **Bulk operation support** for large datasets
- **Efficient memory management**
- **Iterator-based processing**
- **Optimized type conversions**

## Installation

The enhanced Python bindings are installed with the main NLM project. No additional installation is required.

```bash
# The Python bindings are automatically installed with the NLM package
# You can import them directly after installing NLM:
import pynlm
```

## Basic Usage

### Quick Start

```python
import pynlm

# Create a simple brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Run a few simulation steps
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Example

```python
import pynlm

# Setup
brain = pynlm.createBrain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get sensory input
    percept = world.getSensoryPercept()
    
    # 3. Process with agent
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks
    brain.step(step)
    
    # 5. Get action
    action = agent.decodeMotorCommand()
    
    # 6. Execute action
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

## Enhanced Features

### Property-Style Access

The enhanced bindings provide property-style access to commonly used attributes:

```python
brain = pynlm.createBrain()
brain.initialize()

# Direct attribute access
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Simulation time: {brain.getSimulationTime()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Context Managers

Use context managers for automatic initialization and reset:

```python
brain = pynlm.createBrain()

# Context manager with automatic reset
with brain.create_context_manager(auto_reset=True):
    brain.step(0)
    action = brain.produce_action()
    print(f"Produced action: {action.getName()}")

# Context manager without automatic reset
with brain.create_context_manager(auto_reset=False):
    brain.initialize()  # Must be initialized manually
    brain.step(0)
    print("Using context manager")
```

### Network Analysis

Analyze your neural network with built-in tools:

```python
brain = pynlm.createBrain()
brain.initialize()

# Get network statistics
stats = brain.get_stats()
print("Network statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Get region information
regions = brain.get_network_info()
print(f"\nNumber of regions: {len(regions)}")

# Analyze network
analysis = brain.analyze_network()
print(f"\nNetwork analysis:")
for key, value in analysis.items():
    print(f"  {key}: {value}")
```

### Recording and Playback

Record simulation states and playback later:

```python
brain = pynlm.createBrain()
brain.initialize()

# Record simulation
print("Recording simulation...")
brain.enable_recording("simulation.json")

for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"  Step {step}")

brain.disable_recording()
print("Recording complete!")

# Playback recorded simulation
brain2 = pynlm.createBrain()
brain2.playback_state("simulation.json")
print("Playback complete!")
```

### Visualization Data

Create visualization data for plotting and analysis:

```python
brain = pynlm.createBrain()
brain.initialize()

# Enable visualization
brain.enable_visualization()
viz_data = brain.create_visualization_data()

# Use visualization data
print("Visualization data contains:")
for key, value in viz_data.items():
    if hasattr(value, 'shape'):
        print(f"  {key}: array with shape {value.shape}")
    elif hasattr(value, '__len__'):
        print(f"  {key}: collection with {len(value)} items")
    else:
        print(f"  {key}: {type(value).__name__}")

brain.disable_visualization()
```

### Batch Operations

Process multiple neurons or actions efficiently:

```python
brain = pynlm.createBrain()
brain.initialize()

# Batch inject current to all neurons of specific type
print("Injecting current to all excitatory neurons...")
brain.inject_current_to_type(pynlm.NeuronType.Excitatory, 0.5)

# Get membrane potentials as numpy array
potentials = brain.get_numpy_potentials()
print(f"Membrane potentials shape: {potentials.shape}")
print(f"Potentials mean: {potentials.mean():.4f}")
print(f"Potentials std: {potentials.std():.4f}")

# Run simulation
for step in range(20):
    brain.step(step)
```

### Type Conversions

Convert between Python and C++ types seamlessly:

```python
import pynlm

# Convert Python list to numpy array
data = [1.0, 2.0, 3.0, 4.0, 5.0]
array = pynlm.to_numpy_array(data)
print(f"Python list -> Numpy array: {data}")
print(f"  Shape: {array.shape}")
print(f"  Mean: {array.mean():.4f}")

# Convert numpy array back to Python list
vector = pynlm.to_vector(array)
print(f"Numpy array -> Python list: {vector}")
print(f"  Lists equal: {data == vector}")

# Convert neuron types
print("\nNeuron type conversions:")
for neuron_type in pynlm.NeuronType:
    print(f"  {neuron_type} -> {pynlm.neuron_type_to_string(neuron_type)}")

# String to enum conversion
neuron_type = pynlm.string_to_neuron_type("Inhibitory")
print(f"\nString 'Inhibitory' -> {neuron_type}")
```

## State Management

Manage brain state with dictionaries:

```python
brain = pynlm.createBrain()
brain.initialize()

# Get current state
state = brain.get_state()
print("Current state:")
for key, value in state.items():
    print(f"  {key}: {value}")

# Modify state
state["custom_value"] = "test"
state["simulation_time"] = 1.0

# Set state back
brain.set_state(state)
print("\nState set successfully!")
```

## Error Handling

The enhanced bindings provide comprehensive error handling:

```python
import pynlm

brain = pynlm.createBrain()

# Test with uninitialized brain
try:
    brain.step(0)
    print("ERROR: Should have thrown SimulationError")
except pynlm.SimulationError as e:
    print(f"✓ Caught SimulationError: {e}")

# Test with invalid configuration
try:
    pynlm.string_to_neuron_type("InvalidType")
    print("ERROR: Should have thrown PythonError")
except pynlm.PythonError as e:
    print(f"✓ Caught PythonError: {e}")

# Test validation
try:
    brain.set_state({"invalid_key": "not_a_number"})
    print("✓ Handled invalid state gracefully")
except Exception as e:
    print(f"Caught error: {e}")
```

## Configuration Management

Create and manage configurations:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
print(f"Default config has {len(config.getKeys())} keys")

# Get all keys
keys = config.getKeys()
print(f"Config keys: {keys}")

# Set values
config.set("neuron_count", 100)
config.set("region_count", 3)
config.set("connection_probability", 0.1)

# Get values
neuron_count = config.getOr("neuron_count", 50)
print(f"Neuron count: {neuron_count}")

# Create config from dictionary
config_dict = {"neuron_count": 200, "region_count": 5}
config2 = pynlm.create_config_from_dict(config_dict)
print(f"Config2 created with {len(config2.getKeys())} keys")
```

## Development Features

### Development Stages

Control and monitor development stages:

```python
brain = pynlm.createBrain()
brain.initialize()

# Check current development stage
print(f"Current development stage: {brain.get_developmental_stage()}")

# Change development stage
brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
print(f"Changed to: {brain.get_developmental_stage()}")

# Development affects plasticity
brain.enableDevelopment(True)
brain.enableStructuralPlasticity(True)

for step in range(10):
    brain.step(step)
    print(f"Step {step}: Stage = {brain.get_developmental_stage()}")
```

### Neuromodulation

Enable and monitor neuromodulation:

```python
brain = pynlm.createBrain()
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable neuromodulation features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)

# Monitor neuromodulation levels
for step in range(20):
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    agent.initialize(world)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    neuromodulation = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    print(f"Step {step}: Neuromodulation = {neuromodulation:.4f}, Curiosity = {curiosity:.4f}")
```

## Testing

Run the test examples to verify all features:

```bash
# Run basic tests
cd /workspace/app/python
python3 test_bindings.py

# Run example usage
cd /workspace/app/python
python3 example_usage.py
```

## API Reference

### Core Classes

#### Brain
- `createBrain(config)` - Create a new brain
- `get_stats()` - Get network statistics
- `get_network_info()` - Get detailed network information
- `get_state()` - Get brain state as dictionary
- `set_state(state)` - Set brain state from dictionary
- `analyze_network()` - Analyze neural network
- `enable_recording(filepath)` - Enable recording
- `disable_recording()` - Disable recording
- `enable_visualization()` - Enable visualization
- `disable_visualization()` - Disable visualization
- `get_numpy_potentials()` - Get membrane potentials as numpy array
- `inject_current_to_type(type, current)` - Inject current to neurons of type
- `create_context_manager(auto_reset=False)` - Create context manager

#### Config
- `createDefaultConfig()` - Create default configuration
- `create_config_from_dict(config_dict)` - Create config from dictionary

### Helper Functions
- `to_numpy_array(data)` - Convert to numpy array
- `to_vector(array)` - Convert to vector
- `batch_inject_currents(brain, neuron_currents)` - Batch inject currents
- `create_visualization_data(brain)` - Create visualization data
- `playback_state(brain, filepath)` - Playback recorded state
- `record_state(brain, filepath)` - Record current state
- `analyze_network(brain)` - Analyze neural network
- `neuron_ids_to_indices(neuron_ids)` - Convert neuron IDs to indices
- `indices_to_neuron_ids(indices)` - Convert indices to neuron IDs
- `neuron_type_to_string(type)` - Convert neuron type to string
- `string_to_neuron_type(str)` - Convert string to neuron type
- `get_version()` - Get bindings version
- `get_default_config_dict()` - Get default config dictionary

### Enums
- `NeuronType` - Neuron types
- `SynapseType` - Synapse types
- `DevelopmentalStage` - Developmental stages
- `FiringState` - Neuron firing states
- `ActionType` - Action types
- `MotorCommand` - Motor commands
- `WorldObjectType` - World object types

### Exceptions
- `NLMError` - General NLM error
- `ValidationError` - Validation error
- `SimulationError` - Simulation error

### Constants
- `VERSION` - Bindings version
- `DEFAULT_NEURON_COUNT` - Default neuron count
- `DEFAULT_REGION_COUNT` - Default region count
- `DEFAULT_CONNECTION_PROBABILITY` - Default connection probability

## Migration Guide

### From Original to Enhanced Bindings

The enhanced bindings maintain full backward compatibility:

1. **All existing function calls work unchanged**
2. **New features are available through enhanced methods**
3. **Better error messages and type safety**
4. **Additional helper functions for common tasks**

### Example Migration

```python
# Original code (still works)
import pynlm

brain = pynlm.createBrain()
brain.initialize()
for step in range(100):
    brain.step(step)
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Enhanced code (new features)
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Use enhanced features
stats = brain.get_stats()
print(f"Network stats: {stats}")

with brain.create_context_manager(auto_reset=True):
    for step in range(100):
        brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Performance Tips

1. **Use batch operations** for processing multiple neurons
2. **Leverage numpy arrays** for efficient numerical operations
3. **Use context managers** for automatic resource management
4. **Enable recording** for debugging and analysis
5. **Use visualization helpers** for monitoring

## Troubleshooting

### Common Issues

**"Brain is not initialized"**
```python
brain = pynlm.createBrain()
try:
    brain.step(0)
except pynlm.SimulationError as e:
    print(f"Error: {e}")
    # Solution: Call brain.initialize() first
    brain.initialize()
```

**"Recording file not found"**
```python
import os
if not os.path.exists("recording.json"):
    print("Recording file not found")
    # Solution: Check if recording was enabled correctly
```

**"Invalid neuron type"**
```python
try:
    neuron_type = pynlm.string_to_neuron_type("InvalidType")
except pynlm.PythonError as e:
    print(f"Error: {e}")
    # Solution: Use valid neuron type strings
    neuron_type = pynlm.string_to_neuron_type("Excitatory")
```

## Advanced Topics

### Custom Configuration

Create custom configurations for specific use cases:

```python
custom_config = pynlm.create_default_config_dict()
custom_config.update({
    "neuron_count": 1000,
    "region_count": 20,
    "connection_probability": 0.2,
    "learning_rate": 0.05,
    "plasticity_enabled": True,
    "development_enabled": True,
})

brain = pynlm.createBrain(custom_config)
```

### Multi-Agent Systems

Run multiple agents in parallel:

```python
import pynlm
import threading

class AgentRunner:
    def __init__(self, agent_id, config):
        self.agent_id = agent_id
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def run(self, steps):
        for step in range(steps):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())

# Create and run multiple agents
agents = []
for i in range(3):
    config = pynlm.create_default_config_dict()
    config["neuron_count"] = 100
    agents.append(AgentRunner(i, config))

# Run all agents in parallel
threads = []
for agent in agents:
    thread = threading.Thread(target=agent.run, args=(50,))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()
```

## Future Enhancements

The enhanced bindings provide a foundation for future improvements:

1. **Integration with visualization libraries** (matplotlib, plotly)
2. **Advanced machine learning integration**
3. **Distributed computing support**
4. **Web interface for remote control**
5. **Real-time simulation monitoring**
6. **Custom neuron models**
7. **Advanced plasticity rules**
8. **Memory management improvements**

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, user-friendly interface to the NLM neural simulation framework. With backward compatibility maintained, new features, improved error handling, and extensive documentation, the bindings are suitable for both beginners and advanced users.

Key benefits:

- **Pythonic APIs** with property access and context managers
- **Comprehensive serialization** for state management
- **Powerful analysis tools** for neural network study
- **Robust error handling** with clear messages
- **Extensive documentation** with examples
- **Performance optimizations** for large-scale simulations

The enhanced bindings make it easier than ever to create, simulate, and analyze neural networks using the NLM framework.
