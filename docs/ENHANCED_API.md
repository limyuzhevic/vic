# NLM Enhanced Documentation

This documentation provides comprehensive guidance for using the NLM (Neural Learning Machine) enhanced with improved Python bindings and C++ core.

## Overview

NLM is an experimental artificial developmental brain simulation framework. The enhanced version provides:

- **Simplified Python API** for easy prototyping
- **Advanced C++ interface** for performance-critical applications  
- **Comprehensive testing** and validation tools
- **Extensive documentation** and examples

## Quick Start

### Basic Usage (Python)

```python
import pynlm_enhanced as nlm

# Create a simple brain
brain = nlm.createSimpleBrain(neuron_count=100, steps=100)
print(f"Created brain with {brain.getNeuronCount()} neurons")

# Run simulation
brain.simulate(50)

# Get status
status = brain.getStatus()
print(f"Firing rate: {status['firing_rate']:.2f}")
print(f"Learning detected: {status['learning_detected']}")
```

### Advanced Usage (C++)

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Log status
    brain->logStatus();
    
    return 0;
}
```

## API Reference

### Python API

#### Core Classes

##### SimplifiedBrain

A user-friendly interface for brain simulation with minimal setup.

**Constructor**
```python
def __init__(self)
    # Default constructor with basic configuration

def __init__(self, neuron_count: int, max_steps: int)
    # Custom constructor with specified parameters

def __init__(self, config: EnhancedConfig)
    # Initialize with custom configuration
```

**Methods**
```python
simulate(steps: int) -> None
    Run simulation for specified number of steps

getNeuronCount() -> int
    Get number of neurons in brain

getSynapseCount() -> int
    Get number of synapses in brain

getAverageFiringRate() -> float
    Get average firing rate across all neurons

getTotalSpikes() -> int
    Get total spike count

checkLearning(threshold: float = 0.01) -> bool
    Check if brain has learned (changed synaptic weights)

getStatus() -> dict
    Get comprehensive brain status as dictionary

enableRewardModulation(enable: bool) -> None
    Enable/disable reward modulation system

enableStructuralPlasticity(enable: bool) -> None
    Enable/disable structural plasticity

enableDevelopment(enable: bool) -> None
    Enable/disable developmental system

enableCuriosity(enable: bool) -> None
    Enable/disable curiosity-driven exploration

save(filepath: str) -> bool
    Save brain state to file

load(filepath: str) -> bool
    Load brain state from file
```

##### NeuralNetwork

A simplified neural network implementation for machine learning tasks.

**Constructor**
```python
def __init__(self, input_size: int, hidden_size: int, output_size: int)
    Create neural network with specified layer sizes
```

**Methods**
```python
forward(input: List[float]) -> List[float]
    Perform forward pass through the network

train(inputs: List[List[float]], 
      targets: List[List[float]], 
      epochs: int) -> None
    Train the network on given data

getStats() -> dict
    Get network statistics and metadata
```

##### EnhancedConfig

A user-friendly configuration interface with type-safe methods.

**Constructor**
```python
def __init__(self)
    Create default configuration
```

**Setter Methods**
```python
def set(self, key: str, value: int) -> EnhancedConfig
def set(self, key: str, value: float) -> EnhancedConfig  
def set(self, key: str, value: str) -> EnhancedConfig
    Set configuration values with method chaining
```

**Getter Methods**
```python
def getInt(self, key: str, default_value: int = 0) -> int
def getFloat(self, key: str, default_value: float = 0.0) -> float
def getString(self, key: str, default_value: str = "") -> str
    Get configuration values with type safety
```

**Static Factory Methods**
```python
EnhancedConfig.createBasicConfig(neuron_count: int = 100) -> EnhancedConfig
EnhancedConfig.createAdvancedConfig(neuron_count: int = 500) -> EnhancedConfig
    Create pre-configured configurations
```

**Utility Methods**
```python
def toJson(self) -> str
    Export configuration as JSON string

def getConfig(self) -> Config
    Get underlying C++ config object
```

#### Convenience Functions

```python
createSimpleBrain(neuron_count: int = 100, steps: int = 100) -> SimplifiedBrain
    Quick factory function for simple brain creation

createNeuralNetwork(input_size: int, hidden_size: int, output_size: int) -> NeuralNetwork
    Create a simple neural network

batchSimulate(brains: List[SimplifiedBrain], steps: int) -> List[dict]
    Run simulation on multiple brains

trainAndEvaluate(training_data, training_labels, test_data, test_labels, epochs=100) -> dict
    Train neural network and evaluate performance
