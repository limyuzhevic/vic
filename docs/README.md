# NLM Python Bindings - Documentation

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that simulates neural networks with biologically inspired mechanisms.

This document provides comprehensive information about the Python bindings for NLM.

## Installation

### Prerequisites

- Python 3.8+
- C++ compiler with C++20 support
- CMake 3.16+

### Standard Installation

```bash
pip install pynlm
```

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library and Python bindings
make -j4

# Install the Python package
pip install .
```

## Quick Start

### Basic Usage

```python
import pynlm
import math

# Create a configuration with custom settings
config = pynlm.Config()
config.set("neuron_count", 2000)      # Increase neuron count
config.set("region_count", 3)         # Create 3 brain regions
config.set("learning_rate", 0.02)     # Adjust learning rate
config.set("enabled", True)            # Enable all features

# Create and initialize the brain
brain = pynlm.Brain(config)
brain.initialize()

# Create a simulation world
world = pynlm.SimpleWorld()
world.configure(1000, 800, 100, 80)  # Large world for complex scenarios
world.setAgentStart(500, 400)        # Set agent starting position

# Create agent brain interface
agent_brain = pynlm.AgentBrain(brain)
agent_brain.initialize(world)

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Brain regions: {brain.getRegionCount()}")
```

### Complete Simulation Example

```python
import pynlm
import random
import time

def run_simulation(seed=42, duration=500):
    """Run a complete NLM simulation with reward-based learning"""
    
    # Set random seed for reproducibility
    random.seed(seed)
    
    # Create configuration with optimal settings
    config = pynlm.Config()
    config.set("neuron_count", 3000)
    config.set("region_count", 4)
    config.set("random_seed", seed)
    config.set("simulation_timestep", 0.001)
    config.set("replay_interval", 200)
    config.set("consolidation_interval", 1000)
    
    # Initialize brain
    brain = pynlm.Brain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.SimpleWorld()
    world.configure(800, 600, 80, 60)
    world.setAgentStart(400, 300)
    
    agent_brain = pynlm.AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable learning systems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableDevelopment(True)
    
    print(f"=== NLM Simulation Started ===")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Simulation steps: {duration}")
    print()
    
    # Main simulation loop
    for step in range(duration):
        # Get current sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Produce action based on brain activity
        action = brain.produceAction()
        
        # Apply action in the world
        result = world.applyMotorCommand(action, step)
        
        # Calculate reward based on outcome
        reward = 0.0
        if result.success:
            reward = result.reward
            # Positive reinforcement for successful actions
            if result.reward > 0:
                agent_brain.applyRewardModulation(reward, result.reward * 0.5)
        
        # Update development system
        agent_brain.updateDevelopment(1)
        
        # Periodic status logging
        if step % 100 == 0:
            status = brain.logStatus()
            print(f"Step {step:4d}: {status.strip()}")
        
        # Save checkpoint periodically
        if step % 500 == 0 and step > 0:
            filename = f"checkpoints/brain_checkpoint_{step}.json"
            brain.save(filename)
            print(f"Checkpoint saved: {filename}")
    
    # Final statistics
    print()
    print("=== Simulation Complete ===")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Save final brain state
    brain.save("final_brain_state.json")
    print("Final brain state saved: final_brain_state.json")
    
    return brain, world, agent_brain

# Run the simulation
if __name__ == "__main__":
    brain, world, agent_brain = run_simulation(duration=1000)
```

### Advanced Usage Examples

#### 1. Memory Systems Exploration

```python
import pynlm

# Create brain with memory systems
config = pynlm.Config()
config.set("neuron_count", 5000)
config.set("working_memory_size", 1000)
config.set("episodic_memory_size", 500)

brain = pynlm.Brain(config)
brain.initialize()

# Access memory systems (these would need to be added to the enhanced bindings)
# working_memory = brain.getWorkingMemory()
# episodic_memory = brain.getEpisodicMemory()
# associative_memory = brain.getAssociativeMemory()

print("Memory systems ready for exploration")
```

#### 2. Neuromodulation Control

```python
import pynlm

# Create brain
brain = pynlm.Brain()
brain.initialize()

# Create agent brain
world = pynlm.SimpleWorld()
agent_brain = pynlm.AgentBrain(brain)
agent_brain.initialize(world)

