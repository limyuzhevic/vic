# NLM (熙然) - User Guide and Best Practices

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial brain project that simulates neural computation through spiking neurons, synapses, and integrated memory systems. This document provides comprehensive guidance for using NLM effectively.

## Quick Start

### Installation

1. **Build the project**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python bindings**:
   ```bash
   pip install -e .
   ```

### Basic Usage

```python
from pynlm import createDefaultConfig, createBrain, createSimpleWorld

# Create configuration
config = createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 4)

# Create brain
brain = createBrain(config)
brain.initialize()

# Create world and agent
world = createSimpleWorld()
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
world.setAgentStart(50, 50)

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process and act
    # brain.receiveSensoryInput(percept)
    # action = brain.produceAction()
    # world.applyMotorCommand(action, step)

brain.logStatus()
```

## API Documentation

### Brain Class

The `Brain` class is the core of the NLM system, representing an integrated artificial brain.

#### Initialization and Configuration

**`brain = createBrain(config)`**
- Creates a new brain with the given configuration
- `config`: Configuration object from `createDefaultConfig()`

**`brain.initialize()`**
- Initializes the brain with all neural components
- Sets up regions, neurons, synapses, and memory systems
- Returns `True` on success, `False` on failure

#### Core Operations

**`brain.step(step, time=None)`**
- Performs one simulation step of the brain loop
- `step`: Simulation step number
- `time`: Optional timestamp (defaults to step * timestep)

**`brain.receiveSensoryInput(input)`**
- Injects sensory input into the brain
- `input`: Vision, Audio, or InternalSignals object

**`brain.produceAction()`**
- Analyzes motor neuron activity and selects appropriate action
- Returns Action object with type and parameters

#### Memory System Access

**`brain.getWorkingMemory()`**
- Accesses working memory for current activity traces
- Returns NeuralWorkingMemory object or `None`

**`brain.getEpisodicMemory()`**
- Accesses episodic memory for experience storage
- Returns NeuralEpisodicMemory object or `None`

**`brain.getAssociativeMemory()`**
- Accesss associative memory for pattern associations
- Returns NeuralAssociativeMemory object or `None`

#### State Management

**`brain.save(filepath)`**
- Saves brain state to file (checkpointing)
- `filepath`: Path to save brain state

**`brain.load(filepath)`**
- Loads brain state from file
- `filepath`: Path to load brain state from

**`brain.reset()`**
- Resets brain to initial state
- Clears all activity, memories, and synaptic states

### Configuration

#### Configuration Parameters

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | integer | 1000 | Total number of neurons |
| `region_count` | integer | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |
| `simulation_timestep` | float | 0.001 | Time step for simulation (seconds) |
| `replay_interval` | integer | 100 | Steps between memory replay |
| `consolidation_interval` | integer | 1000 | Steps between memory consolidation |

#### Creating Configuration

```python
from pynlm import createDefaultConfig

config = createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 4)
config.set("connection_probability", 0.15)

# Save custom configuration
config.saveToFile("my_config.json")
```

### Sensory Input

#### Vision Input

**`vision = Vision(width, height, channels=3)`**
- Creates visual sensory input
- `width`: Image width in pixels
- `height`: Image height in pixels
- `channels`: Number of color channels (1=grayscale, 3=RGB)

**`vision.setData(data)`**
- Sets vision data array
- `data`: Flat array of normalized float values

#### Audio Input

**`audio = Audio(sampleRate, numSamples)`**
- Creates audio sensory input
- `sampleRate`: Samples per second
- `numSamples`: Number of audio samples

**`audio.setData(data)`**
- Sets audio data array
- `data`: Array of audio sample values

#### Internal Signals

**`internal = InternalSignals()`**
- Creates internal neural signal input
- Use `internal.addSignal(value)` for individual signals

### Actions and Motor Commands

#### Action Types

| ActionType | Description |
|------------|-------------|
| MoveForward | Move forward |
| MoveBackward | Move backward |
| MoveLeft | Move left |
| MoveRight | Move right |
| TurnLeft | Turn left |
| TurnRight | Turn right |
| Look | Look forward |
| LookUp | Look upward |
| LookDown | Look downward |
| Interact | Interact with object |
| Eat | Eat food |
| Drink | Drink water |
| Rest | Rest/stay idle |
| Wait | Wait (no action) |
| Custom | Custom action |

#### Creating Actions

```python
from pynlm import Action, ActionType

# Simple action
action = Action(ActionType.MoveForward)

# Action with parameters
action = Action(ActionType.Custom, [0.5, 0.8, 0.3])
```

### Best Practices

#### 1. Configuration Optimization

```python
# Start with reasonable defaults
config = createDefaultConfig()

# Adjust based on available memory and desired complexity
if available_memory < 1024:
    config.set("neuron_count", 100)
    config.set("region_count", 2)
```

#### 2. Step Size Considerations

```python
# Use appropriate step sizes for different use cases
brain.step(step)  # Default 1ms steps for detailed simulation
```

#### 3. Memory Management

```python
# Check memory system availability before use
wm = brain.getWorkingMemory()
if wm:
    wm.setCapacity(brain.getTotalNeuronCount() // 10)
```

#### 4. Error Handling

```python
# Always check initialization
if not brain.initialize():
    print("Brain initialization failed!")
    sys.exit(1)

# Handle save/load errors gracefully
try:
    if not brain.save("checkpoint.nlm"):
        print("Failed to save checkpoint")
except Exception as e:
    print(f"Error saving checkpoint: {e}")
```

