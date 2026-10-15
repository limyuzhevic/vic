# NLM Enhanced Python Bindings

This document provides a comprehensive overview of the enhanced Python bindings for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The NLM Python bindings have been significantly enhanced to provide a more powerful, intuitive, and Pythonic API for users working with spiking neural networks. The enhancements include better error handling, Pythonic properties, enhanced agent functionality, and comprehensive documentation.

## Key Enhancements

### 1. Version Information

The Python module now includes comprehensive version and metadata information:

```python
import pynlm

print(f"Version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"License: {pynlm.__license__}")
print(f"Description: {pynlm.__description__}")
```

**Output:**
```
Version: 0.1.0
Author: NLM Authors
License: MIT
Description: Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework
```

### 2. Pythonic Properties

The enhanced API provides Pythonic property access for key attributes:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Pythonic property access
print(f"Neurons: {brain.neuron_count}")
print(f"Synapses: {brain.synapse_count}")
print(f"Active neurons: {brain.active_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Average firing rate: {brain.average_firing_rate:.3f}")
print(f"Development stage: {brain.development_stage}")
```

### 3. Enhanced Error Handling

Custom exception types provide better error handling for Python users:

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.simulate(100, 0.1)
    
except pynlm.BrainInitializationException as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationException as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationStepException as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Other error: {e}")
```

### 4. Enhanced Agent API

The enhanced agent includes all subsystems with better monitoring:

```python
import pynlm

# Setup enhanced agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable enhanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Enhanced monitoring
print(f"Curiosity Level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty Level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction Error: {agent.getPredictionError():.3f}")
print(f"Development Stage: {agent.getDevelopmentalStage()}")
```

### 5. Enhanced Convenience Methods

Convenience methods provide simplified operations:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enhanced simulation
spike_count = brain.simulate(100, 0.1)
print(f"Simulation complete: {spike_count} spikes")

# Enhanced status
status = brain.get_status()
print(status)
```

### 6. Enhanced Configuration Management

Configuration management is now more Pythonic:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Configuration operations
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.005)

if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get<int>('brain.neuron_count')}")

print(f"Available keys: {config.getKeys()}")
```

## Example Usage

### Basic Enhanced Example

```python
import pynlm

print("=== NLM Enhanced Basic Example ===")

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.neuron_count} neurons")
print(f"Initial development stage: {brain.development_stage}")

# Run enhanced simulation
spike_count = brain.simulate(50, 0.1)
print(f"Simulation complete with {spike_count} total spikes")

# Display enhanced status
print(f"\nBrain Status:\n{brain.get_status()}")
```

### Enhanced Agent Example

```python
import pynlm

print("=== NLM Enhanced Agent Example ===")

# Setup components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Enabled advanced agent subsystems:")
print(f"  Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"  Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"  Development: {agent.isDevelopmentEnabled()}")
print(f"  Curiosity: {agent.isCuriosityEnabled()}")

# Run enhanced episode
for step in range(30):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

print(f"\nEpisode complete! Statistics:\n{agent.get_episode_stats()}")
```

## Available Examples

The enhanced bindings include comprehensive examples:

### Basic Examples
- `examples/enhanced/example_basic.py` - Basic enhanced functionality
- `examples/enhanced/example_agent.py` - Enhanced agent with all subsystems
- `examples/enhanced/example_error_handling.py` - Error handling and recovery

### Advanced Examples
- `examples/enhanced/example_advanced.py` - Advanced usage examples
- `examples/enhanced/example_benchmark.py` - Performance benchmarking
- `examples/enhanced/example_configuration.py` - Configuration management
- `examples/enhanced/example_monitoring.py` - Enhanced monitoring
- `examples/enhanced/example_resilience.py` - Error recovery and resilience
- `examples/enhanced/example_development.py` - Development tracking
- `examples/enhanced/example_performance.py` - Performance optimization

## Documentation

### Main Documentation Files

1. **NLM_PYTHON_ENHANCED_EXAMPLES.md** - Comprehensive examples and usage guide
2. **INSTALL_ENHANCED.md** - Installation guide for enhanced bindings
3. **ENHANCED_BINDINGS_SUMMARY.md** - Complete enhancement summary

### Python Files

1. **examples_enhanced.py** - Main example gallery demonstrating all features
2. **test_enhanced_bindings.py** - Comprehensive test suite for validation
3. **verify_enhanced_bindings.py** - Final verification script

