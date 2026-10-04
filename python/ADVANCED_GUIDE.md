# NLM Python Bindings - Advanced User Guide

This document provides comprehensive documentation for the enhanced Python bindings of the NLM (Neural Learning Machine) framework.

## Overview

The Python bindings (`pynlm`) provide access to the full power of NLM's neural simulation capabilities through a clean, Pythonic API. This guide covers both basic usage and advanced features for power users.

## Quick Start

### Basic Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few simulation steps
for i in range(100):
    brain.step(i)

print(f"Brain processed {brain.getTotalSpikeCount()} spikes")
```

## Advanced Features

### 1. Advanced Configuration Presets

The bindings include several preset configurations for different use cases:

```python
import pynlm

# Development configuration (slow, detailed)
dev_config = pynlm.createConfigWithPresets("development")

# Production configuration (fast, optimized)
prod_config = pynlm.createConfigWithPresets("production")

# Research configuration (most features, highest detail)
research_config = pynlm.createConfigWithPresets("research")
```

### 2. Advanced Brain Creation

```python
import pynlm

# Create brain with custom topology and features
brain = pynlm.createAdvancedBrain(
    config=research_config,
    topology="modular",  # "layered", "modular", "dense", "sparse"
    enablePlasticity=True,
    enableDevelopment=True
)

# Initialize and run
brain.initialize()

# Run with statistics
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        stats = pynlm.getBrainPerformanceStats(brain)
        print(f"Step {step}: {stats['spikesProcessed']} spikes")
```

### 3. Batch Operations

Process multiple brains efficiently:

```python
import pynlm
from functools import partial

# Create multiple brains
brains = [pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
          for _ in range(5)]

# Initialize all brains
for brain in brains:
    brain.initialize()

# Apply the same operation to all brains
def applyLearning(brain):
    for step in range(100):
        brain.step(step)

pynlm.batchBrainOperation(brains, applyLearning)

# Analyze all brains
for i, brain in enumerate(brains):
    analysis = pynlm.analyzeBrainNetwork(brain)
    print(f"Brain {i} analysis: {analysis}")
```

### 4. Performance Monitoring

Monitor brain performance in real-time:

```python
import pynlm
import time

brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
brain.initialize()

# Performance monitoring loop
start_time = time.time()
step_count = 0
spike_count = 0

for step in range(1000):
    brain.step(step)
    step_count += 1
    spike_count += brain.getTotalSpikeCount()
    
    # Get performance statistics every 100 steps
    if step % 100 == 0:
        stats = pynlm.getBrainPerformanceStats(brain)
        elapsed = time.time() - start_time
        print(f"Step {step}:")
        print(f"  Speed: {step/elapsed:.1f} steps/sec")
        print(f"  Memory: {stats['memoryUsage']:.2f}%")
        print(f"  CPU: {stats['cpuUsage']:.2f}%")
        print(f"  Average firing rate: {stats['avgFiringRate']:.2f} Hz")
```

### 5. Brain State Management

Save and restore brain states:

```python
import pynlm
import json

brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
brain.initialize()

# Run some simulation
for step in range(500):
    brain.step(step)

# Save brain state
state_data = pynlm.serializeBrain(brain)
with open("brain_state.json", "w") as f:
    f.write(state_data)

# Later, restore brain state
with open("brain_state.json", "r") as f:
    saved_state = f.read()

# Note: In a real implementation, you would load this state back into a brain
# For now, this demonstrates the serialization interface
print("Brain state serialized successfully")
```

### 6. Network Analysis

Analyze brain connectivity and performance:

```python
import pynlm

brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get detailed network analysis
analysis = pynlm.analyzeBrainNetwork(brain)

print("=== Brain Network Analysis ===")
print(f"Connectivity Density: {analysis['connectivity_density']:.6f}")
print(f"Average Firing Rate: {analysis['average_firing_rate']:.2f} Hz")
print(f"Activity Level: {analysis['activity_level']:.2%}")
print(f"Total Synapses: {brain.getTotalSynapseCount()}")
print(f"Total Neurons: {brain.getTotalNeuronCount()}")
print(f"Active Neurons: {brain.getActiveNeuronCount()}")
print(f"Firing Neurons: {brain.getFiringNeuronCount()}")
```

## Error Handling

The enhanced bindings provide comprehensive error handling with custom exceptions:

```python
import pynlm

try:
    brain = pynlm.createAdvancedBrain(None)  # Invalid config
except pynlm.BrainConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Available Error Codes

```python
# Access error codes for programmatic handling
error_codes = pynlm.ERROR_CODES

if error_codes["CONFIG_NOT_FOUND"] in error_messages:
    # Handle missing config
    pass
```

## Performance Optimization Tips

### 1. Memory Management

```python
import pynlm

# Use context managers for resource management
brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("production"))
brain.initialize()

# Process data in batches
batch_size = 100
for i in range(0, 1000, batch_size):
    # Process batch of simulation steps
    for step in range(i, min(i + batch_size, 1000)):
        brain.step(step)
```

### 2. Vectorized Operations

When working with multiple sensory inputs or outputs:

```python
import numpy as np

# Create vectorized sensory inputs
sensory_inputs = []
for i in range(100):
    vision = pynlm.Vision(64, 64, 3)
    # Fill with synthetic data
    data = np.random.rand(64 * 64 * 3).astype(np.float32)
    vision.setData(list(data))
    sensory_inputs.append(vision)

# Process batch
brain.runWithInput(100, sensory_inputs)
```

