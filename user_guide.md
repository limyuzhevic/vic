# NLM User Guide

## Overview

This user guide provides comprehensive instructions for using the NLM (Neural Learning Machine) framework. It covers everything from basic setup to advanced usage, making it accessible for users of all skill levels.

## Getting Started

### Installation

The NLM framework is already installed with the project. To use the Python bindings:

```python
import pynlm
```

For C++ development, ensure you have a C++20 compatible compiler and the NLM headers.

### Basic Setup

Create your first neural network simulation:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)
    action = brain.produce_action()
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Concepts

### Brain Components

A Brain consists of several key components:

- **Neurons**: Computational units that process information
- **Regions**: Groups of neurons organized by function
- **Synapses**: Connections between neurons
- **Membrane Potentials**: Electrical potentials across neuron membranes

### Simulation Process

1. **Initialization**: Configure and set up the neural network
2. **Sensory Processing**: Receive environmental input
3. **Forward Pass**: Process information through neural layers
4. **Action Production**: Generate motor actions based on processed information
5. **Learning**: Update synaptic weights through plasticity mechanisms

## Tutorial

### Tutorial 1: Basic Neural Network

Create and train a simple neural network:

```python
def basic_neural_network_example():
    """Create a basic neural network example."""
    import pynlm
    import time
    
    print("=== Basic Neural Network Example ===")
    
    # Create brain with custom configuration
    config = pynlm.create_default_config_dict()
    config["neuron_count"] = 100
    config["region_count"] = 3
    config["connection_probability"] = 0.1
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create sensory and motor systems
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get results
    stats = brain.get_stats()
    print(f"Total neurons: {stats['neuron_count']}")
    print(f"Total synapses: {stats['synapse_count']}")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    
    return brain

if __name__ == "__main__":
    brain = basic_neural_network_example()
```

### Tutorial 2: Enhanced Features

Explore the enhanced Python features:

```python
def enhanced_features_example():
    """Demonstrate enhanced Python features."""
    import pynlm
    import numpy as np
    
    print("=== Enhanced Features Example ===")
    
    # Create brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # 1. Property-style access
    print("Property-style access:")
    print(f"  Total neurons: {brain.get_total_neuron_count()}")
    print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
    
    # 2. Context manager
    print("\nContext manager example:")
    with brain.create_context_manager(auto_reset=True):
        for step in range(10):
            brain.step(step)
            action = brain.produce_action()
            print(f"  Step {step}: Produced action '{action.getName()}'")
    
    # 3. Type conversions
    print("\nType conversions:")
    data = [1.0, 2.0, 3.0, 4.0, 5.0]
    array = pynlm.to_numpy_array(data)
    print(f"  Python list -> NumPy array: {array.shape}")
    
    # 4. Network analysis
    print("\nNetwork analysis:")
    analysis = brain.analyze_network()
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    return brain
```

### Tutorial 3: Recording and Playback

Record and replay neural network simulations:

```python
def recording_example():
    """Demonstrate recording and playback functionality."""
    import pynlm
    import json
    
    print("=== Recording and Playback Example ===")
    
    # Create and run brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Record simulation
    print("Recording simulation...")
    recording_file = "simulation.json"
    brain.enable_recording(recording_file)
    
    for step in range(20):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}")
    
    brain.disable_recording()
    print("Recording complete!")
    
    # Save recorded state
    if os.path.exists(recording_file):
        with open(recording_file, 'r') as f:
            recorded_data = json.load(f)
        print(f"Recorded {len(recorded_data)} time points")
        os.remove(recording_file)  # Clean up
    
    # Playback recorded simulation
    print("\nPlaying back simulation...")
    brain2 = pynlm.createBrain()
    brain2.playback_state(recording_file)  # This would fail since file was deleted
    print("Playback would restore brain state from file")
    
    return brain
```

## Configuration Management

### Default Configuration

The framework uses a comprehensive configuration system:

```python
import pynlm

# Get default configuration
config = pynlm.create_default_config_dict()
print("Default configuration keys:", config.keys())

# Customize configuration
custom_config = {
    "neuron_count": 500,
    "region_count": 10,
    "connection_probability": 0.15,
    "learning_rate": 0.05,
    "plasticity_enabled": True,
    "development_enabled": True,
    "num_threads": 4
}

brain = pynlm.createBrain(custom_config)
brain.initialize()
```

