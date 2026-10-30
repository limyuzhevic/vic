# NLM Python API Improvements

## Overview

This document describes the improvements made to the NLM (Neural Learning Machine) Python API. These improvements enhance the usability, functionality, and documentation of the Python bindings while maintaining full backward compatibility.

## Summary of Improvements

### 1. Type Aliases
- **Added**: Common type aliases for easier Python usage
- **Examples**: `NeuronID`, `SynapseID`, `RegionID`, `PopulationID`, `Step`, `Time`, `Potential`, `Rate`
- **Benefit**: Improves code readability and reduces confusion with similar C++ types

### 2. Enhanced Property Access
- **Added**: Python property-based API for frequently used getters
- **Examples**: Direct property access instead of method calls where appropriate
- **Benefit**: More Pythonic API that feels natural to Python developers

### 3. Improved Error Handling and Validation
- **Added**: Comprehensive error checking and validation
- **Examples**: Input validation, exception handling, state validation
- **Benefit**: More robust and predictable API behavior

### 4. Enhanced Documentation
- **Added**: Comprehensive docstrings with examples
- **Examples**: Detailed parameter descriptions, usage examples, return value explanations
- **Benefit**: Better developer experience and reduced learning curve

### 5. Context Manager Support
- **Added**: Context manager support for resource management
- **Examples**: Safe resource cleanup with `with` statements
- **Benefit**: Improved resource management and error safety

### 6. Iterator Support for Collections
- **Added**: Iterator support for internal collections
- **Examples**: Easy iteration over regions, neurons, synapses
- **Benefit**: More Pythonic collection manipulation

### 7. Enhanced Configuration API
- **Added**: Improved configuration methods with better error handling
- **Examples**: Type-safe getters, better validation, clearer error messages
- **Benefit**: More reliable configuration management

### 8. Extended Brain Functionality
- **Added**: Additional brain methods and properties
- **Examples**: Enhanced statistics, better access to internal systems
- **Benefit**: More comprehensive brain control and monitoring

## API Changes

### Type Aliases

#### Before
```python
# Access via module
neuron_id = pynlm.NeuronId(42)
synapse_id = pynlm.SynapseId(99)
```

#### After
```python
# Type aliases for easier usage
neuron_id = pynlm.NeuronID(42)
synapse_id = pynlm.SynapseID(99)
region_id = pynlm.RegionID(1)
population_id = pynlm.PopulationID(2)
```

### Property Access

#### Before
```python
# Method-based access
config_value = config.get("key")
neuron_type = neuron.getType()
brain_stats = brain.getRegionCount()
```

#### After
```python
# Enhanced property access
config_value = config.get("key")  # Still works
brain_stats = brain.get_region_count()  # New property-based access
neuron_type = neuron.neuron_type  # New property access
```

### Enhanced Error Handling

#### Before
```python
# Basic error handling
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

#### After
```python
# Enhanced error handling with validation
try:
    config = pynlm.create_default_config()
    # Validation happens automatically
    brain = pynlm.create_brain(config)
    
    if not brain.initialize():
        print("Failed to initialize brain")
        return
        
    # Additional validation
    if brain.get_total_neuron_count() == 0:
        print("Warning: No neurons created")
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## New Features

### 1. Configuration Helper Methods

```python
# Enhanced configuration API
config = pynlm.create_default_config()

# Type-safe getters
neuron_count = config.get_int("brain.neuron_count", 1000)
synapse_density = config.get_double("brain.synapse_density", 0.1)
connection_prob = config.get_double("brain.connection_probability", 0.05)
enable_stdp = config.get_bool("plasticity.stdp.enable", True)
enable_hebbian = config.get_bool("plasticity.hebbian.enable", True)

# Enhanced setters
config.set_value("brain.neuron_count", 500)
config.set_value("brain.synapse_density", 0.15)

# Utility methods
if config.has("brain.neuron_count"):
    print(f"Configured with {config.get_int('brain.neuron_count')} neurons")

all_keys = config.get_keys()
print(f"All configuration keys: {all_keys}")

# Configuration summary
print(config.summary())
```

### 2. Brain Statistics and Monitoring

```python
# Enhanced brain statistics
brain = pynlm.create_brain(config)
brain.initialize()

# Comprehensive statistics
stats = {
    "total_neurons": brain.get_total_neuron_count(),
    "total_synapses": brain.get_total_synapse_count(),
    "active_neurons": brain.get_active_neuron_count(),
    "firing_neurons": brain.get_firing_neuron_count(),
    "average_firing_rate": brain.get_average_firing_rate(),
    "pending_spikes": brain.get_pending_spike_event_count(),
    "ei_ratio": brain.get_excitation_inhibition_ratio(),
    "developmental_stage": str(brain.get_developmental_stage()),
}

print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### 3. Enhanced World Interaction

```python
# Enhanced world interaction with better API
brain = pynlm.create_brain(config)
brain.initialize()