```

### C++ API

#### Core Classes

##### Brain

The central neural simulation brain with comprehensive functionality.

**Constructors**
```cpp
Brain(std::shared_ptr<Config> config)
    Create brain with configuration
```

**Methods**
```cpp
initialize() -> bool
    Initialize brain with configuration

step(SimulationStep currentStep) -> void
    Perform simulation step

step(SimulationStep currentStep, Timestamp currentTime) -> void
    Perform simulation step with timestamp

receiveSensoryInput(const SensoryInput& input) -> void
    Inject sensory input into sensory neurons

injectCurrent(NeuronId neuron, MembranePotential current) -> void
    Inject current into specific neuron

injectCurrentToNeurons(NeuronType type, MembranePotential current) -> void
    Inject current into all neurons of specified type

produceAction() -> std::unique_ptr<Action>
    Generate motor action based on neural activity

reset() -> void
    Reset brain state

save(const std::string& filepath) const -> bool
    Save brain state to file

load(const std::string& filepath) -> bool
    Load brain state from file

addRegion(const std::string& name = "") -> RegionId
    Add new neural region

getRegion(RegionId id) -> NeuralRegion*
getRegion(RegionId id) const -> const NeuralRegion*
    Get region by ID

// ... many more methods for statistics, memory systems, etc.
```

#### Configuration System

##### Config

Configuration management with type-safe getters and setters.

**Core Methods**
```cpp
loadFromFile(const std::string& filepath) -> bool
loadFromArgs(int argc, char** argv) -> bool
saveToFile(const std::string& filepath) -> bool
has(const std::string& key) const -> bool
getKeys() const -> std::vector<std::string>
clear() -> void
summary() const -> std::string
```

## Examples

### Example 1: Basic Brain Simulation

```python
import pynlm_enhanced as nlm

print("=== Example 1: Basic Brain Simulation ===")

# Create brain with 200 neurons
brain = nlm.createSimpleBrain(neuron_count=200, steps=50)

# Run simulation
brain.simulate(100)

# Check results
status = brain.getStatus()
print(f"Brain initialized with {status['neuron_count']} neurons")
print(f"Simulation completed with {status['total_spikes']} total spikes")
print(f"Average firing rate: {status['firing_rate']:.3f} Hz")
print(f"Learning detected: {status['learning_detected']}")
```

### Example 2: Neural Network Training

```python
import pynlm_enhanced as nlm
import random

print("=== Example 2: Neural Network Training ===")

# Generate simple data (XOR problem)
inputs = [
    [0, 0], [0, 1], [1, 0], [1, 1]
]
targets = [
    [0], [1], [1], [0]
]

# Create and train network
network = nlm.createNeuralNetwork(input_size=2, hidden_size=4, output_size=1)
results = nlm.trainAndEvaluate(inputs, targets, inputs, targets, epochs=100)

print(f"Network trained for {results['epochs_completed']} epochs")
print(f"Input size: {results['input_size']}")
print(f"Output size: {results['output_size']}")
print(f"Final learning rate: {results['final_stats']['learning_rate']:.4f}")
print(f"Average weight magnitude: {results['final_stats']['avg_weight_magnitude']:.4f}")
```

### Example 3: Advanced Configuration

```python
import pynlm_enhanced as nlm

print("=== Example 3: Advanced Configuration ===")

# Create basic configuration
basic_config = nlm.EnhancedConfig.createBasicConfig(neuron_count=500)

# Modify configuration
config = nlm.EnhancedConfig.createAdvancedConfig(neuron_count=1000)
config.set("stdp_ltp_weight", 0.02f)
config.set("stdp_ltd_weight", 0.015f)
config.set("synaptogenesis_rate", 0.00005f)
config.set("pruning_rate", 0.000005f)

print("Basic config:")
print(basic_config.toJson())

print("\nAdvanced config:")
print(config.toJson())

print(f"\nNeuron count in advanced config: {config.getInt('neuron_count')}")
print(f"STDP LTP weight: {config.getFloat('stdp_ltp_weight')}")
```

### Example 4: Batch Processing

```python
import pynlm_enhanced as nlm
import time

print("=== Example 4: Batch Processing ===")

# Create multiple brains for parallel processing
brains = [nlm.createSimpleBrain(neuron_count=100 + i*50, steps=50) 
          for i in range(5)]

# Run batch simulation
start_time = time.time()
results = nlm.batchSimulate(brains, steps=100)
end_time = time.time()

print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
print(f"Processed {len(brains)} brains")

for i, result in enumerate(results):
    print(f"Brain {i+1}: {result['neuron_count']} neurons, "
          f"{result['total_spikes']} spikes, "
          f"firing rate: {result['firing_rate']:.3f} Hz")
