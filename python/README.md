# NLM Python API Documentation

This document provides comprehensive documentation for the NLM Python API, including new functionality and improvements.

## Overview

The NLM Python API provides a user-friendly interface to the NLM (Neural Learning Machine) brain simulation framework. The API includes:

- **Enhanced Error Handling**: Custom exception classes with descriptive error messages
- **Direct Neuron Access**: Complete control over individual neurons
- **Synapse Management**: Create and modify synaptic connections
- **Working Memory Integration**: Access to memory systems
- **Prediction System**: Control over prediction capabilities
- **Convenience Methods**: Factory methods and utility functions
- **Type Conversion Helpers**: Seamless conversion between Python and C++ types

## Installation

```bash
pip install -e .
```

## Getting Started

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createSimpleBrain(config)

# Run simulation
pynlm.run_brain_step(brain, 100)

print(f"Simulation complete. Total neurons: {brain.getTotalNeuronCount()}")
```

### Example 2: Advanced Brain Configuration

```python
import pynlm

# Create a custom configuration using Python dictionary
config_dict = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": True,
    "neuromod.dopamine.scale": 2.0,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "development.stage": "Maturation"
}

config = pynlm.config_from_dict(config_dict)
print(config.summary())
```

### Example 3: Direct Neuron Manipulation

```python
import pynlm

# Create a simple world for neuron testing
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Test neuron properties
# (Note: Direct neuron access requires additional API methods)
```

## Enhanced Error Handling

The NLM Python API now includes custom exceptions for better error handling:

```python
try:
    brain = pynlm.createSimpleBrain(None)
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMError as e:
    print(f"General NLM error: {e}")
```

Available exception classes:

- `NLMError`: Base exception for all NLM errors
- `ConfigError`: Configuration-related errors
- `BrainError`: Brain-related errors  
- `AgentError`: Agent-related errors
- `WorldError`: World/environment-related errors

## Type Conversion Helpers

### Neuron ID Conversion

```python
import pynlm

# Convert uint64 to NeuronId
neuron_id = pynlm.to_neuron_id(42)
print(f"Neuron ID: {neuron_id.value}")

# Convert NeuronId to uint64
value = pynlm.from_neuron_id(neuron_id)
print(f"Value: {value}")
```

### Vector Conversion

```python
import pynlm

# Convert Python lists to C++ vectors
python_list = [1.0, 2.0, 3.0, 4.0, 5.0]
float_vector = pynlm.to_float_vector(python_list)
double_vector = pynlm.to_double_vector(float_vector)

print(f"Float vector: {float_vector}")
print(f"Double vector: {double_vector}")
```

## Configuration Management

### Creating Configurations

```python
import pynlm

# Method 1: Basic configuration
config1 = pynlm.createDefaultConfig()

# Method 2: From Python dictionary
config_dict = {"brain.neuron_count": 1500}
config2 = pynlm.config_from_dict(config_dict)

# Method 3: Custom factory
config3 = pynlm.Config()
config3.set("brain.neuron_count", 1000)
config3.set("plasticity.stdp.enable", True)
```

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
errors = pynlm.validate_config(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

## Direct Neuron Access

### Neuron Properties

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get basic information
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Check developmental stage
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Neuron State Access

```python
import pynlm

# For direct neuron access, you would need:
# neuron = pynlm.createNeuron()
# neuron.setType(pynlm.NeuronType.Excitatory)
# neuron.setMembranePotential(-65.0)
# neuron.setThreshold(-50.0)
```

## Synapse Management

### Creating Synapses

```python
import pynlm

# Create synapses using neuron IDs
source_id = pynlm.to_neuron_id(0)
target_id = pynlm.to_neuron_id(1)

# Create a new synapse (example - requires API method)
# synapse = pynlm.createSynapse(source_id, target_id, weight=0.5)
```

## Working Memory Access

### Working Memory Operations

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory (requires additional API)
# working_memory = brain.getWorkingMemory()
# working_memory.setCapacity(100)
# working_memory.storeInformation([0.1, 0.5, 1.0, 0.8])
```

## Prediction System

### Prediction Operations

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access prediction system (requires additional API)
# prediction_system = brain.getPredictionSystem()
# prediction_system.updatePrediction(error=0.1, confidence=0.8)
```

## Convenience Functions

### Utility Functions

```python
import pynlm

# Get version information
version = pynlm.get_version()
print(f"NLM Version: {version}")