# Fine-tune neuromodulation
agent_brain.enableRewardModulation(True)
agent_brain.enableCuriosity(True)
agent_brain.enableNovelty(True)

# Monitor neuromodulation levels
for step in range(100):
    dopamine_level = agent_brain.getNeuromodulationLevel()
    curiosity_level = agent_brain.getCuriosityLevel()
    novelty_level = agent_brain.getNoveltyLevel()
    
    print(f"Step {step}: Dopamine={dopamine_level:.3f}, "
          f"Curiosity={curiosity_level:.3f}, Novelty={novelty_level:.3f}")
    
    # Update development
    agent_brain.updateDevelopment(1)
```

#### 3. Development and Plasticity

```python
import pynlm

# Create brain with developmental capabilities
config = pynlm.Config()
config.set("neuron_count", 3000)
config.set("structural_plasticity", True)

brain = pynlm.Brain(config)
brain.initialize()

# Create agent brain
world = pynlm.SimpleWorld()
agent_brain = pynlm.AgentBrain(brain)
agent_brain.initialize(world)

# Enable development systems
agent_brain.enableDevelopment(True)
agent_brain.enableStructuralPlasticity(True)

# Track developmental stage
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

for step in range(500):
    # Update development
    agent_brain.updateDevelopment(1)
    
    # Get current developmental stage
    stage = brain.getDevelopmentalStage()
    
    if step % 100 == 0:
        print(f"Step {step}: Developmental stage = {stage}")
        
        # Modify plasticity based on developmental stage
        if stage == pynlm.DevelopmentalStage.CriticalPeriod:
            print("Critical period - enhanced plasticity")
        elif stage == pynlm.DevelopmentalStage.Maturation:
            print("Maturation phase - balanced plasticity")
        elif stage == pynlm.DevelopmentalStage.Adult:
            print("Adult phase - stable plasticity")
```

#### 4. Prediction and Planning

```python
import pynlm

# Create brain with prediction capabilities
config = pynlm.Config()
config.set("neuron_count", 4000)
config.set("prediction_system", True)
config.set("planning_horizon", 10)

brain = pynlm.Brain(config)
brain.initialize()

# Create agent brain
world = pynlm.SimpleWorld()
agent_brain = pynlm.AgentBrain(brain)
agent_brain.initialize(world)

# Monitor prediction system
for step in range(100):
    # Update development (includes prediction)
    agent_brain.updateDevelopment(1)
    
    # Get prediction error
    prediction_error = agent_brain.getPredictionError()
    
    # Get novelty level
    novelty_level = agent_brain.getNoveltyLevel()
    
    print(f"Step {step}: Prediction error = {prediction_error:.4f}, "
          f"Novelty = {novelty_level:.4f}")
```

## API Reference

### Core Classes

#### Config
- `Config()`: Create empty configuration
- `Config.loadFromFile(filepath)`: Load from JSON or simple format
- `Config.saveToFile(filepath)`: Save to JSON or simple format
- `Config.set(key, value, source)`: Set configuration value
- `Config.get(key)`: Get configuration value (type-safe)
- `Config.getOr(key, default)`: Get with default fallback
- `Config.validate()`: Validate configuration
- `Config.getStats()`: Get configuration statistics

#### Brain
- `Brain(config)`: Create brain with configuration
- `Brain.initialize()`: Initialize brain
- `Brain.step(step)`: Perform simulation step
- `Brain.step(step, time)`: Perform step with timestamp
- `Brain.produceAction()`: Produce motor action
- `Brain.reset()`: Reset brain state
- `Brain.save(filepath)`: Save brain state
- `Brain.load(filepath)`: Load brain state
- `Brain.getTotalNeuronCount()`: Get total neuron count
- `Brain.getTotalSynapseCount()`: Get total synapse count
- `Brain.getActiveNeuronCount()`: Get active neuron count

#### SimpleWorld
- `SimpleWorld()`: Create world instance
- `SimpleWorld.configure(width, height, visionWidth, visionHeight)`: Configure world
- `SimpleWorld.setAgentStart(x, y)`: Set agent start position
- `SimpleWorld.update(timestep)`: Update world state
- `SimpleWorld.applyMotorCommand(action, time)`: Apply action
- `SimpleWorld.getSensoryPercept()`: Get sensory percept

#### AgentBrain
- `AgentBrain(brain)`: Create agent brain interface
- `AgentBrain.initialize(world)`: Initialize with world
- `AgentBrain.processSensoryInput(percept)`: Process sensory input
- `AgentBrain.decodeMotorCommand()`: Decode motor command
- `AgentBrain.applyRewardModulation(reward, predictedReward)`: Apply reward

### Utility Functions

#### Sensory Percept Creation
- `pynlm.createVisionPercept(data, width, height, channels=3)`: Create vision percept
- `pynlm.createAudioPercept(data, sampleRate)`: Create audio percept
- `pynlm.createInternalSignalsPercept()`: Create internal signals percept

#### Action Creation
- `pynlm.createAction(type, params=[])`: Create action with parameters

#### Mathematical Utilities
- `pynlm.clamp(value, min, max)`: Clamp value to range
- `pynlm.normalize(value, min, max)`: Normalize value to [0,1]
- `pynlm.calculateDistance(x1, y1, x2, y2)`: Calculate distance

#### Constants
- `pynlm.PI`: Pi constant
- `pynlm.EPSILON`: Small epsilon value

### Constants and Enumerations

#### ActionType
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

#### WorldObjectType
- `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