```

## Performance Tips

### Python Performance

1. **Reuse objects**: Create brain/config objects and reuse them
2. **Batch operations**: Use batch functions instead of individual calls
3. **Memory management**: Be mindful of memory usage with large simulations
4. **Parallel processing**: Consider using multiple processes for CPU-bound tasks

### C++ Performance

1. **Configuration reuse**: Reuse configuration objects
2. **Memory pools**: Use memory pools for large numbers of objects
3. **SIMD optimizations**: Consider using SIMD instructions where appropriate
4. **Profiling**: Use profiling tools to identify bottlenecks

## Troubleshooting

### Common Issues

#### "Brain fails to initialize"
- **Cause**: Invalid configuration parameters
- **Solution**: Check configuration values and ensure all required parameters are set

#### "Simulation runs too slowly"
- **Cause**: Too many neurons or synapses
- **Solution**: Reduce network size or optimize configuration

#### "Python bindings not found"
- **Cause**: Module not built or installed
- **Solution**: Rebuild and install the enhanced Python bindings

#### "Memory usage too high"
- **Cause**: Large network with many neurons/synapses
- **Solution**: Reduce network size or implement checkpointing

### Debugging Tools

#### Python Debug Mode
```python
import pynlm_enhanced as nlm

# Enable verbose logging
# (Note: This may require additional setup in the bindings)

# Debug specific components
brain = nlm.createSimpleBrain()
status = brain.getStatus()
print("Debug status:", status)
```

#### C++ Debug Builds
```bash
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0"
make -j4
```

## Advanced Features

### 1. Neural Network Composition

```python
# Create network with multiple layers
input_layer = nlm.createSimpleBrain(neuron_count=100)
hidden_layer = nlm.createSimpleBrain(neuron_count=200)  
output_layer = nlm.createSimpleBrain(neuron_count=50)

# Simulate all layers
input_layer.simulate(50)
hidden_layer.simulate(50)
output_layer.simulate(50)
```

### 2. Custom Configurations

```python
# Create custom configuration
config = nlm.EnhancedConfig()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.0005f)
config.set("random_seed", 12345)
config.set("connection_probability", 0.05f)
config.set("stdp_ltp_weight", 0.01f)
config.set("stdp_ltd_weight", 0.01f)
```

### 3. Performance Monitoring

```python
import time
import pynlm_enhanced as nlm

# Create brain and monitor performance
brain = nlm.createSimpleBrain(neuron_count=500)

start_time = time.time()
brain.simulate(1000)
end_time = time.time()

status = brain.getStatus()
print(f"Simulation took {end_time - start_time:.2f} seconds")
print(f"Performance: {status['total_spikes']/(end_time - start_time):.0f} spikes/second")
```

## Migration Guide

### From Original NLM to Enhanced NLM

#### Python Migration

**Before (Original)**
```python
import pynlm as nlm

# Basic usage (limited features)
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
brain.step(0)
```

**After (Enhanced)**
```python
import pynlm_enhanced as nlm

# Enhanced usage (many more features)
brain = nlm.createSimpleBrain(neuron_count=100)
brain.simulate(50)
status = brain.getStatus()
```

**Key Changes**

1. **Simplified API**: `createSimpleBrain()` instead of `createBrain(config)`
2. **Easier initialization**: No need to manually call `initialize()`
3. **Better methods**: `simulate()` instead of manual step loops
4. **Rich status**: `getStatus()` provides comprehensive information
5. **Convenience functions**: Additional factory functions for common patterns

#### C++ Migration

The C++ API remains largely unchanged for backward compatibility. Enhanced features are available through the enhanced Python bindings.

## Frequently Asked Questions

### Q: What are the main differences between `pynlm` and `pynlm_enhanced`?

**A:** `pynlm` is the original bindings with the full C++ API exposed. `pynlm_enhanced` provides a simplified, user-friendly API with additional convenience functions and high-level abstractions.

### Q: Can I use both modules in the same project?

**A:** Yes, both modules can be used simultaneously. They are designed to be complementary - use `pynlm_enhanced` for rapid prototyping and `pynlm` for low-level control.

### Q: What Python versions are supported?

**A:** The enhanced bindings support Python 3.8 and above, similar to the original bindings.

### Q: How do I install the enhanced bindings?

**A:** See the project README for installation instructions. The enhanced bindings are built alongside the original bindings.

## Acknowledgments

This enhancement work builds upon the original NLM project developed by the NLM research team. Special thanks to all contributors who have improved the codebase over time.

## License

MIT License - See LICENSE file for details.
