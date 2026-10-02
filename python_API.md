# NLM Python API Reference

This document provides comprehensive reference documentation for the NLM Python API.

## Overview

The NLM Python API (`pynlm`) provides a high-level interface to the NLM (Neural Learning Machine) neural simulation framework.
It exposes the full functionality of the C++ core through an intuitive Python interface with enhanced Pythonic patterns.

## Key Features

- **Context Manager Support**: Use `with` statements for resource management
- **Factory Methods**: Convenient creation methods with sensible defaults
- **Enhanced Error Handling**: Comprehensive exception handling and validation
- **Type Hints**: Python type annotations for better IDE support
- **Pythonic API**: Intuitive methods following Python conventions

## Installation

```bash
pip install pynlm
```

For development, install in editable mode:

```bash
pip install -e .
```

## Core Classes

### Config

Configuration management for NLM systems.

```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("random_seed", 12345)
```

**Methods:**
- `createDefaultConfig()` - Create config with safe defaults
- `config.has(key)` - Check if config has key
- `config.getOr(type, key, default)` - Get value with default
- `config.set(key, value, source)` - Set configuration value
- `config.saveToFile(filepath)` - Save to file
- `config.loadFromFile(filepath)` - Load from file

### Brain

Central neural simulation brain class.

```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

**Key Methods:**
- `getTotalNeuronCount()` - Total neurons
- `getTotalSynapseCount()` - Total synapses
- `getFiringNeuronCount()` - Currently firing neurons
- `getAverageFiringRate()` - Average firing rate
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

**Factory Method:**
- `Brain.createQuickBrain()` - Create and initialize test brain with 500 neurons

### SimpleWorld

2D world simulation environment.

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.update(0.1)
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `getSensoryPercept()` - Get current sensory perception
- `getAgentBody()` - Get agent physical state
- `applyMotorCommand(action, time)` - Execute action
- `reset()` - Reset world
- `update(timestep)` - Update world state

### AgentBrain

Bridge connecting NLM brain to the world.

```python
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)
```

**Key Methods:**
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableCuriosity(bool)` - Enable/disable curiosity-driven exploration

### SensoryInput Classes

Base class for all sensory inputs with context manager support.

#### Vision

Vision sensory input (image-like data).

```python
with pynlm.Vision.createImage(640, 480) as vision:
    # Process vision data
    print(f"Image size: {vision.getWidth()}x{vision.getHeight()}")
```

**Factory Methods:**
- `Vision.createImage(width, height, channels=3)` - Create vision object

**Methods:**
- `getWidth()`, `getHeight()`, `getChannels()` - Get image properties
- `setData(data)` - Set pixel data

#### Audio

Audio sensory input.

```python
with pynlm.Audio.createAudio(44100, 1024) as audio:
    # Process audio data
    print(f"Audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
```

**Factory Methods:**
- `Audio.createAudio(sampleRate, numSamples)` - Create audio object

**Methods:**
- `setSampleRate(rate)` - Set sample rate
- `getSampleRate()`, `getNumSamples()` - Get audio properties

#### InternalSignals

Internal neural signals.

```python
with pynlm.InternalSignals.createSignals() as signals:
    signals.addSignal(1.0)
    signals.addSignal(2.0)
    print(f"Signals: {signals.getData()}")
```

**Factory Methods:**
- `InternalSignals.createSignals()` - Create signals object

**Methods:**
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear all signals

### Action and WorldObject

Action representations and world objects.

```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
```

## Advanced Usage Patterns

### Complete Agent Simulation

```python
def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 3. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 4. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply it
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

### Context Manager Patterns

```python
def process_vision_with_context(width, height, channels=3):
    """Process vision data using context manager."""
    with pynlm.Vision.createImage(width, height, channels) as vision:
        # Ensure vision is properly initialized
        if not hasattr(vision, 'getData'):
            vision.setData([0.0] * (width * height * channels))
        
        # Process the image
        data = vision.getData()
        return sum(data) / len(data)  # Return average pixel value

def create_sensory_scenario():
    """Create a complete sensory scenario with multiple inputs."""
    with pynlm.Vision.createImage(320, 240) as vision, \
         pynlm.Audio.createAudio(22050, 512) as audio, \
         pynlm.InternalSignals.createSignals() as signals:
        
        # Initialize with sample data
        vision.setData([0.5] * (320 * 240 * 3))
        audio.setData([0.1] * 512)
        signals.addSignal(0.0)  # Initial state
        
        # Process the combined sensory input
        # ... (your processing logic here)
        
        return vision, audio, signals
