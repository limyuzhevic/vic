# NLM Python Bindings - Enhanced API Documentation

## Overview

This document provides comprehensive documentation for the enhanced Python bindings of the NLM (Neural Learning Machine) framework. The bindings have been completely redesigned to provide:

- **Complete type coverage** for all NLM systems
- **Robust error handling** with specific exception types
- **Advanced Python API** with enhanced usability
- **Memory-efficient data structures** for large datasets
- **Comprehensive documentation** with examples

## Key Improvements

### 1. Enhanced Error Handling

The new bindings provide specific exception types for different error scenarios:

```python
import pynlm as nlm

# Specific exceptions for different error types
try:
    action = nlm.Action()
    action.setParameter(100, 5.0)  # Index out of range
except nlm.InvalidParameterError as e:
    print(f"Invalid parameter: {e}")

try:
    brain.save("/nonexistent/path/file.bin")
except nlm.FileNotFoundError as e:
    print(f"File not found: {e}")

try:
    brain.initialize()
except nlm.IntegrationError as e:
    print(f"Integration error: {e}")
```

### 2. Complete Type Coverage

All NLM systems are now fully exposed to Python:

```python
# Memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Development system
development_system = brain.getDevelopmentSystem()

# Prediction system
prediction_system = brain.getPredictionSystem()
```

### 3. Enhanced Configuration API

The configuration class now provides type-safe getters and comprehensive management:

```python
config = nlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1000)
config.set("connection_probability", 0.15)
config.set("enable_development", True)

# Get configuration values with type safety
neuron_count = config.get("neuron_count", 0)  # int64_t
connection_prob = config.get("connection_probability", 0.0)  # double
enable_dev = config.get("enable_development", False)  # bool
string_val = config.get("some_string", "default")  # std::string

# Get all configuration as dictionary
all_config = config.getAll()
for key, value in all_config.items():
    print(f"{key}: {value}")
```

### 4. Enhanced Action Class

The Action class now provides comprehensive parameter management:

```python
# Create action with parameters
action = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.8, 1.2])

# Enhanced parameter access
print(action.getName())  # "MoveForward"
print(action.getParameters())  # [0.5, 0.8, 1.2]
print(action.getParameterCount())  # 3

# Safe parameter access
try:
    param_value = action.getParameter(1)
    print(f"Parameter 1: {param_value}")
except nlm.InvalidParameterError as e:
    print(f"Error: {e}")

# Check if action has specific parameter
if action.hasParameter("move"):
    print("Action has 'move' parameter")

# Parameter modification
action.setParameter(0, 0.9)  # Update first parameter
print(f"Updated parameters: {action.getParameters()}")

# Clone action
cloned = action.clone()
print(f"Cloned action: {cloned.getName()}")

# Check if ready
if action.isReady():
    print("Action is ready for execution")
```

### 5. Enhanced World Object System

World objects now provide comprehensive functionality:

```python
world = nlm.createSimpleWorld()
world.initialize(64, 64, 32, 32)

# Create objects with different properties
player = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 100.0, 0.8)
player.setVelocity(1.5, 0.5)
player.setHealth(150.0)

hazard = nlm.WorldObject(30.0, 30.0, nlm.WorldObjectType.Hazard, 50.0, 1.0)

wall = nlm.WorldObject(20.0, 20.0, nlm.WorldObjectType.Wall, 0.0, 2.0)

# Add objects to world
world.addObject(player)
world.addObject(hazard)
world.addObject(wall)

# Enhanced object interactions
distance = player.getDistance(15.0, 15.0)
print(f"Distance to (15,15): {distance:.2f}")

if player.isDestroyed():
    print("Player is destroyed")

# Get velocity
vx, vy = player.getVelocity()
print(f"Player velocity: ({vx}, {vy})")

# Get nearby objects
nearby = world.getObjectsNear(10.0, 10.0, 5.0)
print(f"Found {len(nearby)} objects near player")
```

### 6. MemoryView Support

For efficient access to large data arrays:

```python
vision = nlm.Vision(128, 128)
vision_data = [0.1] * (128 * 128 * 3)  # RGB data
vision.setData(vision_data)

# Get memoryview for efficient access
data_view = vision.getDataPtr()
print(f"Vision data shape: {len(data_view)}")

# Useful for performance-critical applications
import numpy as np
# Convert to numpy array if needed
vision_array = np.array(data_view)
```

