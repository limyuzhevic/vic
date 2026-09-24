# NLM Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the **NLM Python bindings**, which is a complete Python interface to the NLM (Neural Learning Machine) C++ neural simulation framework.

## What's New

### Major Improvements

#### 1. **Complete Class Coverage**
- Added bindings for all missing key classes:
  - `Neuron`, `Synapse`, `NeuralRegion`, `NeuralPopulation`
  - `Dopamine`, `Curiosity`, `Novelty`, `PredictionError` (neuromodulators)
  - `WorkingMemory`, `EpisodicMemory`, `AssociativeMemory` (memory systems)
  - `PredictionSystem`, `NeuralPlanner`, `ConceptFormation` (cognition)
  - `DevelopmentSystem`, `Maturation`, `Synaptogenesis`, `Pruning` (development)
  - And many more...

#### 2. **Enhanced Brain Class**
- Complete binding of all Brain class methods
- Access to all sub-systems (working memory, episodic memory, prediction system, etc.)
- Statistics methods for monitoring neural activity
- Developmental stage controls

#### 3. **Pythonic Interfaces**
- Method chaining and fluent APIs
- Dictionary-style serialization/deserialization
- Comprehensive helper methods (distance calculation, conversion, etc.)
- Type safety with proper Python type hints

#### 4. **Convenience Factory Functions**
- `createDefaultConfig()`: Creates configuration with sensible defaults
- `createDefaultBrainConfig()`: Configuration optimized for brain simulation
- `createSimpleAgentConfig()`: Configuration for simple agent use cases
- `createWorldConfig()`: Configuration for specific world dimensions
- `run_simulation()`: High-level function to run complete simulations
- `create_agent_system()`: Creates complete agent system (brain + world + agent)

#### 5. **Advanced APIs**
- Serialization/deserialization utilities
- Statistics and monitoring tools
- Object proximity queries
- Batch operations
- Debugging and development tools

#### 6. **Documentation and Examples**
- Comprehensive docstrings for all classes and methods
- Complete usage examples
- API reference documentation
- Tutorial-style documentation

### Before vs After

#### Before (Limited Coverage):
```python
# Only basic classes were available
config = pynlm.Config()
brain = pynlm.Brain(config)
world = pynlm.SimpleWorld()
agent = pynlm.AgentBrain(brain)

# Missing many important classes and methods
# No convenience functions
# Limited functionality
```

#### After (Complete Coverage):
```python
# Full feature set available
config = pynlm.createDefaultBrainConfig()  # Easy setup with defaults
brain = pynlm.createBrain(config)  # Factory function
world = pynlm.createSimpleWorld()  # Factory function
agent = pynlm.createAgentBrain(brain)  # Factory function

# Access to all systems
region = brain.addRegion("visual_cortex")
neuron = region.getPopulation(0).getNeuron(5)
synapse = region.addSynapse(neuron.getId(), other_neuron.getId())

# Advanced features
simulation_result = pynlm.run_simulation(brain, world, steps=1000, verbose=True)
```

## API Reference

### Core Types

#### NeuronId, SynapseId, RegionId, PopulationId
- **Purpose**: Unique identifiers for neural components
- **Usage**: 
  ```python
  neuron_id = pynlm.NeuronId(42)
  synapse_id = pynlm.SynapseId(999)
  ```

#### Enumeration Types
- **NeuronType**: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- **SynapseType**: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- **DevelopmentalStage**: Initial, CriticalPeriod, Maturation, Adult, Aging
- **FiringState**: Resting, Active, Refractory, Inhibited
- **ActionType**: MoveForward, MoveBackward, Look, Interact, etc.
- **MotorCommand**: MoveForward, TurnLeft, LookRight, etc.

### Main Classes

#### Config
- **Purpose**: Configuration management for NLM system
- **Key Methods**:
  - `loadFromFile()`: Load from JSON
  - `saveToFile()`: Save to JSON
  - `set()`: Set configuration values
  - `has()`: Check if key exists