### 3. Configuration Optimization

For different use cases:

```python
# For real-time applications
real_time_config = pynlm.createProductionConfig(neuronCount=2000)

# For research applications  
research_config = pynlm.createResearchConfig(neuronCount=10000, learningRate=0.001)

# For learning applications
learning_config = pynlm.createConfigWithPresets("development")
```

## Advanced Brain Operations

### Brain Cloning

```python
import pynlm

# Clone brain state (preserves configuration)
original_brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
original_brain.initialize()

# Run some steps
for step in range(200):
    original_brain.step(step)

# Clone for experimentation
cloned_brain = pynlm.cloneBrainState(original_brain)

# Now both brains can be modified independently
```

### Batch Brain Operations

```python
import pynlm

# Create multiple brain instances
brains = [pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
          for _ in range(10)]

# Initialize all
for brain in brains:
    brain.initialize()

# Apply learning to all brains simultaneously
def trainAllBrains(brain_list):
    for brain in brain_list:
        for step in range(100):
            brain.step(step)

pynlm.batchBrainOperation(brains, trainAllBrains)
```

## Integration with Other Systems

### Machine Learning Integration

```python
import pynlm
import numpy as np

# Use NLM as a feature extractor
brain = pynlm.createAdvancedBrain(pynlm.createResearchConfig())
brain.initialize()

# Extract neural activity patterns
features = []
for step in range(500):
    brain.step(step)
    
    # Collect activity statistics
    activity = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'active_neurons': brain.getActiveNeuronCount(),
        'connectivity': brain.getExcitationInhibitionRatio()
    }
    features.append(activity)

# Convert to numpy array for ML
feature_array = np.array(features)
```

### Reinforcement Learning Integration

```python
import pynlm

# Create agent with learning capabilities
brain = pynlm.createAdvancedBrain(pynlm.createResearchConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure agent for reinforcement learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run RL episode
agent.initialize(world)
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    agent.applyRewardModulation(result.reward, agent.getPredictionError())
```

## Debugging and Development

### Logging and Monitoring

```python
import pynlm

# Enable detailed logging
brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
brain.initialize()

# Set up logging to capture brain state changes
import logging
logging.basicConfig(level=logging.INFO)

# Run with logging
for step in range(100):
    brain.step(step)
    if step % 50 == 0:
        brain.logStatus()
```

### Visualization Tools

```python
import pynlm
import matplotlib.pyplot as plt

brain = pynlm.createAdvancedBrain(pynlm.createResearchConfig())
brain.initialize()

# Collect statistics for visualization
steps = []
spike_counts = []
firing_rates = []

for step in range(1000):
    brain.step(step)
    steps.append(step)
    spike_counts.append(brain.getTotalSpikeCount())
    firing_rates.append(brain.getAverageFiringRate())

# Plot results
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))

ax1.plot(steps, spike_counts)
ax1.set_xlabel('Step')
ax1.set_ylabel('Total Spikes')
ax1.set_title('Spike Activity Over Time')

ax2.plot(steps, firing_rates)
ax2.set_xlabel('Step')
ax2.set_ylabel('Average Firing Rate (Hz)')
ax2.set_title('Firing Rate Over Time')

plt.tight_layout()
plt.show()
```

## Migration Guide

### From Basic to Advanced API

If you're migrating from basic usage to advanced features:

**Before:**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

**After:**
```python
brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("development"))
brain.initialize()
brain.step(0)
```

### Upgrading Configuration

**Before:**
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
```

**After:**
```python
config = pynlm.createConfigWithPresets("research")
# Already includes neuron_count = 5000
```

## Best Practices

### 1. Resource Management

- Always initialize brains before use
- Use context managers when available
- Monitor memory usage for long-running simulations

### 2. Error Handling

- Always wrap brain operations in try-catch blocks
- Use specific exception types for different error conditions
- Implement proper logging for debugging

### 3. Performance Optimization

- Use batch operations for multiple brains
- Pre-allocate memory where possible
- Monitor performance regularly
- Use production configurations for real-time applications

### 4. Testing and Validation

- Test configurations thoroughly before deployment
- Validate brain states after operations
- Implement comprehensive error checking

## Troubleshooting

### Common Issues

**Issue:** Brain initialization fails
**Solution:** Ensure configuration is valid and parameters are within expected ranges

**Issue:** Simulation runs slowly
**Solution:** Use production configurations or optimize hardware

**Issue:** Memory usage is high
**Solution:** Use memory-efficient configurations or implement batch processing

**Issue:** Unexpected results
**Solution:** Enable logging and check brain status regularly

### Error Messages

- `BrainConfigurationError`: Check your configuration parameters
- `SimulationError`: Verify simulation parameters are valid
- `NeuronError`: Check neuron configuration and parameters
- `SynapseError`: Verify synaptic connections are valid
- `MemoryError`: Check available memory and resource usage

## Conclusion

The enhanced NLM Python bindings provide a comprehensive suite of tools for neural simulation research and development. From simple demonstrations to complex research applications, the bindings offer the flexibility and power needed for advanced neural computation.

For more information, refer to the detailed API documentation and examples in the accompanying source code.