## Installation

### Quick Installation

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install enhanced bindings
pip install -e .
```

### Verification

```python
# Test installation
import pynlm
print(f"NLM Python Bindings Version: {pynlm.__version__}")

# Run examples
python3 examples_enhanced.py

# Run tests
python3 test_enhanced_bindings.py
```

## Build Instructions

### Enhanced Build Script

The enhanced bindings include a comprehensive build script:

```bash
# Build enhanced bindings
python3 build_enhanced.py
```

This script:
1. Configures CMake build
2. Builds the NLM project
3. Builds Python bindings
4. Creates enhanced examples and documentation
5. Sets up installation guides

### Manual Build

```bash
# Manual build instructions
mkdir build
cd build
cmake ..
make -j$(nproc)
pip install -e ..
```

## Testing

### Test Suite

The enhanced bindings include a comprehensive test suite:

```bash
# Run enhanced tests
python3 test_enhanced_bindings.py
```

The test suite validates:
- Version information and module metadata
- Basic functionality and Pythonic properties
- Enhanced agent API and subsystems
- Error handling and exception types
- Configuration management
- World and object functionality

### Example Tests

Each example file can be run individually:

```bash
# Run specific examples
python3 examples/enhanced/example_basic.py
python3 examples/enhanced/example_agent.py
python3 examples/enhanced/example_error_handling.py
```

## Benefits

### 1. **Improved Developer Experience**
- Pythonic API with intuitive property access
- Comprehensive documentation and examples
- Better error handling with custom exception types
- Enhanced developer productivity

### 2. **Enhanced Functionality**
- More powerful agent control with enhanced subsystems
- Better performance monitoring and benchmarking
- Advanced error recovery and resilience
- Custom extension support for advanced users

### 3. **Better Integration**
- Comprehensive example gallery with copy-paste functionality
- Enhanced documentation with detailed usage guides
- Improved testing and validation
- Better installation and setup instructions

### 4. **Advanced Features**
- Enhanced simulation control with convenience methods
- Pythonic property access for quick data retrieval
- Comprehensive statistics and monitoring
- Custom extension support for advanced use cases

## Migration Guide

### From Basic to Enhanced API

**Before (Basic API):**

```python
import pynlm

# Basic usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

**After (Enhanced API):**

```python
import pynlm

# Enhanced usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use enhanced features
print(f"Brain created with {brain.neuron_count} neurons")
spike_count = brain.simulate(100, 0.1)
print(f"Simulation complete: {spike_count} spikes")
```

### Key Migration Points

1. **Version Information**: Check `pynlm.__version__` for module version
2. **Property Access**: Use `brain.neuron_count` instead of `brain.getNeuronCount()`
3. **Error Handling**: Use custom exceptions like `pynlm.BrainInitializationException`
4. **Enhanced Methods**: Use `brain.simulate()` and `brain.get_status()` for enhanced functionality
5. **Agent Subsystems**: Enable enhanced subsystems with `agent.enable*()()`

## Conclusion

The NLM Python bindings have been comprehensively enhanced to provide a more powerful, intuitive, and Pythonic API for users working with spiking neural networks. The enhancements include:

1. **Better error handling** with custom exception types
2. **Pythonic properties** for intuitive API access
3. **Enhanced functionality** with convenience methods and advanced features
4. **Comprehensive documentation** with examples and usage guides
5. **Advanced monitoring** and benchmarking tools
6. **Error recovery** and resilience features
7. **Custom extension** support for advanced users

These enhancements make NLM more accessible, robust, and powerful for Python users while maintaining the core scientific functionality of the neural simulation framework.

## Version Information

- **Version**: 0.1.0
- **Author**: NLM Authors
- **License**: MIT
- **Description**: Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework

## Compatibility

- **Python**: 3.8+
- **C++**: C++20
- **Build System**: CMake 3.16+
- **Dependencies**: pybind11, scikit-build-core, pytest, numpy

## Support

For support and questions, please refer to the documentation files:
- `NLM_PYTHON_ENHANCED_EXAMPLES.md` - Examples and usage guide
- `INSTALL_ENHANCED.md` - Installation and setup guide
- `ENHANCED_BINDINGS_SUMMARY.md` - Complete enhancement documentation

The enhanced NLM Python bindings are ready for production use and provide a comprehensive, Pythonic API for working with neural simulation systems.