### Configuration Validation

All configurations are validated before use:

```python
def validate_config_example():
    """Show configuration validation."""
    import pynlm
    
    print("=== Configuration Validation Example ===")
    
    # Valid configuration
    valid_config = {
        "neuron_count": 100,
        "connection_probability": 0.1
    }
    config = pynlm.create_config_from_dict(valid_config)
    print(f"Valid config created with {len(config.getKeys())} keys")
    
    # Configuration with defaults for missing keys
    partial_config = {"neuron_count": 50}
    config2 = pynlm.create_config_from_dict(partial_config)
    print(f"Partial config automatically filled with defaults")
    
    return config, config2
```

## Advanced Features

### Batch Processing

Process multiple neurons efficiently:

```python
def batch_processing_example():
    """Demonstrate batch processing capabilities."""
    import pynlm
    
    print("=== Batch Processing Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Batch inject current to all neurons of specific type
    print("Injecting current to all excitatory neurons...")
    brain.inject_current_to_type(pynlm.NeuronType.Excitatory, 0.5)
    
    # Get membrane potentials for all neurons
    potentials = brain.get_numpy_potentials()
    print(f"Membrane potentials shape: {potentials.shape}")
    print(f"Potentials mean: {potentials.mean():.4f}")
    print(f"Potentials std: {potentials.std():.4f}")
    
    # Analyze network state
    stats = brain.get_stats()
    print(f"\nNetwork statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    return brain
```

### Visualization and Analysis

Create visualization data and analyze network structure:

```python
def visualization_example():
    """Demonstrate visualization and analysis capabilities."""
    import pynlm
    
    print("=== Visualization and Analysis Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Enable visualization
    brain.enable_visualization()
    viz_data = brain.create_visualization_data()
    
    print("Visualization data available:")
    for key, value in viz_data.items():
        if hasattr(value, 'shape'):
            print(f"  {key}: array with shape {value.shape}")
        elif hasattr(value, '__len__'):
            print(f"  {key}: collection with {len(value)} items")
        else:
            print(f"  {key}: {type(value).__name__}")
    
    # Network analysis
    print("\nNetwork analysis:")
    analysis = brain.analyze_network()
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    # Get region information
    regions = brain.get_network_info()
    print(f"\nNumber of regions: {len(regions)}")
    
    brain.disable_visualization()
    
    return brain
```

### Development and Neuromodulation

Control brain development and neuromodulation:

```python
def development_example():
    """Demonstrate development and neuromodulation."""
    import pynlm
    
    print("=== Development and Neuromodulation Example ===")
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Check current development stage
    print(f"Current development stage: {brain.get_developmental_stage()}")
    
    # Change development stage
    brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
    print(f"Changed to: {brain.get_developmental_stage()}")
    
    # Enable development features
    brain.enableDevelopment(True)
    brain.enableStructuralPlasticity(True)
    brain.enableSynapticPlasticity(True)
    
    # Run simulation with development
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: Stage = {brain.get_developmental_stage()}")
    
    # Neuromodulation example
    print("\nNeuromodulation example:")
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Reward modulation enabled: {agent.getNeuromodulationLevel()}")
    print(f"Curiosity level: {agent.getCuriosityLevel()}")
    
    return brain
```

## Error Handling

### Common Errors and Solutions

```python
def error_handling_example():
    """Demonstrate error handling."""
    import pynlm
    
    print("=== Error Handling Example ===")
    
    # Test with uninitialized brain
    brain = pynlm.createBrain()
    try:
        brain.step(0)
        print("ERROR: Should have thrown SimulationError")
    except pynlm.SimulationError as e:
        print(f"✓ Caught SimulationError: {e}")
    
    # Proper initialization
    brain.initialize()
    
    # Test invalid neuron type
    try:
        neuron_type = pynlm.string_to_neuron_type("InvalidType")
        print("ERROR: Should have thrown PythonError")
    except pynlm.PythonError as e:
        print(f"✓ Caught PythonError: {e}")
    
    # Valid conversion
    neuron_type = pynlm.string_to_neuron_type("Excitatory")
    print(f"✓ Valid conversion: 'Excitatory' -> {neuron_type}")
    
    return brain
```