#### 5. Simulation Monitoring

```python
# Monitor brain state periodically
if step % 100 == 0:
    brain.logStatus()
    
    # Check critical thresholds
    firing_ratio = brain.getAverageFiringRate()
    if firing_ratio > 50.0:  # High firing rate
        print(f"High firing rate detected: {firing_ratio}")
```

### Troubleshooting Guide

#### Common Issues and Solutions

**1. Brain Initialization Fails**
- **Cause**: Invalid configuration parameters
- **Solution**: Check configuration values and ensure all required keys are present
- **Command**: `brain.logStatus()` to see initialization details

**2. Memory Systems Return None**
- **Cause**: Memory systems not properly initialized
- **Solution**: Check if memory systems are properly configured
- **Command**: Verify configuration includes memory system settings

**3. High Memory Usage**
- **Cause**: Too many neurons or regions
- **Solution**: Reduce neuron count or region count
- **Command**: `config.set("neuron_count", 100)`

**4. Simulation Instability**
- **Cause**: Unstable neural dynamics
- **Solution**: Check connection probability and synaptic weights
- **Command**: Use `config.set("connection_probability", 0.05)` for stability

**5. Sensory Input Processing Issues**
- **Cause**: Mismatched input dimensions
- **Solution**: Match input dimensions to sensory neuron count
- **Command**: Check brain.getTotalNeuronCount() for expected input size

### Performance Tips

#### 1. Memory Optimization

```python
# Reduce memory footprint
config.set("neuron_count", 200)  # Fewer neurons
config.set("region_count", 2)    # Fewer regions
```

#### 2. Simulation Speed

```python
# Trade-off between detail and speed
brain.step(step)  # 1ms steps, detailed simulation
# vs
# brain.step(step * 10)  # 10ms steps, faster simulation
```

#### 3. Parallel Processing

```python
# For high-performance computing
import multiprocessing
multiprocessing.set_start_method('spawn')
```

### Advanced Features

#### 1. Checkpointing and Persistence

```python
# Save brain state periodically
if step % 1000 == 0:
    brain.save(f"checkpoint_{step}.nlm")

# Restore from checkpoint
brain.load("checkpoint_5000.nlm")
```

#### 2. Custom Configurations

```python
import json

# Create custom configuration
custom_config = {
    "neuron_count": 1000,
    "region_count": 4,
    "connection_probability": 0.2,
    "simulation_timestep": 0.001,
    "neuron_pools": {
        "sensory": 250,
        "internal": 500,
        "motor": 250
    }
}

# Save custom configuration
with open("custom_config.json", "w") as f:
    json.dump(custom_config, f, indent=2)
```

#### 3. Development Stage Control

```python
from pynlm import DevelopmentalStage

# Control brain development
brain.setDevelopmentalStage(DevelopmentalStage.Maturation)

# Monitor development
stage = brain.getDevelopmentalStage()
print(f"Current developmental stage: {stage}")
```

### Example: Complete Simulation

```python
#!/usr/bin/env python3
"""
Complete NLM simulation example demonstrating:
- Brain initialization and configuration
- World setup and agent interaction
- Sensory processing and motor control
- State management and checkpointing
"""

from pynlm import (
    createDefaultConfig, createBrain, createSimpleWorld,
    Vision, ActionType
)

def main():
    print("NLM Phase 6 Complete Integration Example")
    print("=" * 50)
    
    # Create and configure brain
    print("\n1. Creating brain configuration...")
    config = createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 3)
    config.set("connection_probability", 0.12)
    
    brain = createBrain(config)
    
    # Initialize brain
    print("2. Initializing brain...")
    if not brain.initialize():
        print("ERROR: Brain initialization failed!")
        return 1
    
    # Create world
    print("3. Creating simulation world...")
    world = createSimpleWorld()
    world.configure(80, 80, 16, 16)  # smaller world for testing
    world.setAgentStart(40, 40)
    
    # Run simulation
    print("4. Running simulation...")
    steps = 500
    checkpoint_interval = 100
    
    for step in range(steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Create simple visual input based on agent position
        vision = Vision(16, 16, 3)
        vision.setData([0.1, 0.2, 0.3] * 16 * 16)
        
        # Process sensory input
        brain.receiveSensoryInput(vision)
        
        # Produce action
        action = brain.produceAction()
        
        # Apply action to world
        world.applyMotorCommand(action, float(step))
        
        # Log periodically
        if step % checkpoint_interval == 0:
            print(f"Step {step}: Action = {action.getType()}")
            brain.logStatus()
            
            # Save checkpoint
            brain.save(f"checkpoint_step_{step}.nlm")
    
    # Final status
    print("\n5. Simulation complete!")
    print(f"Total steps: {steps}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    # Save final brain state
    brain.save("final_brain_state.nlm")
    print("\nFinal brain state saved to 'final_brain_state.nlm'")
    
    return 0

if __name__ == "__main__":
    exit(main())
```

## References

1. NLM Architecture Documentation: `/workspace/app/docs/PHASE6_ARCHITECTURE.md`
2. Scientific Background: `/workspace/app/docs/SCIENCE.md`
3. Performance Analysis: `/workspace/app/docs/PERFORMANCE.md`
4. Phase 6 Final Audit: `/workspace/app/docs/PHASE6_FINAL_AUDIT.md`

## Contact

For questions, issues, or feature requests, please refer to the NLM project documentation and issue tracker.

---

*This documentation is for NLM Phase 6 (Final Integration). Please refer to version-specific documentation for earlier phases.*