#### Brain
- **Purpose**: Central neural simulation controller
- **Key Features**:
  - Step-based simulation
  - Sensory input processing
  - Motor action production
  - Region management
  - System statistics
  - Access to all sub-systems (memory, prediction, cognition, etc.)

#### Neuron
- **Purpose**: Individual LIF neuron implementation
- **Key Features**:
  - Membrane potential dynamics
  - Spike generation and refractory periods
  - Synaptic input handling
  - Plasticity support
  - State management

#### Synapse
- **Purpose**: Synaptic connection with plasticity
- **Key Features**:
  - Weight modulation
  - Spike history tracking
  - Eligibility traces
  - Plasticity rules

#### NeuralRegion
- **Purpose**: Brain region containing populations
- **Key Features**:
  - Population management
  - Local connectivity
  - Regional statistics
  - Random initialization

#### NeuralPopulation
- **Purpose**: Homogeneous group of neurons
- **Key Features**:
  - Bulk operations
  - Type consistency
  - Population statistics

#### SimpleWorld
- **Purpose**: 2D simulation environment
- **Key Features**:
  - Agent movement and physics
  - Object management
  - Sensory field of view
  - Proximity queries

#### AgentBrain
- **Purpose**: Interface between brain and world
- **Key Features**:
  - Sensory processing
  - Motor decoding
  - Reward modulation
  - Development integration

#### Memory Systems
- **WorkingMemory**: Transient active information
- **EpisodicMemory**: Experience storage and replay
- **AssociativeMemory**: Pattern associations

#### Neuromodulation Systems
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Prediction error signals

#### Cognition Systems
- **NeuralPlanner**: Action planning
- **ConceptFormation**: Pattern discovery
- **AttentionalSelection**: Competitive selection

#### Development Systems
- **DevelopmentSystem**: Developmental stages
- **Maturation**: Structural maturation
- **Synaptogenesis**: Synapse formation
- **Pruning**: Synapse elimination

### Convenience Functions

#### Factory Functions
```python
# Easy setup with sensible defaults
config = pynlm.createDefaultBrainConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Configuration Generators
```python
# Pre-configured for specific use cases
brain_config = pynlm.createDefaultBrainConfig()  # For complex simulations
agent_config = pynlm.createSimpleAgentConfig()   # For simple agents
world_config = pynlm.createWorldConfig(100, 100, 20, 20)
```

#### High-Level APIs
```python
# Run complete simulation
result = pynlm.run_simulation(brain, world, steps=1000, verbose=True)
spikes, avg_rate = result

# Create complete agent system
system = pynlm.create_agent_system(config)
brain, world, agent = system
agent.initialize(world)
```

### Advanced Features

#### Serialization
```python
# Convert objects to dictionaries
neuron_dict = neuron.to_dict()
action_tuple = action.to_dict()
result_tuple = result.to_dict()
```

#### Statistics and Monitoring
```python
# Get neural statistics
brain_stats = brain.getNeuronStatistics()
synapse_stats = brain.getSynapseStatistics()

# Real-time monitoring during simulation
for step in range(1000):
    # ... simulation steps ...
    if step % 100 == 0:
        brain.logStatus()
```

#### Object Relationships
```python
# Find nearby objects
nearby_walls = world.find_nearby_objects(x, y, radius)
nearby_resources = [obj for obj in world.getWorldObjects() 
                   if obj.type == pynlm.WorldObjectType.Resource and
                   world.isValidPosition(obj.x, obj.y)]
```

## Examples

### Example 1: Basic Simulation
```python
import pynlm

# Setup with sensible defaults
config = pynlm.createDefaultBrainConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

world.configure(100, 100, 20, 20)
agent.initialize(world)

# Run simulation
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step)
```

### Example 2: Complex Agent with Memory
```python
import pynlm

# Create configuration with memory support
config = pynlm.createDefaultBrainConfig()
config.set("memory.episodic.capacity", "50")
config.set("agent.max_memory", "100")

# Create system
brain, world, agent = pynlm.create_agent_system(config)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run long simulation
result = pynlm.run_simulation(brain, world, steps=10000, verbose=True)
spikes, avg_rate = result
```

### Example 3: Interactive Development
```python
import pynlm

