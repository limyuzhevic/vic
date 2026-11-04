# NLM Python Bindings - Enhanced Documentation

This document provides comprehensive information about the improved NLM Python bindings
and how to use the enhanced features for neural simulation development.

## Overview

The NLM Python bindings provide a complete Python interface to the NLM (Neural Learning Machine)
framework, enabling users to create, simulate, and analyze neural systems using Python.

**Key Improvements in Version 2.0:**
- ✅ Better organization and modularity
- ✅ Enhanced error handling and type safety
- ✅ Additional utility functions and debugging tools
- ✅ Configuration management utilities
- ✅ Performance monitoring and benchmarking
- ✅ Advanced simulation examples
- ✅ Comprehensive documentation

## Installation

### Basic Installation

```bash
pip install -e .
```

This will build the NLM C++ libraries and the enhanced Python bindings.

### Development Installation

```bash
pip install -e .[dev]
```

This includes development dependencies for testing and documentation.

## Environment Setup

### Quick Setup Script

The NLM Python bindings include a setup script with various utilities:

```python
# Import the setup utilities
import nlm_setup as setup

# Check environment
status = setup.check_environment()
print(status)

# Run a test simulation
results = setup.run_test_simulation(100)
print(results['formatted_results'])
```

### Available Commands

```bash
# Show environment information
setup --action info

# Run a test simulation
setup --action test --steps 50

# Benchmark brain creation performance
setup --action benchmark --iterations 20
```

## Basic Usage

### Quick Start (Beginner)

```python
import pynlm

# 1. Create a default configuration
config = pynlm.createDefaultConfig()

# 2. Create a brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation
for step in range(100):
    brain.step(step)

# 4. Get statistics
stats = pynlm.getBrainStatistics(brain)
print(pynlm.formatSimulationResults(stats))
```

### Advanced Usage (Example-Based)

See the examples directory for comprehensive usage patterns:

```python
# Examples directory
python/python/examples/

# Key example files:
- basic_usage.py: Basic simulation patterns
- agent_world_demo.py: Complete agent-environment integration
- brain_inspection.py: Brain state analysis and debugging
- learning_demo.py: Advanced learning with neuromodulation
- performance_monitoring.py: Simulation analysis tools
- batch_processing.py: Parameter sweeps and multiple simulations
```

## Core Classes and Their Uses

### 1. Config Class

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Configuration dictionary utility
config_dict = pynlm.createDefaultConfigDict()
print(pynlm.configToString(config_dict))
```

### 2. Brain Class

```python
# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, current_time=step * 0.1)

# Get brain statistics
stats = pynlm.getBrainStatistics(brain)

# Inspect brain state
pynlm.printBrainState(brain)

# Save and load brain state
brain.save("brain_checkpoint.pkl")
# Later...
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.load("brain_checkpoint.pkl")
```

### 3. AgentBrain Class

```python
# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Run simulation loop
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 4. SimpleWorld Class

```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)

# Set agent start position
world.setAgentStart(5, 5)

# Add objects
obj = pynlm.WorldObject(x=10, y=10, type=pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)

# Check positions
if world.isValidPosition(15, 15):
    print("Position is valid")
```

## Advanced Features

### 1. Neuromodulation and Learning

```python
# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Apply reward modulation
agent.applyRewardModulation(reward=10.0, predicted_reward=5.0)

# Update development
agent.updateDevelopment(0.1)
```

### 2. Text Processing (Phase 6 Feature)

```python
# Encode text for associative learning
text_pattern = agent.encodeText("hello world")
print(f"Text encoded as neuron indices: {text_pattern}")

# Inject text into brain for learning
# (Text is injected during sensory processing)
```

### 3. Concept Formation

```python
# Get concept information
concept_count = agent.getConceptCount()
print(f"Number of learned concepts: {concept_count}")

# Get self-model confidence
confidence = agent.getSelfModelConfidence()
print(f"Self-model confidence: {confidence}")
```

### 4. Performance Monitoring

```python
import time
import nlm_setup as setup

# Benchmark brain creation
results = setup.benchmark_brain_creation(20)
print(f"Brains per second: {results['brains_per_second']:.2f}")

# Validate brain state
issues = setup.validate_brain_state(brain)
if issues:
    print("Issues found:", issues)
```

## Configuration Utilities

### 1. Default Configuration Dictionary

```python
# Get default configuration as dictionary
config_dict = pynlm.createDefaultConfigDict()

# Display configuration
print(pynlm.configToString(config_dict))
```

### 2. Configuration Validation

```python
# Create example configuration
example_config = setup.create_example_config()

# Use configuration dictionary
print("Example configuration keys:", list(example_config.keys()))
```

## Debugging and Analysis Tools

### 1. Brain Inspection

```python
# Get detailed brain state information
brain_state = pynlm.getBrainStateString(brain)
print(brain_state)

# Get available actions
actions = pynlm.getAvailableActions(brain)
print(f"Available actions: {actions}")
```

### 2. Error Handling