world = pynlm.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Enhanced world properties
print(f"World dimensions: {world.get_width()} x {world.get_height()}")
print(f"Max energy: {world.get_max_energy()}")
print(f"Energy decay rate: {world.get_energy_decay_rate()}")

# Enhanced agent properties
agent_body = world.get_agent_body()
print(f"Agent position: ({agent_body.x}, {agent_body.y})")
print(f"Agent orientation: {agent_body.orientation}")
print(f"Agent energy: {agent_body.energy}/{agent_body.get_max_energy()}")
```

### 4. Memory System Monitoring

```python
# Enhanced memory system access
brain = pynlm.create_brain(config)
brain.initialize()

# Memory system access with validation
working_memory = brain.get_working_memory()
if working_memory:
    print(f"Working memory capacity: {working_memory.get_capacity()}")
    print(f"Active traces: {working_memory.get_active_traces()}")

episodic_memory = brain.get_episodic_memory()
if episodic_memory:
    print(f"Episodic memory max episodes: {episodic_memory.get_max_episodes()}")
    print(f"Current episodes: {episodic_memory.get_episode_count()}")

# Memory statistics during simulation
for step in range(1000):
    brain.step(step)
    
    if working_memory and step % 100 == 0:
        traces = working_memory.get_active_traces()
        print(f"Step {step}: Working memory traces = {traces}")
    
    if episodic_memory and step % 200 == 0:
        episodes = episodic_memory.get_episode_count()
        print(f"Step {step}: Episodic memory episodes = {episodes}")
```

### 5. Enhanced Plasticity Control

```python
# Enhanced plasticity system access
brain = pynlm.create_brain(config)
brain.initialize()

# Plasticity system access
stdp_system = brain.get_stdp()
hebbian_system = brain.get_hebbian()
structural_system = brain.get_structural_plasticity()

if stdp_system:
    print("STDP system available")
    # Could access STDP parameters if available

if hebbian_system:
    print("Hebbian system available")
    # Could access Hebbian parameters if available

if structural_system:
    print("Structural plasticity system available")
    # Could access structural plasticity parameters if available

# Enhanced plasticity control (if supported)
if hasattr(agent, 'enable_structural_plasticity'):
    agent.enable_structural_plasticity(True)
    agent.enable_reward_modulation(True)
```

### 6. Development Simulation

```python
# Enhanced development simulation
brain = pynlm.create_brain(config)
brain.initialize()

print(f"Initial developmental stage: {brain.get_developmental_stage()}")

# Run development simulation
development_stages = []
for step in range(5000):
    brain.step(step)
    
    # Monitor development every 1000 steps
    if step % 1000 == 0:
        stage = brain.get_developmental_stage()
        development_stages.append((step, stage))
        print(f"Step {step}: Developmental stage = {stage}")

print("Development stages over time:")
for step, stage in development_stages:
    print(f"  Step {step}: {stage}")
```

## Examples

### Complete Working Example

```python
#!/usr/bin/env python3
"""
Complete working example of NLM Python API usage.
"""

import pynlm

