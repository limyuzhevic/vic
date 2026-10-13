# Enhanced NLM Python Bindings - Documentation

## Overview

This document provides comprehensive information about the enhanced NLM (Neural Learning Machine) Python bindings for Phase 6: Final Integration of Artificial Brain Systems.

### What Are Enhanced Bindings?

The enhanced Python bindings represent a significant upgrade over the basic NLM Python bindings, providing researchers and developers with:

1. **Complete Brain Introspection** - Full access to all brain systems and state
2. **Advanced Memory System Access** - Direct control over working memory, episodic memory, and associative memory
3. **Enhanced Neuromodulation Control** - Fine-tuning of dopamine, curiosity, novelty, and prediction error systems
4. **Batch Operations** - Improved performance through efficient batch processing
5. **Development Stage Control** - Precise control over brain development and maturation
6. **Configuration Templates** - Pre-built configurations for common research scenarios
7. **Comprehensive Error Handling** - Meaningful error messages and debugging tools
8. **Performance Monitoring** - Built-in benchmarking and performance tracking
9. **Convenience Methods** - High-level methods for common workflows
10. **Documentation and Examples** - Extensive documentation and working examples

## Installation

### Prerequisites

- Python 3.8 or higher
- pip (Python package installer)

### Building Enhanced Bindings

1. **Clone the repository** (if not already done):
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. **Build the C++ core**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Build and install Python bindings**:
   ```bash
   cd python
   pip install -e .
   ```

### Alternative: Using Pre-built Wheels

If you have internet access, you can install directly:
```bash
pip install nlm-enhanced
```

## Basic Usage

### Quick Start (Simplified)

```python
import pynlm

# Create brain with research-grade configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Completed {brain.getTotalNeuronCount()} neurons simulation")
```

### Configuration Management

```python
import pynlm

# Using presets
config = pynlm.getMinimalConfig()  # For testing
config = pynlm.getStandardConfig()  # For general use
config = pynlm.getResearchConfig()  # For complex scenarios

# Modify configuration
config.set("brain.neuron_count", 3000)
config.set("brain.region_count", 4)
config.set("development.enable", True)

# Save/load configuration
config.saveToFile("my_config.json")
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

### Advanced Brain Introspection

```python
import pynlm

config = pynlm.getResearchConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get comprehensive brain statistics
stats = brain.getNeuronStats()
print(f"Total neurons: {stats['total']}")
print(f"Active neurons: {stats['active']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Total spikes: {stats['spike_count']}")

# Get region information
regions = brain.getRegionStats()
for region in regions:
    print(f"Region {region['id']}: {region['neuron_count']} neurons")

# Get neuromodulation status
neuromod = brain.getNeuromodulationStats()
for key, value in neuromod.items():
    print(f"{key}: {value:.3f}")
```

### Enhanced Agent Simulation

```python
import pynlm

# Setup environment
config = pynlm.getStandardConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = agent.getNeuromodulationLevel()
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

## Advanced Features

### 1. Configuration Presets

```python
import pynlm

# Available presets
presets = pynlm.PRESETS
for name, config in presets.items():
    print(f"\n{name.upper()} Preset:")
    for key, value in config.items():
        print(f"  {key}: {value}")
```

### 2. Brain State Analysis

```python
import pynlm

config = pynlm.getResearchConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Comprehensive analysis
stats = brain.getNeuronStats()
regions = brain.getRegionStats()
neuromod = brain.getNeuromodulationStats()
memory = brain.getMemoryStats()

print("=== BRAIN ANALYSIS REPORT ===")
print(f"Neuron Statistics: {stats}")
print(f"Regional Statistics: {len(regions)} regions")
print(f"Neuromodulation: {neuromod}")
print(f"Memory Systems: {memory}")
```

### 3. Batch Operations

```python
import pynlm

config = pynlm.getStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Single step processing
for step in range(100):
    brain.step(step)

# Batch processing (more efficient)
brain.stepMultiple(0, 1000, 0.001)
```

### 4. Development Control

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Initial stage: {brain.getDevelopmentalStage()}")

# Simulate development
for step in range(0, 600, 100):
    for i in range(10):
        brain.step(step + i)
    
    stage = brain.getDevelopmentalStage()
    print(f"After {step + 10} steps: {stage}")
    print(f"Plasticity factor: {stage.plasticity_factor():.3f}")
