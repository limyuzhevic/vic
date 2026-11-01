# NLM Python Bindings - User Guide

This document provides a comprehensive guide for using the NLM (Neural Learning Machine) Python bindings.

## Overview

The NLM Python bindings provide a high-level, Pythonic interface to the NLM C++ neural simulation framework. This module includes:

- **Enhanced error handling** with descriptive messages
- **Type hints** for better IDE support and developer experience
- **Comprehensive documentation** with examples
- **Advanced utilities** for both beginners and experts
- **Robust configuration validation** and logging
- **Complete simulation management** tools

## Quick Start

### Installation

```bash
# Install the NLM Python bindings
pip install -e .

# Or install from source
pip install .
```

### Basic Usage

```python
import pynlm

# Check version
print(f"NLM version: {pynlm.get_version()}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm
import random

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(200):
    # Add slight noise for exploration
    noise = random.uniform(-0.1, 0.1)
    world.update(0.1 + noise)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Simulation complete!")
```

## Getting Started

### 1. Basic Brain Simulation

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simple simulation
for step in range(50):
    brain.step(step)

print(f"Brain simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### 2. Brain with World Interaction

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run interaction loop
for step in range(100):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("World interaction complete!")
```

### 3. Learning Agent

```python
import pynlm

# Setup with learning capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
for step in range(300):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

print("Learning simulation complete!")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
```

## Advanced Features

### 1. Configuration Management

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Set configuration parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Get configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")
```

### 2. Error Handling

```python
import pynlm

try:
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except NLMError as e:
    print(f"NLM error: {e}")
```

### 3. Logging

```python
import pynlm

# Configure logging
pynlm.configure_logging('INFO', 'simulation.log')

# Or configure with custom format
pynlm.configure_logging('DEBUG', '%(levelname)s - %(message)s')

# Log levels:
# - DEBUG: Detailed debugging information
# - INFO: High-level operation information
# - WARNING: Warnings and potential issues
# - ERROR: Error conditions
# - CRITICAL: Critical errors
```

### 4. Simulation Summary

```python
import pynlm

# Create simulation components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get simulation summary
summary = pynlm.create_simulation_summary(brain, steps_completed=100)

print("Simulation Summary:")
print(f"  Steps completed: {summary['steps_completed']}")
print(f"  Total neurons: {summary['brain']['total_neurons']}")
print(f"  Total synapses: {summary['brain']['total_synapses']}")
print(f"  Firing neurons: {summary['brain']['firing_neurons']}")
print(f"  Total spikes: {summary['brain']['total_spikes']}")
print(f"  Average firing rate: {summary['brain']['average_firing_rate']:.1f}")
print(f"  Developmental stage: {summary['brain']['developmental_stage']}")
```

### 5. Demo and Examples

Run comprehensive examples:

```python
import pynlm

# Run demo simulation
summary = pynlm.run_demo_simulation(100)
print(f"Demo simulation completed: {summary['steps_completed']} steps")

# View all examples
from pynlm.examples import run_all_examples
run_all_examples()
```

## Type Hints and IDE Support

The NLM Python bindings include comprehensive type hints for better IDE support:

```python
from typing import List, Dict, Tuple, Optional
from pynlm import Brain, SimpleWorld, Action, Config

# Type hints improve autocompletion and type checking

def my_simulation(brain: Brain, world: SimpleWorld, steps: int) -> Dict[str, Any]:
    """Example function with type hints."""
    result = {
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'steps': steps,
    }
    return result

# Type aliases for common types
from pynlm import FloatArray, IntArray, Matrix, ConfigDict

# Use type aliases in your code
float_data: FloatArray = [1.0, 2.0, 3.0]
config: ConfigDict = {"brain.neuron_count": 1000}
```

## Enum Types

The NLM bindings include comprehensive enum types:

```python
import pynlm

# Neuron types
assert pynlm.NeuronType.EXCITATORY == "Excitatory"
assert pynlm.NeuronType.INHIBITORY == "Inhibitory"

# Developmental stages
assert pynlm.DevelopmentalStage.ADULT == "Adult"
assert pynlm.DevelopmentalStage.AGING == "Aging"

# Action types
assert pynlm.ActionType.MOVEFORWARD == "MoveForward"
assert pynlm.ActionType.TURNLEFT == "TurnLeft"

# World object types
assert pynlm.WorldObjectType.RESOURCE == "Resource"
assert pynlm.WorldObjectType.HAZARD == "Hazard"
```

## ID Types

Unique identifier types for brain components:

```python
import pynlm

# Create neuron ID
neuron_id = pynlm.NeuronId(42)
print(f"Neuron ID: {neuron_id}")
print(f"Neuron index: {neuron_id.index()}")

# Create synapse ID
synapse_id = pynlm.SynapseId(100)
print(f"Synapse ID: {synapse_id}")

# Invalid ID constants
invalid_neuron = pynlm.INVALID_NEURON_ID
print(f"Invalid neuron ID: {invalid_neuron}")
```

## Error Classes

The NLM bindings provide a comprehensive error handling system:

```python
import pynlm

# Custom exception classes
class NLMError(Exception):
    """Base exception for all NLM-related errors."""

class ConfigurationError(NLMError):
    """Raised when there's an error with configuration."""

class SimulationError(NLMError):
    """Raised when there's an error during simulation."""

class ValidationError(NLMError):
    """Raised when validation fails."""

# Use exceptions in your code
try:
    # Your NLM code here
    pass
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

## Configuration Options

The NLM configuration system allows fine-grained control over brain behavior:

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Testing and Validation

### Run the Test Suite

```bash
# Run comprehensive tests
python -m pytest python/test_improvements.py -v

# Or run the examples
python python/examples.py
```

### Configuration Validation

The NLM bindings include configuration validation:

```python
import pynlm

# Valid configuration
valid_config = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": True,
}

