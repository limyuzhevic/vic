# NLM - Enhanced Python Bindings Documentation

This document provides comprehensive information about the improved NLM Python bindings.

## Overview

The NLM (Neural Learning Machine) Python bindings provide a high-level, Pythonic interface to the C++ neural simulation framework. The enhanced version includes:

- Better error handling and validation
- Pythonic convenience methods
- Advanced features for experienced users
- Comprehensive documentation and examples
- Performance monitoring tools

## Installation

### From Source (Recommended)

```bash
# Clone the repository
 git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the C++ library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
mkdir -p python_build
cd python_build
cmake ../python -DPYTHON_EXECUTABLE=$(which python3)
make -j4

# Install
pip install -e .  # From the project root
```

### Using pip (Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install in development mode
pip install -e .
```

## Basic Usage

### Quick Start

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Example

```python
import pynlm
import time

def run_complete_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Starting simulation with {num_steps} steps...")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # 7. Run simulation loop
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    
    # 8. Print final statistics
    print(f"\nSimulation complete!")
    print(f"Total time: {end_time - start_time:.2f} seconds")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Total simulation time: {world.getSimulationTime():.2f} seconds")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'duration': end_time - start_time
    }

# Run the simulation
if __name__ == "__main__":
    results = run_complete_simulation(1000)
```

## Advanced Features

### 1. Brain Comparison and Analysis

```python
import pynlm

# Create two brains for comparison
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createDefaultConfig()

brain1 = pynlm.createBrain(config1)
brain2 = pynlm.createBrain(config2)

brain1.initialize()
brain2.initialize()

# Run different simulations
for step in range(500):
    brain1.step(step)
    brain2.step(step * 1.1)  # Different timing

# Compare brains
comparison = pynlm.compareBrains(brain1, brain2)
print(f"Neuron count match: {comparison['neuron_count_match']}")
print(f"Spike count difference: {comparison['spike_count_diff']}")
print(f"Firing rate difference: {comparison['firing_rate_diff']:.3f} Hz")
```

### 2. Development Statistics

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor development over time
for step in range(100):
    brain.step(step)
    
    # Get development statistics
    dev_stats = brain.getDevelopmentStats()
    print(f"Step {step}: {dev_stats['developmental_stage']}")
    
    # Check if development stage changed
    if step % 20 == 0:
        print(f"Plasticity multiplier: {dev_stats['plasticity_multiplier']}")
```

### 3. Utility Functions

```python
import pynlm

# Create some data
data = [1.0, 2.0, 3.0, 4.0, 5.0]

# Use utility functions
normalized = pynlm.normalize(data)
print(f"Normalized: {normalized}")

sliced = pynlm.slice(data, 1, 4)
print(f"Sliced: {sliced}")

mean_val = pynlm.mean([1.0, 2.0, 3.0, 4.0, 5.0])
print(f"Mean: {mean_val}")

std_val = pynlm.standardDeviation([1.0, 2.0, 3.0, 4.0, 5.0])
print(f"Standard deviation: {std_val}")
```

### 4. Episode-based Simulation

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Create a dummy sensory input (e.g., vision data)
sensory_input = [0.1, 0.2, 0.3, 0.4, 0.5] * 20  # 100 values

# Run a single episode
episode_result = brain.simulateEpisode(sensory_input, 500)

print(f"Episode completed:")
print(f"  Final spikes: {episode_result['final_spikes']}")
print(f"  Final firing: {episode_result['final_firing']}")
print(f"  Final rate: {episode_result['final_rate']:.2f} Hz")
print(f"  Final stage: {episode_result['final_stage']}")
```

### 5. World Object Manipulation

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

# Add various objects
resource = world.createResourceObject(5.0, 5.0, 10.0)
print(f"Created resource at ({resource.x}, {resource.y}) with value {resource.value}")

hazard = world.createHazardObject(15.0, 15.0, 2.0)
print(f"Created hazard at ({hazard.x}, {hazard.y}) with radius {hazard.radius}")

wall = world.createWallObject(10.0, 5.0, 1.0)
print(f"Created wall at ({wall.x}, {wall.y})")

# Find nearest object
nearest = world.findNearestObject(12.0, 12.0)
if nearest is not None:
    print(f"Nearest object at ({nearest.x}, {nearest.y}) type: {nearest.type}")
```

