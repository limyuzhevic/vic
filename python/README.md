# Enhanced NLM Python Bindings Documentation

This document describes the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The NLM Python bindings provide access to a sophisticated neural simulation framework that includes:
- Real spiking neural computation with event-driven dynamics
- Integrated memory systems (working memory, episodic memory, associative memory)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Prediction systems for sensory prediction and error computation
- Cognitive mechanisms (neural planning, concept formation, attentional selection)
- Developmental systems that modulate plasticity over time
- Complete brain-environment interaction loops

## What's New in Enhanced Bindings (Version 2.0.0-enhanced)

### 1. Enhanced Error Handling and Validation

#### New Exception Classes
- `PyNLMError`: Base exception for all NLM-related errors
- `PyValidationError`: For input validation and parameter checking
- `PyConfigurationError`: For configuration-related issues

#### Input Validation
All public methods now include input validation:
- Boundary checks for positions and dimensions
- Data size validation for sensory inputs
- Type checking for configuration values
- Parameter range validation

#### Safe Methods
- `saveSafely()`: Wrapper for save operations with error handling
- `loadSafely()`: Wrapper for load operations with error handling

### 2. Pythonic API Enhancements

#### Context Managers
The `SimulationContext` class provides a clean way to manage simulation sessions:

```python
import nlm

# Create brain and world
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent_brain = nlm.createAgentBrain(brain)

# Run simulation with automatic cleanup
with nlm.SimulationContext(brain, max_steps=1000) as sim:
    world.setAgentStart(50.0, 50.0)
    results = sim.runSteps(500)
    
print(f"Simulation completed: {results['steps_completed']} steps")
print(f"Average firing rate: {results['average_firing_rate']}")
```

#### Enhanced Property Access
- Vision data can be converted to/from Python list-of-lists format
- Better access to internal state and statistics
- Property-based access patterns for better readability

#### Type Conversion Utilities
- `VisionHelper`: Convert between numpy arrays, Python lists, and vision data
- `AudioHelper`: Convert between audio data formats
- Automatic conversion of Python lists to C++ vectors

### 3. New Utility Classes

#### VisionHelper
```python
import nlm

# Create vision from Python list
image_data = [
    [0.1, 0.2, 0.3],
    [0.4, 0.5, 0.6],
    [0.7, 0.8, 0.9]
]

vision = nlm.VisionHelper.createVisionFromPyList(image_data, channels=3)

# Create gradient vision
gradient_vision = nlm.VisionHelper.createGradientVision(width=64, height=48)

# Convert vision back to Python list
image_list = vision.to_list()
```

#### AudioHelper
```python
import nlm

# Create audio from Python list
audio_data = [0.1, 0.2, 0.3, 0.4, 0.5]
audio = nlm.AudioHelper.createAudioFromPyList(audio_data, sampleRate=44100)

# Create sine wave for testing
sine_wave = nlm.AudioHelper.createSineWave(
    numSamples=44100, 
    sampleRate=44100, 
    frequency=440.0,
    amplitude=0.5
)
```

#### SimulationUtils
```python
import nlm

# Create a complete agent configuration
config = nlm.createAgentConfig("my_agent")

# Create complete simulation setup
world, brain, agent_brain, config = nlm.createSimulationSetup(
    worldWidth=100.0,
    worldHeight=100.0,
    visionWidth=16,
    visionHeight=16
)

# Safe save/load with error handling
success = nlm.saveSafely(brain, "brain_checkpoint.nlm")
if success:
    print("Saved successfully!")

loaded_brain = nlm.loadSafely("brain_checkpoint.nlm", "brain")
```

### 4. Enhanced Configuration System

#### Improved Config Class
The enhanced Config class provides better Python integration:

```python
import nlm

# Create config with enhanced API
config = nlm.Config()

# Set values with type conversion
config.set("agent.name", "test_agent")
config.set("agent.sensory.input_size", 262)
config.set("agent.max_energy", 100.0)
config.set("agent.development.enabled", True)

# Get values with better error handling
name = config.get("agent.name")
if name is not None:
    print(f"Agent name: {name}")

# Get with default
energy = config.getOr("agent.energy", 100.0)
```

#### Enhanced Configuration Methods
- `get()`: Returns None instead of throwing exception
- `getOr()`: Provides default fallback values
- Type-safe setting with automatic conversions
- Better error messages for configuration issues

### 5. Advanced Simulation Features

#### Batch Operations
```python
import nlm
import time

# Create multiple brains with different configurations
brains = []
for i in range(5):
    config = nlm.createAgentConfig(f"agent_{i}")
    brain = nlm.createBrain(config)
    brains.append(brain)

# Run batch simulation
start_time = time.time()
for brain in brains:
    brain.step(100)
    brain.step(100, time.time())

print(f"Batch simulation completed in {time.time() - start_time:.2f} seconds")
```