if pynlm.validate_config(valid_config):
    print("Configuration is valid")

# Invalid configuration (will be rejected)
invalid_config = {"brain.neuron_count": -100}
try:
    pynlm.validate_config(invalid_config)
except pynlm.ValidationError as e:
    print(f"Invalid configuration: {e}")
```

## Best Practices

### 1. Initialize Components Properly

```python
# Always initialize components in the correct order
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### 2. Use Logging for Debugging

```python
import pynlm

# Configure logging for debugging
pynlm.configure_logging('DEBUG', 'debug.log')

# Your simulation code here
# All debug information will be logged
```

### 3. Handle Errors Gracefully

```python
import pynlm

try:
    # Your NLM simulation code
    pass
except pynlm.NLMError as e:
    print(f"NLM error occurred: {e}")
    # Handle error appropriately
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle unexpected errors
```

### 4. Use Type Hints for Better Code

```python
from typing import Optional
from pynlm import Brain, SimpleWorld

# Use type hints for better IDE support
# and code documentation
def run_simulation(
    brain: Optional[Brain] = None,
    world: Optional[SimpleWorld] = None,
    steps: int = 100
) -> dict:
    """Run a simulation with type hints."""
    # Your implementation here
    return {}
```

### 5. Validate Configuration Before Use

```python
import pynlm

# Validate configuration before creating brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

if not pynlm.validate_config({
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
}):
    print("Configuration is invalid!")
    exit(1)

# Configuration is valid
brain = pynlm.createBrain(config)
```

## Troubleshooting

### Common Issues

**Issue: "My brain isn't doing anything"**
- **Solution**: Did you call `brain.initialize()` before `brain.step()`?
- **Fix**: Always initialize the brain before using it

**Issue: "The agent isn't moving"**
- **Solution**: Did you call `world.applyMotorCommand()`?
- **Fix**: Ensure the agent's actions are being applied to the world

**Issue: "Everything is 0"**
- **Solution**: Brains need time to "warm up"
- **Fix**: Run more simulation steps, or add sensory input

**Issue: "ImportError: No module named 'pynlm'"**
- **Solution**: Install the NLM Python bindings
- **Fix**: `pip install -e .` or `pip install .`

**Issue: "Segmentation fault"**
- **Solution**: Check for initialization issues and invalid configurations
- **Fix**: Ensure all components are properly initialized

### Getting Help

1. **Documentation**: See `easy_usage.md` for beginner examples
2. **Technical Guide**: See `HOW_TO_USE.md` for detailed information
3. **Architecture**: See `docs/ARCHITECTURE.md` for system details
4. **Examples**: See `python/examples.py` for comprehensive examples
5. **Error Messages**: Check the error messages for specific guidance

## Future Enhancements

The NLM Python bindings provide a foundation for future enhancements:

1. **Advanced neural models**: More sophisticated neuron types
2. **3D world simulation**: Complex 3D environments
3. **Machine learning integration**: Model training capabilities
4. **Distributed computing**: Parallel simulation support
5. **Web interfaces**: Jupyter notebook support
6. **Mobile platforms**: Android and iOS bindings
7. **Cloud integration**: AWS Lambda and Google Cloud support

## Conclusion

The NLM Python bindings provide a comprehensive, Pythonic interface to the NLM neural simulation framework. The improvements include:

- **Better usability** for beginners
- **Enhanced functionality** for advanced users
- **Robust error handling** and validation
- **Comprehensive documentation** and examples
- **Type hints** for better IDE support
- **Advanced utilities** and monitoring tools

These improvements make the NLM Python bindings significantly more accessible, robust, and powerful while maintaining full backward compatibility.

For more information, see:
- `easy_usage.md` - Beginner-friendly examples
- `HOW_TO_USE.md` - Comprehensive technical documentation
- `docs/ARCHITECTURE.md` - System architecture details
- `python/examples.py` - Comprehensive examples
- `python/test_improvements.py` - Test suite