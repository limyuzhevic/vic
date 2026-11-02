# NLM Python Bindings (pynlm)

Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework.

## Quick Start

### Installation

The Python bindings are built as part of the NLM project using CMake. To build and install:

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make
```

For development and testing, you can also build just the Python module:

```bash
mkdir python_build
cd python_build
cmake ../python
make
```

### Basic Usage

```python
import pynlm

# Create a complete NLM system using factory functions
brain, world, agent_brain = pynlm.create_nlm_system()
agent_brain.initialize(world)

# Run simulation for multiple steps
for step in range(10):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.1)
    brain.step(step, step * 0.1)

# Get system statistics
stats = pynlm.get_system_stats(brain)
print(f"Neurons: {stats['neuron_count']}")
print(f"Synapses: {stats['synapse_count']}")
```

## Features

### Core Functionality

- **Config Class**: Configuration management with file I/O
- **Brain Class**: Central neural simulation brain
- **AgentBrain Class**: Interface connecting NLM brain to world
- **SimpleWorld Class**: Simple 2D world for NLM simulation
- **SensoryInput Classes**: Vision, Audio, InternalSignals for sensory processing
- **Action Class**: Motor actions for agent control
- **AgentBody Class**: Agent state representation
- **ActionResult Class**: Results from world interactions
- **SensoryPercept Class**: Combined sensory input data

### ID Types

- **NeuronId**: Unique identifier for neurons
- **SynapseId**: Unique identifier for synapses
- **RegionId**: Unique identifier for brain regions
- **PopulationId**: Unique identifier for neuron populations

### Enumeration Types

- **NeuronType**: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- **SynapseType**: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- **DevelopmentalStage**: Initial, CriticalPeriod, Maturation, Adult, Aging
- **FiringState**: Resting, Active, Refractory, Inhibited
- **ActionType**: MoveForward, TurnLeft, Look, Interact, Eat, Drink, Rest, Wait, Custom
- **MotorCommand**: MoveForward, TurnLeft, LookLeft, Interact, Wait
- **WorldObjectType**: Empty, Resource, Hazard, Wall, Marker

### Factory Functions

- **createDefaultConfig()** - Create a default configuration
- **createBrain(config)** - Create a brain with configuration
- **createSimpleWorld()** - Create a simple world
- **createAgentBrain(brain)** - Create an agent brain interface
- **create_nlm_system()** - Create a complete NLM system with defaults

### System Utilities

- **run_simulation(brain, world, steps, timestep)** - Run complete simulation
- **get_system_stats(brain)** - Get comprehensive brain statistics
- **safe_step(brain, step, time)** - Safe step with error handling
- **batch_process_sensory(brain, percepts, timestep)** - Batch sensory processing

### Enhanced Python Interfaces

- **Property-like access** for configuration values
- **Better string representations** for debugging
- **Distance methods** for spatial calculations
- **Type safety** with input validation
- **Error handling** with meaningful exceptions

## Code Organization

The Python bindings have been refactored into a modular structure:

- **bindings_utils.h** - Common utilities and error handling
- **bindings_id_types.h/.cpp** - ID type bindings
- **bindings_enums.h/.cpp** - Enumeration type bindings
- **bindings_core.h/.cpp** - Core class bindings
- **bindings_factory.h/.cpp** - Factory function bindings
- **pynlm.cpp** - Main Python module

This modular structure makes the code easier to maintain, test, and extend.

## Testing

### Unit Tests

Run comprehensive unit tests:

```bash
cd python
python test_pynlm.py
```

### Integration Tests

Run integration tests for improved features:

```bash
cd python
python test_integration.py
```

### Example

Run the complete example demonstrating all features:

```bash
cd python
python example_pynlm.py
```

## Advanced Features

### Neuromodulation System

The NLM bindings include comprehensive neuromodulation support:

```python
# Enable neuromodulation systems
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Get neuromodulation levels
neuromod_level = agent_brain.getNeuromodulationLevel()
curiosity_level = agent_brain.getCuriosityLevel()
novelty_level = agent_brain.getNoveltyLevel()
prediction_error = agent_brain.getPredictionError()
```

### Simulation Control

Run complex simulations with ease:

```python
# Run a complete simulation
body = pynlm.run_simulation(brain, world, 100, 0.1)

# Batch process multiple sensory inputs
percepts = [get_sensor_data() for _ in range(10)]
actions = pynlm.batch_process_sensory(brain, percepts, 0.01)
```

### Configuration Management

Save and load configurations:

```python
# Create and configure
config = pynlm.Config()
config.set("neuron_count", 200)
config.set("region_count", 3)

# Save to file
config.saveToFile("my_config.json")

# Load from file
config2 = pynlm.Config()
config2.loadFromFile("my_config.json")
```

## Error Handling

The improved bindings include comprehensive error handling:

```python
try:
    # Perform operations that might fail
    pynlm.get_system_stats(brain)
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Considerations

### Memory Efficiency

- Efficient cloning of Action objects
- Batch processing for reduced overhead
- Memory-efficient data structures

### Simulation Performance

- Fast step execution with error handling
- Optimized batch sensory processing
- Efficient world simulation

## Version Information

```python
import pynlm
print(f"pynlm version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"Description: {pynlm.__description__}")
```

## Dependencies

The Python bindings require:

- **Python 3.8+**
- **pybind11** (automatically found by CMake)
- **CMake 3.16+**

The bindings are built as part of the NLM project and include all necessary dependencies.

## Building from Source

### Standard Build

```bash
mkdir build
cd build
cmake ..
make -j4
```

### Python Bindings Only

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make
```

### Development Build

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Debug
make
```

## Troubleshooting

### Common Issues

**Issue: pynlm module not found**

Solution: Ensure the Python module is built and in your Python path:

```bash
# Build the module
cd build
make

# Install to Python site-packages
pip install .
```

**Issue: Missing dependencies**

Solution: The NLM Python bindings use pybind11 and are built as part of the project. Ensure you have a modern compiler and CMake.

**Issue: Compilation errors**

Solution: Check that you have the required dependencies:

```bash
# On Ubuntu/Debian
 sudo apt-get install build-essential cmake libpython3-dev

# On macOS
brew install cmake python3
```

## Migration Guide

### From Old to New API

The improved bindings maintain full backward compatibility. However, some enhancements are available:

**Old API:**
```python
# Create brain manually
config = pynlm.Config()
brain = pynlm.Brain(config)
```

**New API (easier):**
```python
# Create brain using factory function
brain = pynlm.createBrain(pynlm.Config())
```

**New Features Available:**
- Factory functions for common operations
- Enhanced error handling
- Better string representations
- System utilities and simulation helpers
- Comprehensive testing framework

## Support

For questions, issues, or enhancements:

1. Check the NLM project documentation
2. Run the example scripts to see all features in action
3. Review the comprehensive test suite
4. Consult the improvements summary for detailed feature descriptions

## License

MIT License. See the LICENSE file for details.

## Authors

NLM Authors. The Python bindings are part of the NLM (Neural Learning Machine) research project.

## Acknowledgements

This work is part of the NLM (熙然) research project, an experimental artificial developmental brain. The Python bindings enhance the usability of the NLM framework for Python users while maintaining full compatibility with the C++ core.