### 7. Comprehensive Exception Hierarchy

```python
# All available exception types
exceptions = [
    nlm.InvalidNeuronIdError,
    nlm.InvalidSynapseIdError,
    nlm.FileNotFoundError,
    nlm.InvalidParameterError,
    nlm.MemoryAllocationError,
    nlm.IntegrationError,
]

for exc in exceptions:
    print(f"Exception: {exc.__name__}")
```

## Advanced Usage Examples

### 1. Complete Brain Simulation Pipeline

```python
import pynlm as nlm

def run_simulation():
    # Create configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    
    # Create brain and initialize
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = nlm.createSimpleWorld()
    world.initialize(100, 100, 50, 50)
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Set starting position
    world.setAgentStart(50, 50)
    
    # Run simulation loop
    for step in range(1000):
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step, step * 0.001)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, step * 0.001)
        
        # Check progress
        if step % 100 == 0:
            print(f"Step {step}: Reward = {brain.getAverageFiringRate():.3f}")
    
    return brain

# Run simulation
brain = run_simulation()
print("Simulation completed successfully!")
```

### 2. Memory Management and Cleanup

```python
def manage_memory():
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Store some data
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    # Perform operations
    for i in range(100):
        # Process and store data
        percept = get_sensory_input()
        brain.processSensoryInput(percept)
        
        # Check memory usage
        if working_memory.getActiveTraces() > 1000:
            print("High memory usage detected")
    
    # Cleanup
    brain.reset()
    print("Memory management completed")
```

### 3. Configuration Loading and Saving

```python
def manage_configuration():
    config = nlm.createDefaultConfig()
    
    # Load configuration from JSON
    config_file = "../configs/experiment_config.json"
    if config.loadFromFile(config_file):
        print(f"Loaded configuration from {config_file}")
    
    # Modify configuration
    config.set("simulation_timestep", 0.0001)
    config.set("enable_plasticity", True)
    
    # Save configuration
    output_file = "experiment_config.json"
    config.saveToFile(output_file)
    print(f"Saved configuration to {output_file}")
    
    # Load from command line args
    config.loadFromArgs(len(sys.argv), sys.argv)
```

## Performance Considerations

### 1. Memory Efficiency

- Use `getDataPtr()` for vision/audio data to avoid copying
- For large datasets, use `memoryview` objects
- Avoid creating excessive intermediate objects

### 2. Error Handling Efficiency

- Catch specific exceptions rather than broad exceptions
- Use try-catch blocks around potentially failing operations
- Validate inputs before operations

### 3. Data Access Patterns

```python
# Efficient pattern for accessing large data
vision = nlm.Vision(1024, 1024)
data_view = vision.getDataPtr()

# Process in chunks for large datasets
chunk_size = 256
for i in range(0, len(data_view), chunk_size):
    chunk = data_view[i:i + chunk_size]
    process_chunk(chunk)
```

## Installation and Setup

### 1. From Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the Python bindings
mkdir build
cd build
cmake ..
make -j4

# Install Python package
pip install -e python/
```

### 2. From PyPI (when available)

```bash
pip install nlm-neural-learning-machine
```

### 3. Development Installation

```bash
# For development with editable install
cd nlm
pip install -e .
```

## API Reference

### Core Classes

| Class | Module | Description |
|-------|--------|-------------|
| `Config` | Core | Configuration management |
| `Brain` | Brain | Central neural simulation engine |
| `AgentBrain` | Agent | Interface between brain and world |
| `SimpleWorld` | World | 2D simulation environment |
| `Action` | Motor | Motor action representation |
| `WorldObject` | World | World object representation |
| `SensoryPercept` | Agent | Sensory perception data |

### Enum Types

- `NeuronType`: Neuron classification
- `SynapseType`: Synapse classification
- `ActionType`: Action type enumeration
- `MotorCommand`: Motor commands
- `WorldObjectType`: Object types in world
- `DevelopmentalStage`: Developmental stages
- `FiringState`: Neuron firing states

### Exception Types

- `InvalidNeuronIdError`: Invalid neuron identifier
- `InvalidSynapseIdError`: Invalid synapse identifier
- `FileNotFoundError`: File not found error
- `InvalidParameterError`: Invalid parameter error
- `MemoryAllocationError`: Memory allocation error
- `IntegrationError`: System integration error

## Migration Guide

### From Old API to New API

**Old API (Limited Functionality):**
```python
# Old: Basic configuration
config = nlm.Config()
config.loadFromFile("config.json")