#### NeuronType
- `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`

#### SynapseType
- `Excitatory`, `Inhibitory`, `Modulatory`, `Electrical`, `GapJunction`

#### DevelopmentalStage
- `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`

#### FiringState
- `Resting`, `Active`, `Refractory`, `Inhibited`

### Exception Types

- `pynlm.NLMException`: Base NLM exception
- `pynlm.ValueError`: Value-related errors
- `pynlm.IndexError`: Index-related errors
- `pynlm.ConfigError`: Configuration-related errors

## Performance Tips

### Memory Management
1. Use context managers for resource cleanup:
```python
with brain:  # Automatically resets brain on exit
    brain.initialize()
    # Do work
```

2. Reuse brain instances instead of creating new ones

3. Monitor memory usage with `brain.getStats()`

### Simulation Optimization
1. Adjust neuron count based on hardware capabilities:
```python
# For lightweight simulations
config.set("neuron_count", 500)

# For high-performance simulations
config.set("neuron_count", 10000)
```

2. Use checkpointing for long simulations:
```python
# Save checkpoints every 1000 steps
if step % 1000 == 0:
    brain.save(f"checkpoint_{step}.json")
```

3. Monitor simulation statistics:
```python
if step % 100 == 0:
    brain.logStatus()
```

### Configuration Best Practices

1. Use descriptive key names:
```python
config.set("sensory_input_width", 80)
config.set("sensory_input_height", 60)
config.set("motor_output_size", 10)
```

2. Validate configuration before use:
```python
config = pynlm.Config()
if config.validate():
    # Safe to use
else:
    # Handle invalid configuration
```

3. Use type-safe access:
```python
# Safe - returns Optional
value = config.get("learning_rate")

# Safe - with default
value = config.getOr("learning_rate", 0.01)
```

## Troubleshooting

### Common Issues

#### Issue: "ModuleNotFoundError: No module named 'pynlm'"
**Solution**: Ensure the module is installed:
```bash
pip install pynlm
```

#### Issue: "ImportError: libpynlm.cpython-310-x86_64-linux-gnu.so: undefined symbol: pybind11::cpp_function"
**Solution**: Reinstall the module:
```bash
pip install --force-reinstall pynlm
```

#### Issue: Simulation crashes with "Segmentation fault"
**Solution**: Reduce complexity:
```python
config.set("neuron_count", 500)  # Reduce from 5000
config.set("region_count", 1)     # Reduce regions
```

#### Issue: Configuration file not found
**Solution**: Use absolute paths or ensure file exists:
```python
import os
filepath = os.path.join(os.getcwd(), "config.json")
config.loadFromFile(filepath)
```

### Getting Help

For additional support:
1. Check the NLM documentation online
2. Report issues on the GitHub repository
3. Join the NLM community for discussions
4. Review example simulations in the repository

## License

This software is licensed under the MIT License. See the LICENSE file for details.

## Authors

NLM Project Contributors

## Version

pynlm 1.0.0

## Build Information

- Build date: $(date)
- Python version: $(python --version)
- Compiler: $(c++ --version | head -1)
- Build type: Release
