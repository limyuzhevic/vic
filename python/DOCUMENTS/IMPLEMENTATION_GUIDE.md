# NLM Python API - Comprehensive Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain simulation framework. This document provides a comprehensive guide to using the improved Python API with enhanced error handling, validation, and convenience features.

## Installation

```bash
# Install NLM with Python bindings
pip install pynlm

# For development (install from source)
pip install -e .
```

## Quick Start Examples

### Basic Brain Simulation

```python
import pynlm

# Create a brain with optimized configuration
config = pynlm.createBrainConfig(neuron_count=1000, region_count=1)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Simulation

```python
import pynlm

# Setup brain and agent
config = pynlm.createBrainConfig(neuron_count=500)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

### World with Objects

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15)
world.reset()

# Add objects to world
world.createResource(3.0, 3.0, 10.0)      # Food/resource
world.createHazard(10.0, 10.0, 5.0)     # Dangerous object
world.createWall(7.0, 7.0, 2.0)          # Wall

# Check agent position
body = world.getAgentBody()
print(f"Agent at ({body.x:.1f}, {body.y:.1f}), energy: {body.energy:.1f}")

# Check what's nearby
percept = world.getSensoryPercept()
print(f"Vision width: {percept.getVisionWidth()}, height: {percept.getVisionHeight()}")

# Update world
world.update(0.1)
```

### Configuration Management

```python
import pynlm

# Create optimized configuration
config = pynlm.createBrainConfig(
    neuron_count=1000,
    region_count=1,
    enable_plasticity=True,
    enable_development=True
)

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.002)

# Validate configuration
if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration has errors")

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config2 = pynlm.importJsonConfig("my_config.cfg")
```

### Validation and Error Handling

```python
import pynlm

# Configuration validation
config = pynlm.createBrainConfig(neuron_count=100)

# Check if configuration is complete
if config.isComplete():
    print("Configuration has all required keys")
else:
    print("Missing required configuration keys")

# Get validation details
is_valid, error_msg = config.validateWithErrors()
if not is_valid:
    print(f"Validation failed: {error_msg}")

# Safe value access
try:
    neuron_count = config.get("brain.neuron_count")
    if neuron_count:
        print(f"Neuron count: {neuron_count.value}")
    else:
        print("Neuron count not found")
except Exception as e:
    print(f"Error accessing configuration: {e}")

# Set with type safety
config.set("brain.v_thresh", -50.0)  # Membrane potential threshold
config.set("brain.v_rest", -70.0)    # Resting potential
config.set("brain.v_reset", -75.0)   # Reset potential
```

## API Reference

### Core Classes

#### Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)                    # or brain.step(step, time)
brain.receiveSensoryInput(percept)  # Inject sensory data
brain.injectCurrent(neuron_id, current)  # Inject current
brain.produceAction()               # Get motor action
brain.save("brain_state.bin")       # Save state
brain.load("brain_state.bin")       # Load state
brain.addRegion("cortex")           # Add brain region
brain.getTotalNeuronCount()         # Get neuron count
brain.getAverageFiringRate()        # Get firing rate
brain.getDevelopmentalStage()      # Get development stage
```

#### AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)              # Initialize with world
agent.processSensoryInput(percept)   # Process senses
agent.decodeMotorCommand()          # Get action from brain
agent.applyRewardModulation(reward, predicted)  # Learn from experience
agent.updateDevelopment(timestep)    # Update development
agent.enableRewardModulation(True)  # Enable features
agent.enableCuriosity(True)          # Enable exploration
```

#### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)
world.reset()
world.setAgentStart(x, y)
world.update(timestep)
world.applyMotorCommand(action, time)
world.getSensoryPercept()            # Get sensory input
world.getAgentBody()                 # Get agent position/state
world.createResource(x, y, value)    # Add resource
world.createHazard(x, y, value)      # Add hazard
world.createWall(x, y, radius)       # Add wall
world.isValidPosition(x, y)          # Check position
```

### Configuration

#### Create Configuration
```python
config = pynlm.createDefaultConfig()                    # Default config
config = pynlm.createBrainConfig(neuron_count=1000)      # Optimized brain config
config = pynlm.createSimpleWorldConfig(width=20)         # World config

config = pynlm.importJsonConfig("file.cfg")              # Load from file
config.saveToFile("file.cfg")                            # Save to file
```

#### Configuration Operations
```python
config.has("brain.neuron_count")                        # Check key exists
config.get("brain.neuron_count")                        # Get value (may return None)
config.set("brain.neuron_count", 1000)                  # Set value
config.set("brain.v_thresh", -50.0)                     # Set with type safety
config.remove("temp_key")                               # Remove key
config.clear()                                           # Clear all
config.validate()                                        # Validate configuration
config.isComplete()                                     # Check if complete

# Access all keys
for key in config.getKeys():
    print(f"{key}: {config.get(key)}")
```