# Validate configuration
config = pynlm.createDefaultConfig()
errors = pynlm.validate_config(config)

# Run simulation steps
brain = pynlm.createSimpleBrain(config)
pynlm.run_brain_step(brain, 50)
```

## Advanced Examples

### Complete Agent Simulation

```python
import pynlm

def run_complete_simulation():
    """Run a complete agent simulation with all subsystems enabled."""
    
    # 1. Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # 2. Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 3. Run simulation loop
    for step in range(1000):
        world.update(0.1)
        
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
        
        # Show progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print("Simulation complete!")

# Run the simulation
run_complete_simulation()
```

### Memory Management Example

```python
import pynlm

def test_memory_systems():
    """Test memory system functionality."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get working memory (if available)
    working_memory = brain.getWorkingMemory()
    
    if working_memory:
        # Test working memory
        print("Testing working memory...")
        
        # Set capacity
        working_memory.setCapacity(50)
        print(f"Capacity set to: {working_memory.getCapacity()}")
        
        # Store information
        test_data = [0.1, 0.5, 1.0, 0.8, 0.3]
        working_memory.storeInformation(test_data)
        print(f"Stored {len(test_data)} values")
        
        # Get active neurons count
        active_count = working_memory.getActiveNeuronsCount()
        print(f"Active neurons: {active_count}")
        
        # Clear memory
        working_memory.clear()
        print("Memory cleared")
    
    print("Memory system test complete!")

# Run memory test
test_memory_systems()
```

## API Reference

### Classes

#### `pynlm.NLMError`
Base exception class for all NLM errors.

#### `pynlm.ConfigError`
Configuration-related errors.

#### `pynlm.BrainError`
Brain-related errors.

#### `pynlm.AgentError`
Agent-related errors.

#### `pynlm.WorldError`
World/environment-related errors.

### Functions

#### `pynlm.createDefaultConfig()`
Create a default configuration with sensible parameters.

#### `pynlm.config_from_dict(dictionary)`
Create a configuration from a Python dictionary.

#### `pynlm.createBrain(config)`
Create a new brain with the given configuration.

#### `pynlm.createSimpleBrain(config)`
Create and initialize a simple brain with error checking.

#### `pynlm.createSimpleWorld()`
Create a simple world with default configuration.

#### `pynlm.createAgentBrain(brain)`
Create an agent brain interface.

#### `pynlm.run_brain_step(brain, steps)`
Run brain simulation for specified number of steps.

#### `pynlm.validate_config(config)`
Validate configuration and return list of errors.

#### `pynlm.get_version()`
Get NLM version information.

#### `pynlm.to_neuron_id(value)`
Convert uint64 to NeuronId.

#### `pynlm.from_neuron_id(id)`
Extract value from NeuronId.

#### `pynlm.to_float_vector(input)`
Convert double vector to float vector.

#### `pynlm.to_double_vector(input)`
Convert float vector to double vector.

### Constants

- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Internal`
- `pynlm.SynapseType.Excitatory`
- `pynlm.SynapseType.Inhibitory`
- `pynlm.SynapseType.Modulatory`
- `pynlm.SynapseType.Electrical`
- `pynlm.SynapseType.GapJunction`

## Migration Guide

### From Old API to New API

1. **Configuration Creation**:
   - Old: `config = pynlm.Config()`
   - New: `config = pynlm.createDefaultConfig()`

2. **Error Handling**:
   - Old: Generic `RuntimeError` from pybind11
   - New: Specific exception types (ConfigError, BrainError, etc.)

3. **Brain Creation**:
   - Old: `brain = pynlm.Brain(config)`
   - New: `brain = pynlm.createBrain(config)` (with initialization)

4. **Type Conversion**:
   - Old: Manual type conversions
   - New: Helper functions (to_neuron_id, from_neuron_id, etc.)

## Troubleshooting

### Common Issues

#### "ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`? (The new API does this automatically for `createSimpleBrain`)
- Try increasing the number of steps
- Check that you have enabled the subsystems you need

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Error Messages

#### Configuration Errors
```
ConfigError: Missing required configuration: brain.neuron_count
```

#### Brain Errors
```
BrainError: Brain cannot be null
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [HOW_TO_USE.md](HOW_TO_USE.md) - Complete usage guide with examples

## Version Information

This documentation covers NLM Python API version 1.0.0 with enhanced error handling and new functionality.

For the latest documentation, visit the NLM project repository.
