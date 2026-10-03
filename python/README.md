# NLM Python Bindings

This directory contains Python bindings for the NLM (Neural Learning Machine) project.

## Overview

NLM is a brain simulation project written in C++ with Python bindings using pybind11.
The Python API provides a simplified interface to the complex neural simulation capabilities.

## Current Status

**Bindings: NOT IMPLEMENTED**

The Python bindings shown in the documentation (easy_usage.md, HOW_TO_USE.md) are not yet implemented.
This directory contains:

### TODO: Pybind11 Binding Files
- `brain_binding.cpp` - Bindings for Brain class
- `config_binding.cpp` - Bindings for Config class  
- `world_binding.cpp` - Bindings for SimpleWorld class
- `agent_binding.cpp` - Bindings for AgentBrain class
- `neuron_binding.cpp` - Bindings for Neuron class

### Documentation
- `python_api.md` - Python API documentation (to be generated)
- `examples/` - Example Python scripts

## Building Python Bindings

To build the Python bindings, you need:

1. Install pybind11:
   ```bash
   pip install pybind11
   ```

2. Build the Python module:
   ```bash
   cd /path/to/nlm
   pip install -e .
   ```

Or build with scikit-build:
   ```bash
   pip install scikit-build
   pip install build
   python -m build
   ```

## Python API Examples

Based on the documentation, the NLM Python API includes:

### Brain Class
```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### SimpleWorld Class
```python
import pynlm

# Create and configure a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Update world
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
```

### AgentBrain Class
```python
import pynlm

# Create agent with brain and world
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Implementation Plan

### Phase 1: Core Bindings
1. **Config class** - Configuration management
2. **Brain class** - Neural simulation core
3. **SimpleWorld class** - Environment simulation

### Phase 2: Advanced Bindings
1. **AgentBrain class** - Sensorimotor integration
2. **Neuromodulation classes** - Reward, curiosity, novelty
3. **Memory classes** - Working memory, episodic memory

### Phase 3: Advanced Features
1. **Development system** - Brain maturation
2. **Visualization** - Result visualization
3. **Checkpointing** - Save/load brain states

## Pybind11 Integration

The NLM Python bindings will use pybind11 to expose the C++ classes to Python. Key features:

- **Type safety**: Python's static typing will catch many errors
- **Performance**: C++ core with Python interface
- **Ease of use**: Simplified Python API
- **Interoperability**: Integration with existing Python ecosystem

## Benefits

1. **Accessibility**: Researchers can use NLM without C++ expertise
2. **Research integration**: Easy to integrate with Python ML libraries
3. **Reproducibility**: Python scripts are more reproducible than C++
4. **Visualization**: Easy to plot results with matplotlib
5. **Testing**: Python-based testing framework

## Files to Create

1. **Core binding files** (in src/):
   - `bindings/brain_pybind.cpp`
   - `bindings/config_pybind.cpp`
   - `bindings/world_pybind.cpp`
   - `bindings/agent_pybind.cpp`

2. **Python package setup**:
   - `pyproject.toml`
   - `setup.py` (legacy)
   - `setup.cfg` (alternative)

3. **Documentation**:
   - `docs/python_api.md`
   - `examples/basic_usage.py`
   - `examples/agent_simulation.py`

4. **Testing**:
   - `tests/python_test.py`
   - `tests/test_bindings.py`

## Next Steps

1. **Set up pyproject.toml** with scikit-build-core configuration
2. **Create basic bindings** for the core classes
3. **Implement the main binding functions** from the documentation
4. **Create comprehensive tests** to verify bindings work
5. **Generate documentation** from the bindings
6. **Optimize performance** for large simulations

## Technical Considerations

1. **Memory management**: Use pybind11's smart pointer bindings
2. **Error handling**: Translate C++ exceptions to Python exceptions
3. **Type conversions**: Handle conversion between C++ and Python types
4. **Performance**: Minimize overhead from Python binding calls
5. **Debugging**: Provide helpful Python stack traces for C++ errors

This project aims to create a robust Python interface to the NLM brain simulation capabilities, making advanced neural simulation accessible to a broader research community.