#### Enhanced Sensory Processing
```python
import nlm

# Create agent with enhanced sensory processing
world = nlm.createSimpleWorld()
world.configure(100.0, 100.0, 16, 16)

# Get sensory percept
percept = world.getSensoryPercept()

# Process different sensory modalities
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
proprioception = percept.getProprioception()
audio = percept.getAudio()

# Set processed sensory data
new_vision = nlm.VisionHelper.createGradientVision(16, 16, 0.3, 0.8)
percept.setVision(new_vision)
```

### 6. Development and Testing Tools

#### Simulation Context Manager
The `SimulationContext` provides:
- Automatic cleanup and reset
- Progress tracking
- Early termination detection
- Statistics collection

#### Error Recovery
- Safe save/load operations
- Graceful error handling
- Logging and debugging support

### 7. Performance and Optimization

#### Efficient Memory Management
- Reduced copying between Python and C++
- Smart pointer usage
- Reference counting for better memory efficiency

#### Optimized Data Transfer
- Batch operations for bulk data transfer
- Efficient conversion utilities
- Reduced memory allocation overhead

### 8. Integration with Existing Code

#### Backward Compatibility
All existing Python APIs continue to work:

```python
# Original API still works
import nlm

brain = nlm.Brain(nlm.Config())
brain.initialize()
brain.step(100)

# New enhanced features available
vision = nlm.VisionHelper.createGradientVision(64, 48)
```

## Complete Example

Here's a complete example showing all enhanced features:

```python
import nlm
import time
import random

def main():
    print("=== Enhanced NLM Python Bindings Demo ===\n")
    
    # 1. Create simulation setup
    print("1. Creating simulation setup...")
    world, brain, agent_brain, config = nlm.createSimulationSetup(
        worldWidth=200.0,
        worldHeight=200.0,
        visionWidth=32,
        visionHeight=32
    )
    
    # 2. Configure agent
    print("2. Configuring agent...")
    config.set("agent.name", "demo_agent")
    config.set("agent.max_energy", 200.0)
    config.set("agent.energy_decay_rate", 0.05)
    
    # 3. Enhanced sensory input
    print("3. Processing sensory input...")
    from nlm import VisionHelper
    
    # Create vision from gradient
    vision_data = VisionHelper.createGradientVision(32, 32, 0.1, 0.9)
    
    # Create sensory percept
    percept = agent_brain.getSensoryInputSize()
    
    # 4. Run simulation with context manager
    print("4. Running simulation...")
    with nlm.SimulationContext(brain, max_steps=2000) as sim:
        # Set initial position
        world.setAgentStart(100.0, 100.0)
        
        # Run simulation
        results = sim.runSteps(1000)
        
        print(f"   Simulation completed: {results['steps_completed']} steps")
        print(f"   Average firing rate: {results['average_firing_rate']:.4f}")
        print(f"   Min firing rate: {results['min_firing_rate']:.4f}")
        print(f"   Max firing rate: {results['max_firing_rate']:.4f}")
        
        # Check for early termination
        if 'early_stop' in results:
            print(f"   Early termination: {results['early_stop']}")
    
    # 5. Enhanced memory and save/load
    print("5. Saving and restoring state...")
    success = nlm.saveSafely(brain, "demo_brain.nlm")
    if success:
        print("   Brain saved successfully!")
        
        # Load and continue
        loaded_brain = nlm.loadSafely("demo_brain.nlm", "brain")
        if not loaded_brain.is_none():
            print("   Brain loaded successfully!")
    
    # 6. Statistical analysis
    print("6. Statistical analysis...")
    if brain.hasattr("getDevelopmentalStage"):
        stage = brain.getDevelopmentalStage()
        print(f"   Developmental stage: {stage}")
    
    if brain.hasattr("getNeuromodulationLevel"):
        neuromod = brain.getNeuromodulationLevel()
        print(f"   Neuromodulation level: {neuromod:.4f}")
    
    # 7. Performance metrics
    print("7. Performance metrics...")
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    active_neurons = brain.getActiveNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    
    print(f"   Total neurons: {total_neurons}")
    print(f"   Total synapses: {total_synapses}")
    print(f"   Active neurons: {active_neurons}")
    print(f"   Firing neurons: {firing_neurons}")
    print(f"   Firing percentage: {firing_neurons/total_neurons*100:.1f}%")
    
    print("\n=== Demo completed successfully! ===")

if __name__ == "__main__":
    main()
```

## API Reference

### Core Classes

#### `nlm.Config`
Configuration management with enhanced Python features.

#### `nlm.Brain`
Central neural simulation brain with enhanced Python interface.

#### `nlm.AgentBrain`
Agent interface connecting brain to world with enhanced features.

#### `nlm.SimpleWorld`
Simple 2D world with enhanced Python interface.

#### `nlm.SensoryInput`
Base class for sensory input with validation.

### Utility Classes

#### `nlm.VisionHelper`
Utilities for working with vision data.

#### `nlm.AudioHelper`
Utilities for working with audio data.

#### `nlm.SimulationUtils`
Utilities for simulation setup and management.