```

### 5. Complete Episode Simulation

```python
import pynlm

def run_complete_episode():
    config = pynlm.getResearchConfig()
    brain = pynlm.createBrain(config)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    world.setAgentStart(7.5, 7.5)
    
    # Run episode
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Process feedback
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    return brain, agent, world

# Run episode
brain, agent, world = run_complete_episode()
print(f"Episode completed!")
print(f"Final stage: {brain.getDevelopmentalStage()}")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
```

## API Reference

### Core Classes

#### Brain
- `Brain(config)` - Create brain with configuration
- `brain.initialize()` - Initialize brain
- `brain.step(step)` - Perform simulation step
- `brain.step(step, time)` - Perform step with timestamp
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.getNeuronStats()` - Get comprehensive neuron statistics
- `brain.getRegionStats()` - Get regional statistics
- `brain.getNeuromodulationStats()` - Get neuromodulation statistics
- `brain.getMemoryStats()` - Get memory system statistics
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count

#### Config
- `Config()` - Create default configuration
- `config.loadFromFile(filepath)` - Load from JSON file
- `config.saveToFile(filepath)` - Save to JSON file
- `config.get(key)` - Get configuration value
- `config.set(key, value)` - Set configuration value
- `config.getOr(key, default)` - Get with default
- `config.applyPreset(name)` - Apply predefined preset
- `config.to_dict()` - Convert to dictionary
- `config.validateSchema(schema)` - Validate configuration

#### AgentBrain
- `AgentBrain(brain)` - Create agent brain interface
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.applyRewardModulation(reward, predicted)` - Apply reward modulation
- `agent.updateDevelopment(timestep)` - Update development
- `agent.enableRewardModulation(enable)` - Enable reward modulation
- `agent.enableDevelopment(enable)` - Enable development
- `agent.enableCuriosity(enable)` - Enable curiosity

#### SimpleWorld
- `SimpleWorld()` - Create simple world
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.reset()` - Reset world
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(action, time)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body state

### Factory Functions

- `pynlm.createDefaultConfig()` - Create default configuration
- `pynlm.createBrain(config)` - Create brain with configuration
- `pynlm.createSimpleWorld()` - Create simple world
- `pynlm.createAgentBrain(brain)` - Create agent brain interface
- `pynlm.createVision(width, height, channels)` - Create vision input
- `pynlm.createAudio(sampleRate, numSamples)` - Create audio input
- `pynlm.createAction(type, parameters)` - Create action

### Constants and Utilities

- `pynlm.PRESETS` - Dictionary of configuration presets
- `pynlm.version()` - Get bindings version
- `pynlm.getMinimalConfig()` - Get minimal configuration
- `pynlm.getStandardConfig()` - Get standard configuration
- `pynlm.getResearchConfig()` - Get research configuration

### Enumerations

- `NeuronType` - Neuron types (Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal)
- `SynapseType` - Synapse types (Excitatory, Inhibitory, Modulatory, Electrical, GapJunction)
- `DevelopmentalStage` - Developmental stages (Initial, CriticalPeriod, Maturation, Adult, Aging)

## Performance Considerations

### Batch Operations

The enhanced bindings provide batch operations for improved performance:

```python
# Single step processing
for step in range(1000):
    brain.step(step)

# Batch processing (recommended for large simulations)
brain.stepMultiple(0, 1000, 0.001)
```

### Memory Efficiency

- Use `brain.stepMultiple()` for long simulations
- Reuse configuration objects when possible
- Use appropriate preset configurations for your use case

### Benchmarking

```python
import time

config = pynlm.getStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Average time per step: {end_time-start_time)/1000*1000:.2f} ms")
print(f"Steps per second: {1000/(end_time-start_time):.0f}")
```

## Error Handling

### Common Exceptions

The enhanced bindings provide meaningful error messages:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
```

### Debugging Tools

```python
# Get detailed debug information
debug_info = brain.getDebugInfo()
print(f"Debug info: {debug_info}")

# Check configuration validity
is_valid, errors = config.validateSchema(schema)
if not is_valid:
    print(f"Configuration errors: {errors}")