# Old: Basic brain creation
brain = nlm.Brain(config)
brain.initialize()

# Old: Basic world
world = nlm.SimpleWorld()
world.initialize(16, 16)
```

**New API (Enhanced Functionality):**
```python
# New: Enhanced configuration with type safety
config = nlm.createDefaultConfig()
config.loadFromFile("config.json")
config.set("neuron_count", 1000)  # Type-safe

# New: Enhanced brain creation
brain = nlm.createBrain(config)
brain.initialize()

# New: Enhanced world with more features
world = nlm.createSimpleWorld()
world.initialize(16, 16, 8, 8)  # More configuration options
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: "ModuleNotFoundError: No module named 'pynlm'"**

**Solution:**
```bash
# Build the Python bindings first
cd python
mkdir build
cd build
cmake ..
make

# Install the package
pip install -e .
```

**Issue 2: "ImportError: libpynlm.so: cannot open shared object file"**

**Solution:**
```bash
# Set LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH

# Or install to system library path
sudo make install
```

**Issue 3: Memory errors with large simulations**

**Solution:**
```python
# Use memory-efficient patterns
data_view = vision.getDataPtr()
# Process data in chunks rather than loading everything at once
```

**Issue 4: Performance issues**

**Solution:**
```python
# Profile your code
import cProfile
import pstats

# Profile critical sections
cProfile.run('run_simulation()', 'profile_stats')
stats = pstats.Stats('profile_stats')
stats.sort_stats('cumulative').print_stats()
```

## Future Enhancements

The Python bindings are designed to be easily extensible. Future enhancements may include:

1. **Machine Learning Integration**: Integration with scikit-learn and TensorFlow
2. **Advanced Visualization**: 3D plotting and interactive visualizations
3. **Parallel Processing**: Multi-threading and GPU acceleration
4. **Distributed Computing**: Multi-machine simulation capabilities
5. **Advanced Memory Management**: Better garbage collection and memory pools
6. **Additional Neural Models**: Support for other neuron models (e.g., Hodgkin-Huxley)
7. **Plugin Architecture**: Extensible plugin system for custom brain modules

## Contributing

### Development Guidelines

1. **Code Style**: Follow the existing C++ coding style guidelines
2. **Documentation**: Maintain comprehensive docstrings for all bindings
3. **Error Handling**: Use specific exception types and provide clear error messages
4. **Testing**: Write comprehensive tests for new bindings
5. **Performance**: Optimize for both usability and performance

### Adding New Bindings

To add new bindings to the Python API:

1. **Add C++ headers** for new classes/functions
2. **Create pybind11 bindings** in `python/bindings.cpp`
3. **Update CMakeLists.txt** if needed
4. **Test thoroughly** with Python examples
5. **Update documentation** with new examples

### Example: Adding a New Class

```cpp
// In python/bindings.cpp
py::class_<NewClass>(m, "NewClass", "Documentation for new class")
    .def(py::init<>())
    .def("method", &NewClass::method, "Method documentation")
    .def_readwrite("property", &NewClass::property, "Property documentation");
```

## Acknowledgments

The NLM Python bindings project is made possible by:

- **pybind11**: The C++ Python bindings library
- **CMake**: The build system
- **Python**: The target language
- **NLM Research Team**: For developing the underlying neural simulation framework
- **Contributors**: For improvements and enhancements

## License

This project is licensed under the MIT License. Please see the LICENSE file for details.

## Contact

For questions, issues, or contributions:

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Email**: info@nlm-project.org
- **Documentation**: https://nlm.readthedocs.io

---

*This documentation was generated by analyzing the enhanced NLM Python bindings API and provides comprehensive guidance for users and developers.*

_Last updated: 2026-09-27_