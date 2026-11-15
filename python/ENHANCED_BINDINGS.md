# NLM Python Enhanced Bindings

## Overview

This enhanced version of the NLM Python bindings provides additional utilities and improved interfaces for better usability, especially for power users and research applications.

## Key Features

### 1. Configuration Utilities

- **`createSimulationConfig()`**: Creates a configuration with sensible defaults for simulation
- **`loadConfigurationsFromFile()`**: Load multiple configuration files
- **Better error handling** with custom NLMError exceptions

### 2. Simulation Tools

- **`runSimulation()`**: Run a complete simulation with statistics collection
- **`createVisionFromImage()`**: Convert numpy arrays to Vision sensory input

### 3. Enhanced Class Interfaces

The enhanced bindings provide:
- **Properties** for better access to object data
- **Method chaining** where appropriate
- **Automatic type conversion** for common data types
- **Better documentation strings** for improved IDE support

### 4. Power User Features

- **Batch processing** utilities
- **Numpy integration** for data analysis
- **Configuration validation** and error checking
- **Advanced simulation control** options

## Example Usage

```python
import pynlm
import numpy as np

# Create enhanced configuration
config = pynlm.createSimulationConfig()
print(f"Created brain with {config.get('neuron_count')} neurons")

# Run enhanced simulation
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with automatic statistics
result = pynlm.runSimulation(config, steps=1000)
firing, spikes, avg_firing, reward = result

print(f"Simulation completed:")
print(f"  Neurons firing: {firing}")
print(f"  Total spikes: {spikes}")
print(f"  Average firing rate: {avg_firing}")
print(f"  Reward: {reward}")

# Process image data
image = np.random.rand(64, 64, 3).astype(np.float32)
vision = pynlm.createVisionFromImage(image, 64, 64)
brain.receiveSensoryInput(vision)

# Batch configuration loading
configs = pynlm.loadConfigurationsFromFile(["config1.cfg", "config2.cfg"])
print(f"Loaded {len(configs)} configuration files")
```

## Advanced Features

### 1. Custom Error Handling

The enhanced bindings provide better error handling:

```python
import pynlm

try:
    config = pynlm.createSimulationConfig()
    # ... use config
except pynlm.NLMRuntimeError as e:
    print(f"NLM runtime error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

### 2. Advanced Configuration

```python
# Override configuration with environment variables
import os
if "NLM_NEURON_COUNT" in os.environ:
    config = pynlm.createSimulationConfig()
    config.set("brain.neuron_count", int(os.environ["NLM_NEURON_COUNT"]))
```

### 3. Simulation Control

```python
# Run simulation with custom parameters
def run_custom_simulation(config, steps, verbose=True):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if verbose:
        print(f"Starting simulation with {config.get('neuron_count')} neurons")
    
    for step in range(steps):
        brain.step(step)
        
        if verbose and step % 100 ==  BIT:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons active")
    
    return {
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'e_inhibition_ratio': brain.getExcitationInhibitionRatio()
    }
```

### 4. Performance Monitoring

```python
import time
import pynlm

def benchmark_simulation(config, steps):
    start_time = time.time()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    return {
        'total_steps': steps,
        'elapsed_time': elapsed,
        'steps_per_second': steps / elapsed,
        'firing_stats': {
            'max_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    }
```

## Installation

The enhanced bindings are included in the standard NLM installation. After installing NLM:

```bash
pip install pynlm
```

For development installation:

```bash
pip install -e .
```

## Migration Guide

### From Basic to Enhanced Bindings

The enhanced bindings are mostly backward compatible. Key improvements:

1. **New utility functions**: `createSimulationConfig()`, `runSimulation()`, `createVisionFromImage()`
2. **Better error handling**: Custom exception classes
3. **Enhanced configuration loading**: Batch file loading
4. **Improved numpy integration**: `createVisionFromImage()` for image processing

All existing Python code will continue to work, but you can now use the enhanced features for better productivity and functionality.

## License

MIT License - See LICENSE file for details.
