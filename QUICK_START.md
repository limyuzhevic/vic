# Enhanced NLM Python Bindings - Quick Start

This document provides a quick introduction to the enhanced NLM Python bindings.

## What Are These?

These are enhanced Python bindings for the NLM (Neural Learning Machine) framework.
They provide a Pythonic interface to the C++ neural simulation framework with:

- Context managers for automatic resource management
- Fluent API and property-style access
- Configuration builders
- Simulation runners
- Visualization helpers
- Enhanced error handling
- Type checking utilities

## Installation

### Option 1: Using pip (if already built)

```bash
pip install pynlm
```

### Option 2: Building from source

```bash
# Navigate to the workspace
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_bed2fc13-2bac-4361-969e-cf294cb5429a

# Build the Python module
mkdir build
cd build
cmake ..
make -j4

# Install (optional)
# sudo make install
```

## Getting Started

### Basic Usage

```python
import pynlm

# Create a brain with enhanced interface
with pynlm.PythonBrain(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
    brain.initialize()
    brain.step(0)
    print(f"Neurons: {brain.getNeuronCount()}")
```

### Using the Original API (Backward Compatible)

```python
import pynlm

# Original API still works (backward compatible)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
print(f"Neurons: {brain.getNeuronCount()}")
```

### Advanced Usage with Context Managers

```python
import pynlm

# Create all components with enhanced interfaces
with pynlm.PythonWorld(pynlm.createSimpleWorld()) as world:
    world.configure(20, 20, 8, 8).setStart(10, 10).reset()
    
    with pynlm.PythonBrain(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
        brain.initialize()
        
        with pynlm.PythonAgentBrain(pynlm.createAgentBrain(brain.getBrain())) as agent:
            agent.initialize(world.getWorld())
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(10):
                world.update(0.1)
                agent.processInput(world.getPercept())
                brain.step(step, world.getSimulationTime())
                action = agent.decodeCommand()
                world.applyCommand(action, world.getSimulationTime())
```

## Key Features

### 1. Context Managers

Automatic resource management with `with` statements:

```python
with pynlm.PythonBrain(brain) as brain:
    # Brain is automatically reset when exiting the context
    brain.initialize()
    # ... do work ...
# Context exit - brain is cleaned up
```

### 2. Fluent API

Method chaining for more readable code:

```python
world.configure(20, 20, 8, 8) \
     .setStart(10, 10) \
     .reset() \
     .update(0.1)
```

### 3. Property-Style Access

Easy access to brain state:

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### 4. Configuration Builders

Create common configurations easily:

```python
# Basic configuration
config = pynlm.ConfigBuilder().setBasicConfig().build()

# Advanced configuration
config = pynlm.ConfigBuilder().setAdvancedConfig().build()

# Exploration configuration
config = pynlm.ConfigBuilder().setExplorationConfig().build()

# Production configuration
config = pynlm.ConfigBuilder().setProductionConfig().build()
```

### 5. Simulation Runner

Run complete simulations with ease:

```python
runner = pynlm.SimulationRunner()
results = runner.runSimulation(brain, world, agent, numSteps=100)
```

### 6. Visualization Helpers

Create plots and analysis tools:

```python
viz = pynlm.VisualizationHelper()
plot = viz.createFiringRatePlot(times, rates)
```

### 7. Enhanced Error Handling

Custom exceptions with descriptive messages:

```python
try:
    # Do something
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

### 8. Type Checking

Validate inputs:

```python
pynlm.validateNeuronId(neuron_id)
pynlm.validateConfig(config)
pynlm.validateBrain(brain)
```

## Available Classes

### Main Classes

- **pynlm.PythonBrain**: Enhanced Brain interface with context managers
- **pynlm.PythonWorld**: Enhanced SimpleWorld interface with context managers
- **pynlm.PythonAgentBrain**: Enhanced AgentBrain interface with context managers

### Helper Classes

- **pynlm.ConfigBuilder**: Configuration builder for common setups
- **pynlm.SimulationRunner**: Helper for running complete simulations
- **pynlm.VisualizationHelper**: Visualization and data analysis helpers

### Exception Classes

- **pynlm.NLMException**: Base exception class
- **pynlm.ConfigurationError**: Configuration-specific exceptions
- **pynlm.SimulationError**: Simulation-specific exceptions
- **pynlm.InitializationError**: Initialization-specific exceptions

## Available Functions

### Factory Functions

- **pynlm.createBrain(config)**: Create a Brain object
- **pynlm.createDefaultConfig()**: Create a default Config object
- **pynlm.createSimpleWorld()**: Create a SimpleWorld object
- **pynlm.createAgentBrain(brain)**: Create an AgentBrain object

### Helper Functions

- **pynlm.validateNeuronId(id)**: Validate a NeuronId
- **pynlm.validateSynapseId(id)**: Validate a SynapseId
- **pynlm.validateRegionId(id)**: Validate a RegionId
- **pynlm.validatePopulationId(id)**: Validate a PopulationId
- **pynlm.validateConfig(config)**: Validate a Config object
- **pynlm.validateBrain(brain)**: Validate a Brain object
- **pynlm.validateWorld(world)**: Validate a SimpleWorld object
- **pynlm.validateAgent(agent)**: Validate an AgentBrain object

### Constants

- **pynlm.VERSION**: Module version ("0.1.0")
- **pynlm.API_VERSION**: API version ("enhanced")
- **pynlm.INVALID_NEURON_ID**: Invalid neuron ID
- **pynlm.INVALID_SYNAPSE_ID**: Invalid synapse ID
- **pynlm.INVALID_REGION_ID**: Invalid region ID
- **pynlm.INVALID_POPULATION_ID**: Invalid population ID

## Examples Directory

See the `examples/` directory for comprehensive example scripts:

- `enhanced_api_example.py`: Complete examples of all features
- `basic_example.py`: Simple usage examples
- `simulation_example.py`: Simulation examples
- `configuration_example.py`: Configuration examples

## Testing

Run the basic tests:

```bash
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_bed2fc13-2bac-4361-969e-cf294cb5429a
python test_basic.py
```

Run the enhanced examples:

```bash
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_bed2fc13-2bac-4361-969e-cf294cb5429a
python examples/enhanced_api_example.py
```

## Building from Source

### Prerequisites

- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 development headers

### Build Instructions

```bash
# Create build directory
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_bed2fc13-2bac-4361-969e-cf294cb5429a
mkdir build
cd build

# Configure CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build (using multiple cores for faster compilation)
make -j4
```

### Python Package Installation

After building, you can install the Python module:

```bash
cd build
pip install -e .
```

Or install directly to system:

```bash
sudo make install
```

## Troubleshooting

### Common Issues

#### 1. "ModuleNotFoundError: No module named 'pynlm'"

Make sure you're in the correct directory and the module is built:

```bash
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_bed2fc13-2bac-4361-969e-cf294cb5429a
# Make sure the python module is in your PYTHONPATH
export PYTHONPATH=/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_bed2fc13-2bac-4361-969e-cf294cb5429a/python:$PYTHONPATH
```

#### 2. Import errors

If you get import errors, check that:

1. The module is properly built
2. All dependencies are installed
3. You're using the correct Python environment

#### 3. Build errors

If you encounter build errors:

1. Check that all dependencies are installed
2. Ensure CMake version is compatible
3. Check that pybind11 is available

### Getting Help

- Check the `Enhanced_Bindings_Documentation.md` file for detailed documentation
- Look at the example scripts in the `examples/` directory
- Check the `README.md` file in the main directory
- Consult the project documentation online

## Technical Details

### Module Structure

The enhanced bindings are organized in the `python/` directory:

- `__init__.py`: Main module with documentation and public API
- `bindings.cpp`: Enhanced C++ bindings with Pythonic interfaces
- `CMakeLists.txt`: Build configuration for the Python module

### Key Improvements

1. **Context Managers**: Automatic resource management
2. **Fluent API**: Method chaining for readability
3. **Property Access**: Easy access to brain state
4. **Configuration Builders**: Factory methods for common setups
5. **Simulation Runner**: Complete simulation management
6. **Visualization Helpers**: Data analysis tools
7. **Error Handling**: Custom exceptions with descriptive messages
8. **Type Checking**: Input validation utilities
9. **Unicode Support**: String encoding helpers
10. **Backward Compatibility**: Original API still works

### C++ Changes

The enhanced bindings modify the original `bindings.cpp` file with:

- Additional wrapper classes for Pythonic interfaces
- Enhanced documentation with examples
- Custom exception handling
- Type checking utilities
- Context manager support
- Configuration builders
- Simulation runners
- Visualization helpers

### Version Information

```python
import pynlm
print(f"Version: {pynlm.__version__}")  # "0.1.0"
print(f"API Version: {pynlm.__api_version__}")  # "enhanced"
```

## Support

For support with the enhanced NLM Python bindings:

- Check the documentation in this directory
- Review the example scripts
- Check the project issues and documentation
- Consult the NLM project team

## License

This project is licensed under the MIT License.

See `LICENSE` file for more information.

## Copyright

© 2024 NLM Authors

All rights reserved.