### 6. Agent Learning Statistics

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run a few steps
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    agent.updateDevelopment(0.1)

# Get learning statistics
learning_stats = agent.getLearningStats()
print(f"Reward modulation enabled: {learning_stats['reward_modulation']}")
print(f"Structural plasticity enabled: {learning_stats['structural_plasticity']}")
print(f"Development enabled: {learning_stats['development']}")
print(f"Curiosity enabled: {learning_stats['curiosity']}")
print(f"Curiosity level: {learning_stats['curiosity_level']:.3f}")
print(f"Novelty level: {learning_stats['novelty_level']:.3f}")
print(f"Prediction error: {learning_stats['prediction_error']:.3f}")
print(f"Neuromodulation level: {learning_stats['neuromodulation_level']:.3f}")
```

## Configuration Management

### Creating Custom Configurations

```python
import pynlm
import json

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Set basic parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("random_seed", 42)

# Set learning parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Set neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")
```

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()

# Add required keys
config.set("brain.neuron_count", 1000)
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)

# Validate configuration
try:
    if config.validate():
        print("Configuration is valid!")
except pynlm.NLMError as e:
    print(f"Configuration error: {e}")
```

## Error Handling

### Custom Exceptions

```python
import pynlm

try:
    # Create an invalid configuration
    config = pynlm.createDefaultConfig()
    
    # Try to validate without required keys
    if not config.validate():
        raise pynlm.NLMError("Invalid configuration")
        
except pynlm.NLMError as e:
    print(f"NLM error occurred: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Monitoring

### Brain Statistics

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive brain statistics
stats = brain.getNeuronStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
print(f"Excitation/Inhibition ratio: {stats['e_i_ratio']:.3f}")
```

## Version Information

```python
import pynlm

print(f"NLM Python Bindings Version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"Description: {pynlm.__description__}")
```

## Examples and Tutorials

### 1. Basic Brain Simulation

See `runHelloWorldExample()` in the bindings for a simple demonstration.

### 2. Complete Agent System

See `runAgentExample()` in the bindings for a complete agent simulation.

### 3. Performance Benchmark

Create your own benchmark by:
1. Creating multiple brain configurations
2. Running simulations with different parameters
3. Comparing results using the provided utilities

## Common Issues and Solutions

### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

### "Segmentation fault"

```python
# Ensure brain is initialized before use
brain = pynlm.createBrain(config)
brain.initialize()  # Essential!
brain.step(0)  # Now safe to use
```

### "Configuration validation failed"

```python
# Add missing required keys
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Required
config.set("random_seed", 42)          # Required
config.set("simulation_timestep", 0.001) # Required

if not config.validate():
    print("Configuration is invalid")
```

### "Brain step fails"

```python
# Ensure proper initialization
brain = pynlm.createBrain(config)
brain.initialize()  # CRITICAL!
brain.step(0)  # Now should work
```

## Migration Guide

### From Version 1.x to 2.0

1. **API Changes**: The API is now more Pythonic with better error handling
2. **New Convenience Methods**: Added helper methods for common tasks
3. **Enhanced Documentation**: Comprehensive documentation and examples
4. **Performance Improvements**: Better performance and memory management
5. **New Features**: Advanced simulation control and monitoring

### Key Changes:

- Enhanced error handling with custom exceptions
- New utility functions for data processing
- Improved configuration validation
- Advanced simulation features
- Better documentation and examples

## Future Development

The NLM Python bindings are actively developed. Future improvements may include:

- More advanced machine learning integration
- GPU acceleration support
- Real-time simulation capabilities
- Enhanced visualization tools
- Distributed computing support

## License

This project is licensed under the MIT License.

## Acknowledgments

- The NLM research team for their groundbreaking work
- The pybind11 team for excellent binding framework
- The C++ community for high-performance libraries
- Users who provide feedback and contribute to the project