```

### Error Handling and Validation

```python
def safe_brain_operation(brain, operation_func, *args, **kwargs):
    """Safely execute brain operations with error handling."""
    try:
        if not brain.isInitialized():
            brain.initialize()
        
        result = operation_func(brain, *args, **kwargs)
        return result
        
    except Exception as e:
        print(f"Error in brain operation: {e}")
        # Return default/fallback value
        return None

def validate_simulation_parameters(neuron_count, region_count, connection_prob):
    """Validate simulation parameters before running."""
    if neuron_count <= 0:
        raise ValueError("neuron_count must be positive")
    if region_count <= 0:
        raise ValueError("region_count must be positive")
    if not (0.0 <= connection_prob <= 1.0):
        raise ValueError("connection_prob must be between 0 and 1")
    
    # Return validated config
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connection_prob)
    
    return config
```

### Configuration Management

```python
class NLMConfigurator:
    """Helper class for managing NLM configurations."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def set_brain_parameters(self, neuron_count=1000, region_count=1):
        """Set brain-related parameters."""
        self.config.set("neuron_count", neuron_count)
        self.config.set("region_count", region_count)
        return self
    
    def set_plasticity_parameters(self, stdp_rate=0.001, hebbian_rate=0.01):
        """Set plasticity parameters."""
        self.config.set("stdp_learning_rate", stdp_rate)
        self.config.set("hebbian_learning_rate", hebbian_rate)
        return self
    
    def set_neuromodulation_parameters(self, dopamine_scale=1.0):
        """Set neuromodulation parameters."""
        self.config.set("dopamine_scale", dopamine_scale)
        return self
    
    def create_brain(self):
        """Create brain with current configuration."""
        return pynlm.createBrain(self.config)
    
    def save_config(self, filepath):
        """Save configuration to file."""
        self.config.saveToFile(filepath)
    
    def load_config(self, filepath):
        """Load configuration from file."""
        if self.config.loadFromFile(filepath):
            print(f"Configuration loaded from {filepath}")
        else:
            print(f"Warning: Could not load configuration from {filepath}")
        return self
```

## Type Hints and IDE Support

The NLM Python API includes type hints for better IDE support:

```python
# Type hints for better autocomplete and validation
from typing import Optional, List, Dict

config: pynlm.Config = pynlm.createDefaultConfig()
brain: Optional[pynlm.Brain] = None
neurons: int = brain.getTotalNeuronCount() if brain else 0
synapses: List[str] = config.getKeys()
```

## Performance Tips

1. **Reuse Objects**: Reuse Brain, AgentBrain, and World objects across simulations
2. **Configure Wisely**: Use appropriate configuration values for your use case
3. **Memory Management**: Use context managers for temporary sensory inputs
4. **Batch Operations**: Process multiple steps in batches when possible

## Common Pitfalls and Solutions

### Issue: Brain Not Initialized
```python
# Problem: Brain methods fail because initialize() wasn't called
brain.step(step)  # May crash

# Solution: Ensure initialization
if not brain.isInitialized():  # Check if method exists
    brain.initialize()
brain.step(step)
```

### Issue: Null Configuration
```python
# Problem: Passing None to createBrain()
brain = pynlm.createBrain(None)  # May crash or return invalid object

# Solution: Use factory methods with validation
brain = pynlm.Brain.createQuickBrain()  # Safe factory method
```

### Issue: Memory Leaks with Sensory Inputs
```python
# Problem: Creating many sensory objects without cleanup
for i in range(1000):
    vision = pynlm.Vision.createImage(100, 100)  # No cleanup
    # ... process vision ...

# Solution: Use context managers
for i in range(1000):
    with pynlm.Vision.createImage(100, 100) as vision:
        # ... process vision ...
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python:

**C++:**
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

**Python:**
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
```

### From Phase 2 to Phase 6 API

The NLM Python API provides access to all Phase 6 features while maintaining backward compatibility.

```python
# Phase 2 compatibility (basic brain operations)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)

# Phase 6 advanced features
brain.getDevelopmentalStage()  # Advanced developmental control
agent.enableCuriosity(True)    # Neuromodulation control
agent.applyRewardModulation(1.0, 0.5)  # Learning control
```

## Testing and Validation

Run the NLM Python test suite to verify your installation:

```bash
python test_basic_api.py
python test_factory_methods.py
python test_error_handling.py
python test_context_managers.py
```

All tests should pass, confirming that your NLM Python installation is working correctly.