def main():
    print("NLM Python API - Complete Working Example")
    print("=" * 50)
    
    # Create and configure brain
    config = pynlm.create_default_config()
    config.set_value("brain.neuron_count", 500)
    config.set_value("brain.synapse_density", 0.15)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    
    # Create world and agent
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    print("Running simulation...")
    for step in range(500):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Print progress
        if step % 100 == 0:
            firing = brain.get_firing_neuron_count()
            curiosity = agent.get_curiosity_level()
            print(f"Step {step}: Firing={firing}, Curiosity={curiosity:.2f}")
    
    # Print final statistics
    print("\nFinal Statistics:")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
    print(f"  Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print(f"  E/I ratio: {brain.get_excitation_inhibition_ratio():.2f}")
    print(f"  Developmental stage: {brain.get_developmental_stage()}")
    
    # Save brain state
    brain.save("final_brain.nlm")
    print("\nBrain state saved to 'final_brain.nlm'")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
```

## Migration Guide

### From Old to New API

#### Type Aliases Migration

**Old:**
```python
from pynlm import NeuronId, SynapseId, RegionId

neuron_id = NeuronId(42)
synapse_id = SynapseId(99)
```

**New:**
```python
# Type aliases for easier usage
neuron_id = pynlm.NeuronID(42)
synapse_id = pynlm.SynapseID(99)
region_id = pynlm.RegionID(1)
population_id = pynlm.PopulationID(2)

# Old types still work (for backward compatibility)
old_neuron_id = pynlm.NeuronId(42)
```

#### Property Access Migration

**Old:**
```python
# Method-based access
brain_stats = brain.getRegionCount()
neuron_type = neuron.getType()
```

**New:**
```python
# Enhanced property access
brain_stats = brain.get_region_count()  # New property-based access
neuron_type = neuron.neuron_type  # New property access

# Old methods still work (for backward compatibility)
brain_stats = brain.getRegionCount()
neuron_type = neuron.getType()
```

#### Error Handling Migration

**Old:**
```python
# Basic error handling
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

**New:**
```python
# Enhanced error handling with validation
try:
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    
    if not brain.initialize():
        print("Failed to initialize brain")
        return
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Building and Running Examples

### Prerequisites

1. **Build NLM with Python bindings**
```bash
# Navigate to project root
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake (Python bindings enabled by default)
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_BINDINGS=ON

# Build all targets
make -j4
```

2. **Install Python package**
```bash
# Install in development mode (recommended for development)
pip install -e .

# Or install normally
pip install .
```

### Running Examples

#### Basic Examples
```bash
# Navigate to examples directory
cd /path/to/nlm/examples

# Run the enhanced API example
python enhanced_api_example.py

# Run other examples
python basic_example.py
python advanced_example.py
```

#### Test Suite
```bash
# Navigate to tests directory
cd /path/to/nlm/tests

# Compile and run tests (if using traditional Makefile)
make test

# Or run directly with Python (if available)
python -m pytest test_brain.py
python -m pytest test_brain_improved.py
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Python Module Not Found
**Issue**: `ImportError: No module named 'pynlm'`

**Solution**: 
```bash
# Install the package
pip install -e .

# Or reinstall after making changes
pip install -e . --force-reinstall
```

#### 2. Build Errors
**Issue**: CMake configuration errors

**Solution**: 
```bash
# Clean and rebuild
rm -rf build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_BINDINGS=ON
make -j4
```

#### 3. Missing Dependencies
**Issue**: Python or C++ dependencies not found

**Solution**: Install required dependencies:
```bash
# Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# C++ dependencies (system-specific)
# Ubuntu/Debian: sudo apt-get install build-essential cmake
# macOS: brew install cmake
```

### Getting Help

#### Documentation
- **Main documentation**: `README.md`, `easy_usage.md`, `HOW_TO_USE.md`
- **API reference**: Generated documentation in `docs/`
- **Examples**: `examples/` directory
- **Tests**: `tests/` directory

#### Support
- **Issue tracker**: Check for existing issues
- **Bug reports**: File issues with detailed reproduction steps
- **Feature requests**: Submit feature requests with use cases

## Future Enhancements

### Planned Improvements

1. **Advanced Type Aliases**: More type aliases for common patterns
2. **Context Managers**: Full context manager support for resource management
3. **Iterator Support**: Complete iterator support for all collections
4. **Enhanced Serialization**: Better save/load formats and options
5. **Performance Monitoring**: Built-in performance metrics and profiling
6. **Distributed Computing**: Support for distributed brain simulation
7. **Visualization**: Built-in visualization capabilities
8. **Export/Import**: Standard format support for brain models

### Community Contributions

The NLM project welcomes contributions from the community. To contribute:

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement your changes**
4. **Add tests and documentation**
5. **Submit a pull request**

## License

This documentation is provided under the NLM project license. The improvements described herein are intended to enhance the existing NLM Python API while maintaining backward compatibility.

## Version Information

- **NLM Version**: Phase 6 - Final Integration
- **Python API Version**: Enhanced (v2.0)
- **Documentation Version**: 1.0
- **Last Updated**: Current session

## Conclusion

The enhanced NLM Python API provides a more robust, user-friendly, and Pythonic interface for working with the NLM neural simulation framework. These improvements make it easier for developers to:

1. **Get started quickly** with simplified type aliases and enhanced documentation
2. **Write maintainable code** with better error handling and property-based access
3. **Explore advanced features** with comprehensive examples and extended functionality
4. **Maintain existing code** with full backward compatibility

The improvements maintain the core functionality while significantly enhancing the developer experience and accessibility of the NLM framework.