```

## Examples and Tutorials

### Basic Usage Tutorial

1. **Setup and Configuration**:
   ```python
   import pynlm
   
   # Create and configure brain
   config = pynlm.getStandardConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   ```

2. **Run Simulation**:
   ```python
   # Run simple simulation
   for step in range(100):
       brain.step(step)
   ```

3. **Analyze Results**:
   ```python
   # Get statistics
   stats = brain.getNeuronStats()
   regions = brain.getRegionStats()
   ```

### Advanced Tutorial

1. **Create Research-Grade Simulation**:
   ```python
   config = pynlm.getResearchConfig()
   # Modify as needed for your research
   config.set("brain.neuron_count", 5000)
   ```

2. **Use Enhanced Agent**:
   ```python
   world = pynlm.createSimpleWorld()
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   agent.enableRewardModulation(True)
   ```

3. **Analyze Performance**:
   ```python
   # Use batch operations
   brain.stepMultiple(0, 1000, 0.001)
   
   # Get performance stats
   stats = brain.getNeuronStats()
   ```

## Migration Guide

### From Basic to Enhanced Bindings

The enhanced bindings maintain backward compatibility while adding new functionality:

**Basic bindings maintained**:
- All original classes and methods work unchanged
- Same API for basic operations
- Same factory functions

**New functionality added**:
- Enhanced configuration management
- Brain state analysis
- Batch operations
- Development control
- Convenience methods

**Example Migration**:

**Before (basic)**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)
```

**After (enhanced)**:
```python
import pynlm

# Use preset for better defaults
config = pynlm.getStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)

# Get enhanced analysis
stats = brain.getNeuronStats()
regions = brain.getRegionStats()
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Build errors**

**Solution**:
```bash
# Rebuild with enhanced bindings
cd /workspace/app
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd python
pip install -e .
```

**Issue 2: Configuration validation errors**

**Solution**:
```python
# Test configuration validity
schema = {
    "required": ["brain.neuron_count"],
    "type": {"brain.neuron_count": "int"}
}

is_valid, errors = config.validateSchema(schema)
if not is_valid:
    print(f"Fix these errors: {errors}")
```

**Issue 3: Performance issues**

**Solution**:
```python
# Use batch operations
brain.stepMultiple(0, 1000, 0.001)

# Use appropriate preset
config = pynlm.getMinimalConfig()  # For testing
config = pynlm.getResearchConfig()  # For production
```

### Getting Help

- **Documentation**: See `docs/` directory for detailed documentation
- **Examples**: Run `python examples/` for working examples
- **Issues**: Report bugs at <issue-tracker-url>
- **Community**: Join <community-discussion-url>

## Compatibility

### Python Version

- Compatible with Python 3.8 and higher
- Tested with Python 3.8, 3.9, 3.10, 3.11, 3.12

### C++ Backend

- Requires C++20 compiler
- Built with pybind11 version 2.11.0+
- Requires scikit-build-core for build system

### Installation Notes

- **Development Installation**: `pip install -e .` (editable install)
- **Regular Installation**: `pip install .`
- **System Dependencies**: CMake 3.16+, Python development headers

## Future Enhancements

The enhanced bindings provide a foundation for future improvements:

1. **Machine Learning Integration** - ML-based plasticity rules
2. **Distributed Computing** - Multi-process simulations
3. **Advanced Visualization** - Real-time brain state visualization
4. **Cloud Integration** - Remote simulation execution
5. **Plugin Architecture** - Extensible with custom components

## Summary

The enhanced NLM Python bindings provide comprehensive support for Phase 6: Final Integration of Artificial Brain Systems. Key improvements include:

- **Complete brain introspection** with detailed analysis tools
- **Enhanced memory system** access and control
- **Advanced neuromodulation** interfaces
- **Performance optimizations** through batch operations
- **Development stage** control and simulation
- **Configuration templates** for common scenarios
- **Comprehensive error handling** and debugging tools
- **Extensive documentation** and examples

These bindings enable researchers and developers to build sophisticated neural simulations while maintaining ease of use for beginners.

---

**Version**: Enhanced v1.0 (Phase 6)
**License**: MIT
**Documentation**: https://github.com/nlm-project/nlm/tree/Phase6/docs
**Examples**: https://github.com/nlm-project/nlm/tree/Phase6/examples