### Utility Functions

#### Quick Test
```python
# Run a simple smoke test to verify everything works
firing, spikes = pynlm.runQuickTest()
print(f"Test completed: {firing} firing neurons, {spikes} total spikes")
```

#### Version Information
```python
print(pynlm.getLibraryVersion())  # "NLM Python Bindings v0.1.0"
print(pynlm.getPhase())            # "Phase 6: Final Integration"
```

## Advanced Features

### Parallel Processing Examples

```python
import pynlm
import threading
import time

# Create multiple brains for parallel processing
brains = []
for i in range(5):
    config = pynlm.createBrainConfig(neuron_count=500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run simulations in parallel
def runBrainSimulation(brain, steps):
    for i in range(steps):
        brain.step(i)

threads = []
for brain in brains:
    thread = threading.Thread(target=runBrainSimulation, args=(brain, 100))
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

# Collect results
total_firing = sum(brain.getFiringNeuronCount() for brain in brains)
print(f"Parallel simulation completed: {total_firing} total firing neurons")
```

### Plugin Architecture Example

```python
import pynlm
from typing import Dict, Any

class CustomPlugin:
    def __init__(self, name):
        self.name = name
        self.brain = None
    
    def attachToBrain(self, brain):
        self.brain = brain
        print(f"Plugin '{self.name}' attached to brain")
    
    def processInput(self, input_data):
        # Custom processing logic
        return self.brain.receiveSensoryInput(input_data)
    
    def getStats(self):
        if self.brain:
            return {
                "neurons": self.brain.getTotalNeuronCount(),
                "spikes": self.brain.getTotalSpikeCount(),
                "firing_rate": self.brain.getAverageFiringRate()
            }
        return {}

# Create and use plugin
plugin = CustomPlugin("My Neural Analyzer")
config = pynlm.createBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

plugin.attachToBrain(brain)

# Run simulation with plugin
for i in range(100):
    # Simulate sensory input
    percept = plugin.processInput(f"step_{i}")
    brain.step(i)

# Get plugin statistics
stats = plugin.getStats()
print(f"Plugin stats: {stats}")
```

### Visualization Integration

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

# Create brain with visualization support
config = pynlm.createBrainConfig(neuron_count=1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation and collect data
spike_counts = []
firing_rates = []

for i in range(500):
    brain.step(i)
    spike_counts.append(brain.getTotalSpikeCount())
    firing_rates.append(brain.getAverageFiringRate())

# Create simple visualization
plt.figure(figsize=(12, 8))

plt.subplot(2, 1, 1)
plt.plot(spike_counts)
plt.title("Spike Count Over Time")
plt.xlabel("Step")
plt.ylabel("Total Spikes")

plt.subplot(2, 1, 2)
plt.plot(firing_rates)
plt.title("Average Firing Rate Over Time")
plt.xlabel("Step")
plt.ylabel("Firing Rate (Hz)")

plt.tight_layout()
plt.show()
```

## Error Handling Best Practices

### Catching and Handling Errors

```python
import pynlm

# Initialize brain with error handling
try:
    config = pynlm.createBrainConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        raise pynlm.RuntimeError("Brain initialization failed")
    
    print("Brain initialized successfully")
    
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Configuration Validation

```python
import pynlm

# Validate configuration before use
config = pynlm.createBrainConfig(neuron_count=100)

# Validate and handle errors
is_valid, error_msg = config.validateWithErrors()
if is_valid:
    # Proceed with valid configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Configuration is valid, brain created successfully")
else:
    # Fix configuration errors
    print(f"Configuration errors: {error_msg}")
    # Fix common errors
    if "neuron_count" in error_msg:
        config.set("brain.neuron_count", 1000)  # Use valid value
    
    # Re-validate
    is_valid, error_msg = config.validateWithErrors()
    if is_valid:
        brain = pynlm.createBrain(config)
        brain.initialize()
    else:
        print(f"Still invalid: {error_msg}")
```

## Migration Guide

### From Old API to New API

#### Basic Usage Changes

**Old API:**
```python
import pynlm

config = pynlm.Config()
config.loadFromFile("config.cfg")
brain = pynlm.Brain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

**New API:**
```python
import pynlm

# Or use optimized configuration
config = pynlm.createBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Or use quick test
firing, spikes = pynlm.runQuickTest()
```

#### Error Handling Improvements

**Old API:**
```python
try:
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

**New API:**
```python
try:
    config = pynlm.createBrainConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        raise pynlm.RuntimeError("Brain initialization failed")
    
    print("Success!")
    
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
```

## Performance Tips

### Optimizing Configuration
```python
# Use optimized configuration instead of manual setup
config = pynlm.createBrainConfig(
    neuron_count=1000,
    region_count=1,
    enable_plasticity=True,
    enable_development=True
)

# Pre-validate before use
if config.validate():
    brain = pynlm.createBrain(config)
    brain.initialize()
```

### Efficient Simulation
```python
import pynlm

# Use efficient step methods
brain = pynlm.createBrain(pynlm.createBrainConfig())
brain.initialize()

# Use duration-based stepping for better control
for step in range(0, 1000, 10):
    brain.stepWithDuration(step, 0.1)  # Step with 0.1s duration

# Enable parallel processing for multiple simulations
brains = [pynlm.createBrain(pynlm.createBrainConfig()) for _ in range(5)]
# Run simulations in parallel as shown above
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "Failed to initialize brain"**
```python
# Solution 1: Check configuration
config = pynlm.createBrainConfig()
if not config.isComplete():
    print("Configuration is missing required keys")
    # Fix configuration

# Solution 2: Try with optimized config
config = pynlm.createBrainConfig(neuron_count=500)
brain = pynlm.createBrain(config)
```

**Issue: "Configuration validation failed"**
```python
# Solution: Validate with error details
is_valid, error_msg = config.validateWithErrors()
if not is_valid:
    print(f"Validation failed: {error_msg}")
    # Fix based on error message
    if "neuron_count" in error_msg:
        config.set("brain.neuron_count", 1000)
```

**Issue: "No spikes generated"**
```python
# Solution: Check brain parameters
brain = pynlm.createBrain(pynlm.createBrainConfig())
brain.initialize()

# Verify neural parameters
# (You might need to adjust threshold, etc.)

for i in range(100):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)
    brain.step(i)