# Create visual processing brain
config = pynlm.createDefaultBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add visual region
visual_region = brain.addRegion("visual_cortex")
visual_pop = visual_region.addPopulation(100, pynlm.NeuronType.Sensory)

# Create vision input
vision = pynlm.Vision(20, 20, 3)
vision_data = [i * 0.1 for i in range(20 * 20 * 3)]
vision.setData(vision_data)

# Process visual input
brain.receiveSensoryInput(vision)

# Monitor development
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

### Example 4: World Interaction
```python
import pynlm

# Create world with resources
world = pynlm.createSimpleWorld()
world.configure(200, 200, 30, 30)

# Add various objects
player = pynlm.WorldObject(100, 100, pynlm.WorldObjectType.Resource, 100, 1.0)
wall = pynlm.WorldObject(50, 50, pynlm.WorldObjectType.Wall, 0, 2.0)
hazard = pynlm.WorldObject(150, 150, pynlm.WorldObjectType.Hazard, -50, 0.5)

world.addObject(player)
world.addObject(wall)
world.addObject(hazard)

# Find reachable resources
reachable = world.find_nearby_objects(100, 100, 50)
resources = [obj for obj in reachable if obj.type == pynlm.WorldObjectType.Resource]

print(f"Found {len(resources)} reachable resources")
```

## Installation and Setup

### Requirements
- Python 3.8 or higher
- C++ compiler with C++20 support
- pybind11 (automatically installed via pip)
- NumPy (optional, for array operations)

### Installation Options

#### Option 1: Development Build (Recommended for Contributors)
```bash
# Clone the repository
# Build the C++ core library
mkdir build && cd build
cmake ..
make -j4

# Install the Python bindings
pip install -e .
```

#### Option 2: Using PyPI (Binary Installation)
```bash
pip install pynlm
```

### Testing
```bash
# Run the test suite
python test_bindings.py

# Run specific tests
python -m pytest test_bindings.py -v
```

### Troubleshooting

#### Common Issues
1. **Compilation Errors**: Ensure you have a C++20 compatible compiler
2. **Missing Dependencies**: Install pybind11 and NumPy
3. **Import Errors**: Ensure the module is built and in Python path

#### Build Commands
```bash
# If compilation fails, try:
pip install pybind11
pip install numpy
```

## Performance Considerations

### Memory Management
- All objects use reference counting
- Consider using `clone()` for object duplication
- Monitor memory usage in long-running simulations

### Simulation Speed
- Event-driven architecture for efficient neural computation
- Vectorization for population operations
- Configurable time steps

### Scalability
- Parallel processing support for large networks
- Memory pools for efficient allocation
- Hierarchical organization (regions → populations → neurons)

## Migration Guide

### From Old API to New API

#### Old Way (Limited):
```python
# Missing functionality
config = pynlm.Config()
brain = pynlm.Brain(config)
# No access to neurons/synapses directly
# No convenience functions
```

#### New Way (Complete):
```python
# Full functionality available
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
region = brain.addRegion("test")
population = region.addPopulation(10)
neuron = population.getNeuron(0)
```

## Future Enhancements

### Planned Features
1. **Machine Learning Integration**: Train neural networks using NLM
2. **GPU Acceleration**: CUDA support for parallel neural computation
3. **WebSocket Interface**: Real-time simulation streaming
4. **Visualization Tools**: Built-in plotting and 3D rendering
5. **Cloud Integration**: Distributed computing support
6. **Advanced APIs**: JAX interface for differentiable programming

### Contribution Guidelines
1. **Report Issues**: Use GitHub issues for bug reports
2. **Submit Pull Requests**: Follow the project's coding standards
3. **Documentation**: Update documentation for new features
4. **Testing**: Add comprehensive test coverage

## Acknowledgements

This Python bindings package is built upon:
- **pybind11**: C++/Python binding library
- **The NLM C++ Framework**: Core neural simulation engine
- **All Contributors**: To the NLM project
- **The Python Community**: For ecosystem support

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