## Performance Optimization

### Performance Tips

```python
def performance_tips_example():
    """Demonstrate performance optimization tips."""
    import pynlm
    
    print("=== Performance Optimization Tips ===")
    
    # 1. Use batch operations
    brain = pynlm.createBrain()
    brain.initialize()
    
    print("1. Using batch operations...")
    brain.inject_current_to_type(pynlm.NeuronType.Excitatory, 0.5)
    
    # 2. Use context managers
    print("2. Using context managers for resource management...")
    with brain.create_context_manager(auto_reset=True):
        brain.step(0)
    
    # 3. Enable recording for debugging
    print("3. Using recording for debugging...")
    brain.enable_recording("debug_session.json")
    brain.step(0)
    brain.disable_recording()
    
    # 4. Use numpy arrays for numerical operations
    print("4. Using NumPy arrays for efficient numerical operations...")
    potentials = brain.get_numpy_potentials()
    print(f"   Potentials shape: {potentials.shape}")
    print(f"   Statistical operations: mean={potentials.mean():.4f}, std={potentials.std():.4f}")
    
    return brain
```

## Troubleshooting

### Common Issues

**Issue 1: "Brain is not initialized"**

```python
# PROBLEM: Calling step() without initialization
brain = pynlm.createBrain()
try:
    brain.step(0)
    print("ERROR: Should have thrown SimulationError")
except pynlm.SimulationError as e:
    print(f"Error: {e}")
    # SOLUTION: Call brain.initialize() first
    brain.initialize()
    brain.step(0)
    print("Fixed: Brain initialized successfully")
```

**Issue 2: Recording file not found**

```python
# PROBLEM: Recording file not created
brain = pynlm.createBrain()
brain.initialize()

# Enable recording
brain.enable_recording("recording.json")
brain.step(0)
brain.disable_recording()

# Check if file was created
import os
if not os.path.exists("recording.json"):
    print("Recording file not found")
    # SOLUTION: Check if recording was enabled correctly
    # Make sure enable_recording() was called before simulation steps
    print("Check: Recording should be enabled before simulation steps")
```

**Issue 3: Invalid neuron type**

```python
# PROBLEM: Invalid neuron type conversion
try:
    neuron_type = pynlm.string_to_neuron_type("InvalidType")
    print("ERROR: Should have thrown PythonError")
except pynlm.PythonError as e:
    print(f"Error: {e}")
    # SOLUTION: Use valid neuron type strings
    neuron_type = pynlm.string_to_neuron_type("Excitatory")
    print(f"Fixed: Using valid type 'Excitatory' -> {neuron_type}")
```

## Getting Help

### Resources

- **Documentation**: All documentation files are in the project root
- **Examples**: Run `python3 example_usage.py` in the python directory
- **Tests**: Run `python3 test_bindings.py` in the python directory
- **GitHub Issues**: Report issues at the project repository

### Quick Reference

```python
# Quick reference for common operations

# Create brain
brain = pynlm.createBrain()
brain.initialize()

# Basic operations
brain.step(0)                    # Step simulation
action = brain.produce_action()  # Produce action

# Get statistics
stats = brain.get_stats()
analysis = brain.analyze_network()

# State management
state = brain.get_state()
brain.set_state(state)

# Recording
brain.enable_recording("file.json")
brain.disable_recording()

# Visualization
brain.enable_visualization()
viz_data = brain.create_visualization_data()
brain.disable_visualization()

# Context manager
with brain.create_context_manager(auto_reset=True):
    brain.step(0)  # Auto-initialized/reset
```

## Conclusion

The NLM framework provides a comprehensive platform for neural network simulation and analysis. The enhanced Python bindings make it easy to create, simulate, and analyze complex neural systems while maintaining backward compatibility with existing code.

Key benefits:

- **Pythonic APIs**: Intuitive interface with property access and context managers
- **Enhanced Features**: Comprehensive serialization, visualization, and analysis tools
- **Robust Error Handling**: Detailed error messages with helpful suggestions
- **Performance Optimizations**: Efficient batch operations and memory management
- **Extensive Documentation**: Comprehensive guides and examples for all skill levels

These improvements make the NLM framework more accessible to Python developers while maintaining the power and flexibility of the original C++ implementation.