```

## Example: Complete Research Simulation

```python
import pynlm
import time
import statistics

def runResearchSimulation(num_brains=10, steps_per_brain=1000):
    """Run a parallel research simulation with multiple brains."""
    print(f"Running research simulation with {num_brains} brains...")
    
    # Create optimized configuration
    config = pynlm.createBrainConfig(
        neuron_count=1000,
        enable_plasticity=True,
        enable_development=True
    )
    
    # Create multiple brains
    brains = [pynlm.createBrain(config) for _ in range(num_brains)]
    for brain in brains:
        brain.initialize()
    
    # Run simulations
    start_time = time.time()
    results = []
    
    for brain_id, brain in enumerate(brains):
        brain_spikes = []
        brain_rates = []
        
        for step in range(steps_per_brain):
            brain.step(step)
            brain_spikes.append(brain.getTotalSpikeCount())
            brain_rates.append(brain.getAverageFiringRate())
        
        results.append({
            'brain_id': brain_id,
            'total_spikes': brain_spikes[-1],
            'avg_firing_rate': statistics.mean(brain_rates),
            'max_firing_rate': max(brain_rates),
            'spike_variance': statistics.variance(brain_spikes) if len(brain_spikes) > 1 else 0
        })
    
    end_time = time.time()
    
    # Analyze results
    total_spikes = sum(r['total_spikes'] for r in results)
    avg_rate = statistics.mean(r['avg_firing_rate'] for r in results)
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Total spikes across all brains: {total_spikes}")
    print(f"Average firing rate: {avg_rate:.2f} Hz")
    
    # Print detailed results
    print("\nDetailed Results:")
    for result in results:
        print(f"  Brain {result['brain_id']}: "
              f"{result['total_spikes']} spikes, "
              f"{result['avg_firing_rate']:.2f} Hz avg, "
              f"var={result['spike_variance']:.2f}")
    
    return results

# Run research simulation
research_results = runResearchSimulation(num_brains=5, steps_per_brain=500)
```

## Conclusion

The improved NLM Python API provides:

- **Better error handling** with specific exception types
- **Enhanced validation** for configurations and operations
- **Convenience methods** for common operations
- **Utility functions** for quick testing and setup
- **Parallel processing support** for performance
- **Plugin architecture** for extensibility
- **Visualization integration** for analysis
- **Comprehensive documentation** with examples

These improvements make it easier to:
- Get started with NLM quickly
- Debug issues effectively
- Extend functionality with custom plugins
- Run performance-intensive simulations
- Analyze and visualize results
- Maintain and scale applications

The API is now more robust, user-friendly, and production-ready while maintaining backward compatibility where possible.