#### `nlm.SimulationContext`
Context manager for safe simulation execution.

### Factory Functions

- `nlm.createDefaultConfig()`: Create default configuration
- `nlm.createBrain(config)`: Create brain with initialization
- `nlm.createSimpleWorld()`: Create simple world
- `nlm.createAgentBrain(brain)`: Create agent brain interface
- `nlm.createAgentConfig(name)`: Create agent configuration
- `nlm.createSimulationSetup(...)`: Create complete simulation setup

### Constants and Utilities

- `nlm.INVALID_NEURON_ID`: Invalid neuron identifier
- `nlm.INVALID_SYNAPSE_ID`: Invalid synapse identifier
- `nlm.INVALID_REGION_ID`: Invalid region identifier
- `nlm.INVALID_POPULATION_ID`: Invalid population identifier
- `nlm.PI`: Mathematical constant π
- `nlm.E`: Mathematical constant e
- `nlm.__version__`: Package version
- `nlm.__description__`: Package description

## Error Handling

The enhanced bindings provide comprehensive error handling:

```python
try:
    # Python code that might throw exceptions
    result = some_operation()
except nlm.PyNLMError as e:
    print(f"NLM error: {e}")
except nlm.PyValidationError as e:
    print(f"Validation error: {e}")
except nlm.PyConfigurationError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Installation and Setup

### Prerequisites
- Python 3.8 or higher
- CMake 3.16 or higher
- pybind11
- C++ compiler with C++20 support

### Building from Source

```bash
# Clone the repository
mkdir nlm-pybindings
cd nlm-pybindings
# The enhanced bindings are included in the main NLM repository

cd nlm/build
mkdir python_bindings
# Run CMake configuration and build the Python module
```

### Installation

The enhanced Python bindings are designed to be installed as part of the main NLM package.

## Migration Guide

### From Original Bindings to Enhanced Bindings

The enhanced bindings maintain backward compatibility while adding new features:

**Changes that maintain compatibility:**
- All existing method signatures work identically
- Return types and behavior are preserved
- Error handling is more robust but throws similar exceptions

**New features to explore:**
- Context managers for safe simulation execution
- Enhanced error handling and validation
- Helper classes for vision/audio processing
- Factory functions for common setup patterns
- Safe save/load operations
- Statistical analysis utilities

## Testing

The enhanced bindings include comprehensive testing:
- Unit tests for individual classes and functions
- Integration tests for complex scenarios
- Performance benchmarks
- Error handling tests
- Memory management tests

To run tests:
```bash
cd tests
./nlm_test  # Run C++ tests
```

For Python testing (when available):
```python
# Python tests are in the tests directory
python -m pytest tests/ -v
```

## Troubleshooting

### Common Issues

1. **Module not found**
   ```python
   # Ensure the enhanced bindings are built and installed
   pip install nlm-pybindings
   ```

2. **Import errors**
   ```python
   # Check that all dependencies are installed
   # Ensure the module is in your Python path
   ```

3. **Compilation errors**
   ```bash
   # Ensure you have the required build dependencies
   # Build with: cmake .. -DCMAKE_BUILD_TYPE=Release
   # Then: make -j4
   ```

4. **Performance issues**
   ```python
   # For large simulations, use batch operations
   # Use context managers for better resource management
   # Consider parallel processing for multiple instances
   ```

### Getting Help

For issues with the enhanced Python bindings:
1. Check the documentation for the specific feature
2. Review the examples
3. Check the error messages for specific details
4. Report issues with detailed error information

## Version History

### Version 2.0.0-enhanced
- Complete rewrite of Python bindings with enhanced error handling
- Added context managers for simulation safety
- Enhanced sensory processing utilities
- Improved configuration API
- Added helper classes (VisionHelper, AudioHelper, SimulationUtils)
- Comprehensive documentation and examples
- Backward compatibility maintained

### Version 1.0.0 (Original)
- Basic Python bindings for NLM framework
- Core functionality for neural simulation
- Basic configuration and world interaction

## Future Enhancements

The enhanced Python bindings provide a foundation for future improvements:

1. **Advanced Visualization**
   - Real-time plotting capabilities
   - Interactive visualization tools
   - 3D neural network visualization

2. **Machine Learning Integration**
   - Integration with popular ML libraries
   - Advanced pattern recognition
   - Learning rate optimization

3. **Distributed Computing**
   - Multi-process simulation
   - GPU acceleration
   - Cloud deployment capabilities

4. **Advanced APIs**
   - Additional neural network types
   - Enhanced plasticity rules
   - Advanced neuromodulation models

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface to the sophisticated neural simulation framework. With improved error handling, enhanced APIs, utility classes, and comprehensive documentation, the bindings make it easy to:

- Create and run neural simulations
- Analyze results and statistics
- Integrate NLM into larger applications
- Extend and customize for specific use cases

The bindings maintain backward compatibility while providing powerful new features for both beginners and advanced users.