```python
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Simulate operations that might fail
    for step in range(100):
        brain.step(step)
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Examples Directory Structure

```
python/examples/
├── basic_usage.py              # Basic simulation patterns
├── agent_world_demo.py          # Complete agent-environment integration
├── brain_inspection.py          # Brain state analysis and debugging
├── learning_demo.py             # Advanced learning with neuromodulation
├── performance_monitoring.py   # Simulation analysis tools
├── batch_processing.py          # Parameter sweeps and multiple simulations
├── memory_management.py         # Checkpointing and serialization
└── text_processing_demo.py      # Phase 6 text-as-a-sense capabilities
```

## Utility Functions Reference

### Core Factory Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createDefaultConfig()` | Create default configuration | `config = pynlm.createDefaultConfig()` |
| `createBrain(config)` | Create brain with configuration | `brain = pynlm.createBrain(config)` |
| `createSimpleWorld()` | Create simple world | `world = pynlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |

### Simulation Utilities

| Function | Description | Example |
|----------|-------------|---------|
| `runBasicSimulation(steps)` | Run basic simulation | `brain, spikes, neurons = pynlm.runBasicSimulation(100)` |
| `getBrainStatistics(brain)` | Get comprehensive statistics | `stats = pynlm.getBrainStatistics(brain)` |
| `formatSimulationResults(stats)` | Format statistics as string | `print(pynlm.formatSimulationResults(stats))` |

### Debug Utilities

| Function | Description | Example |
|----------|-------------|---------|
| `getBrainStateString(brain)` | Get formatted brain state | `state = pynlm.getBrainStateString(brain)` |
| `printBrainState(brain)` | Print brain state to console | `pynlm.printBrainState(brain)` |
| `getAvailableActions(brain)` | Get available motor actions | `actions = pynlm.getAvailableActions(brain)` |

### Configuration Utilities

| Function | Description | Example |
|----------|-------------|---------|
| `createDefaultConfigDict()` | Create configuration dictionary | `config_dict = pynlm.createDefaultConfigDict()` |
| `configToString(config)` | Convert configuration to string | `print(pynlm.configToString(config))` |

### System Utilities

| Function | Description | Example |
|----------|-------------|---------|
| `getBrainVersion()` | Get bindings version | `version = pynlm.getBrainVersion()` |
| `checkPyVersion()` | Check Python version compatibility | `if pynlm.checkPyVersion(): print("OK")` |

## Performance Tips

### 1. Memory Management

- Reuse Brain objects instead of creating new ones
- Use `reset()` to clear state instead of destroying and recreating
- Monitor memory usage with debugging tools

### 2. Simulation Optimization

```python
# Batch processing for multiple simulations
import nlm_setup as setup

# Run multiple simulations with different parameters
for seed in range(10):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Analyze results
    stats = pynlm.getBrainStatistics(brain)
    # ... process results
```

### 3. Debugging Large Simulations

```python
# Use debugging utilities to inspect large brains
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get statistics periodically
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        stats = pynlm.getBrainStatistics(brain)
        pynlm.printBrainState(brain)
```

## Common Issues and Solutions

### 1. "No module named 'pynlm'"

**Solution:** Ensure NLM is properly installed:

```bash
pip install -e .
```

Or build from source:

```bash
mkdir build
cd build
cmake ..
make
```

### 2. "Brain has no spikes - may be inactive"

**Solution:** Check if the brain is properly initialized and has sensory input:

```python
# Ensure brain is initialized
if not brain.initialize():
    print("Brain initialization failed")
    return

# Provide sensory input if needed
brain.receiveSensoryInput(vision_data)
```

### 3. "Brain has no neurons"

**Solution:** Check configuration:

```python
config = pynlm.createDefaultConfig()
# Ensure neuron creation parameters are set correctly
```

### 4. Performance Issues

**Solution:** Use performance monitoring tools:

```python
import nlm_setup as setup

results = setup.benchmark_brain_creation(20)
print(f"Performance: {results['brains_per_second']:.2f} brains/second")
```

## Migration Guide

### From Version 1.0 to 2.0

**New Features Added:**
- Enhanced error handling and type safety
- Configuration utilities (`createDefaultConfigDict`, `configToString`)
- Simulation utilities (`runBasicSimulation`, `getBrainStatistics`)
- Debug utilities (`getBrainStateString`, `printBrainState`)
- Performance monitoring (`benchmark_brain_creation`)
- Comprehensive examples directory
- Setup utilities (`nlm_setup.py`)

**Breaking Changes:**
- **None** - All existing APIs are preserved with enhanced functionality

**New APIs (Recommended):**
```python
# Old way (still works)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# New way (with utilities)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
stats = pynlm.getBrainStatistics(brain)
print(pynlm.formatSimulationResults(stats))
```

## Testing and Validation

### Running Tests

```bash
# Run Python tests
pytest tests/

# Run specific tests
pytest tests/test_basic_usage.py
pytest tests/test_agent_world.py
```

### Validation Functions

```python
import nlm_setup as setup

# Validate brain state
issues = setup.validate_brain_state(brain)
if issues:
    print("Validation issues:", issues)
```

## License

MIT License - See LICENSE file for details.

## Authors

NLM Authors - Research project for neural simulation

## Acknowledgements

- pybind11 for Python bindings
- scikit-build-core for build system
- The NLM research community

---

**Version 2.0 (Improved)**
*This documentation applies to NLM Python Bindings v2.0*

For more information, visit:
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)