# Python Bindings for NLM - Core Infrastructure

This directory contains the Python bindings for NLM (Neural Learning Machine).
The bindings provide Python access to all NLM functionality as documented in the
Python documentation.

## Structure

```
python/
├── __init__.py                    # Top-level package
├── pynlm.pyx                      # Cython bindings
├── pynlm.pxd                      # Cython declarations
├── _pynlm.cpython-38-x86_64-linux-gnu.so  # Compiled extension
├── pynlm_advanced.py              # Advanced Python API
├── cli.py                         # Command line interface
├── visualization.py               # Visualization tools
├── examples/                      # Python examples
└── docs/                          # Documentation
```

## Requirements

- Cython >= 3.0
- Python >= 3.8
- NumPy (optional, for advanced features)

## Building

### Using pip (Development)
```bash
pip install -e .
```

### Using setup.py
```bash
python setup.py build_ext --inplace
```

### Using pyproject.toml
```bash
python -m pip install build
python -m build
```

## Quick Start

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run the brain for 100 steps
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

## Module Contents

### pynlm
The main NLM Python module providing access to all NLM functionality.

### pynlm_advanced
Extended functionality for advanced users including:
- Enhanced memory management
- Advanced prediction systems
- Complex cognitive operations
- Performance monitoring

### cli
Command line interface for running NLM simulations and experiments.

### visualization
Visualization tools for NLM systems including:
- Network connectivity visualization
- Neural activity visualization
- Memory trace visualization
- Learning curve analysis

### examples
Comprehensive examples for all use cases:
- Beginner tutorials
- Intermediate examples
- Advanced demonstrations
- Research experiments

## Installation

### System Dependencies
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install python3-dev python3-pip cython3 build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install python3-devel python3-pip cython3

# macOS
brew install python3 cython
```

### Python Package
```bash
pip install pynlm
```

### Development Installation
```bash
pip install -e .
```

## Usage Examples

### Basic Brain Simulation
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Print statistics
print(f"Simulation complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Brain with World
```python
import pynlm

# Create brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
```

### Advanced Features
```python
import pynlm

# Enable advanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Enhanced simulation complete!")
```

## API Reference

### Brain Functions

#### createBrain(config)
Creates a virtual brain with neurons and synapses.

**Parameters:**
- `config`: Brain configuration (use `createDefaultConfig()` for simple setup)

**Returns:** Brain object

#### brain.initialize()
Starts up the brain. Always call this before using the brain!

**Parameters:** None

**Returns:** None

#### brain.step(step_number)
Makes the brain process one moment in time.

**Parameters:**
- `step_number`: Step number for this processing step

**Returns:** None

### World Functions

#### createSimpleWorld()
Creates a simple world for brain-agent interaction.

**Parameters:** None

**Returns:** World object

#### world.configure(width, height, visionWidth, visionHeight)
Configures the world's dimensions and agent's vision.

**Parameters:**
- `width`: World width
- `height`: World height
- `visionWidth`: Agent's visual field width
- `visionHeight`: Agent's visual field height

**Returns:** None

#### world.update(time)
Updates the virtual world by `time` seconds.

**Parameters:**
- `time`: Time step in seconds

**Returns:** None

#### world.reset()
Resets the world to initial state.

**Parameters:** None

**Returns:** None

### Agent Functions

#### createAgentBrain(brain)
Creates an agent brain that connects brain to world.

**Parameters:**
- `brain`: Brain object to connect

**Returns:** Agent object

#### agent.initialize(world)
Initializes the agent with the world.

**Parameters:**
- `world`: World object to interact with

**Returns:** None

#### agent.processSensoryInput(percept)
Gives sensory information to the brain.

**Parameters:**
- `percept`: Sensory perception data

**Returns:** None

#### agent.decodeMotorCommand()
Reads the brain's motor neurons to decide what action to take.

**Parameters:** None

**Returns:** Motor command

#### agent.enableRewardModulation(enabled)
Enables/disables reward-based learning.

**Parameters:**
- `enabled`: True to enable, False to disable

**Returns:** None

#### agent.enableStructuralPlasticity(enabled)
Enables/disables structural plasticity (growing new connections).

**Parameters:**
- `enabled`: True to enable, False to disable

**Returns:** None

#### agent.enableDevelopment(enabled)
Enables/disables brain development over time.

**Parameters:**
- `enabled`: True to enable, False to disable

**Returns:** None

#### agent.enableCuriosity(enabled)
Enables/disables curiosity-driven exploration.

**Parameters:**
- `enabled`: True to enable, False to disable

**Returns:** None

### Statistics Functions

#### brain.getTotalNeuronCount()
Returns total number of neurons in the brain.

**Parameters:** None

**Returns:** Number of neurons

#### brain.getTotalSpikeCount()
Returns total number of spikes generated by the brain.

**Parameters:** None

**Returns:** Number of spikes

#### brain.getFiringNeuronCount()
Returns number of neurons currently firing.

**Parameters:** None

**Returns:** Number of firing neurons

#### brain.getAverageFiringRate()
Returns average firing rate of the brain.

**Parameters:** None

**Returns:** Average firing rate

#### brain.getDevelopmentalStage()
Returns current developmental stage of the brain.

**Parameters:** None

**Returns:** Developmental stage

## Error Handling

The Python bindings raise appropriate exceptions for:

- `ValueError`: Invalid parameters or configuration
- `RuntimeError`: Brain not initialized or other runtime errors
- `MemoryError`: Insufficient memory for operations
- `ImportError`: Missing dependencies

## Performance Tips

### Memory Management
- Reuse brain and world objects when possible
- Clear memory periodically in long-running simulations
- Use appropriate configuration to manage memory usage

### Simulation Speed
- Use smaller brain configurations for faster simulations
- Configure appropriate timestep for your use case
- Consider using `enableStructuralPlasticity(False)` for faster simulations

### Multi-threading
- The Cython extension is thread-safe for independent simulations
- Avoid sharing brain objects across threads
- Consider using separate instances for parallel simulations

## Troubleshooting

### Import Error
```bash
# Install NLM Python bindings
pip install pynlm
```

### Runtime Error: Brain not initialized
```python
# Ensure brain.initialize() is called
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # This line is required!
```

### Attribute Error
```python
# Ensure you're using the correct API
# Some functions may have different names in the actual implementation
brain.step(step)  # Correct
brain.step(step_number)  # Alternative
```

### Performance Issues
```python
# Optimize your simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig(max_neurons=100))
# Use smaller configuration for better performance
```

## Contributing

### Contributing to Python Bindings
1. Fork the repository
2. Create a feature branch
3. Implement changes
4. Test with existing examples
5. Update documentation
6. Submit a pull request

### Adding Examples
1. Add to `examples/` directory
2. Update `docs/PYTHON_COOKBOOK.md`
3. Test with the examples
4. Ensure compatibility with all supported Python versions

### Improving Performance
1. Profile your simulation
2. Identify bottlenecks
3. Optimize Python code
4. Consider using `pynlm_advanced` for optimized operations
5. Report performance improvements

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2026 NLM Project
All rights reserved.
