# pynlm - Python Interface to NLM

A Python wrapper for the NLM (Neural Learning Machine) artificial brain simulation
framework. This module provides a Pythonic interface to the high-performance C++
brain simulation engine, enabling research and experimentation with artificial
developmental systems.

This package contains the Python interface implementation for NLM. It provides:

- Core brain simulation classes and functionality
- World simulation and interaction
- Agent brain interface for perception-action loops
- Configuration system
- Exception handling
- Utility functions and debugging tools

## Overview

The package is organized into the following modules:

### brain.py
Contains the main neural simulation brain class and related functionality:
- **Brain**: Core neural computation with LIF neurons
- **Config**: Configuration system for brain parameters
- **NeuralRegion**: Brain regions with neuron populations
- **NeuralPopulation**: Groups of neurons with same type and connectivity

### world.py  
Contains world simulation and environment:
- **World**: 2D simulated environment
- **WorldObject**: Entities in the world
- **SensoryPercept**: Agent sensory input
- **ActionResult**: Action consequences

### agent.py
Contains agent brain interface:
- **AgentBrain**: Interface connecting brain to world perception and action
- **AgentBody**: Physical agent state and properties
- **MotorCommand**: Low-level motor commands

### exceptions.py
Contains NLM-specific exceptions:
- **NLMError**: Base exception
- **ConfigurationError**: Configuration errors
- **SimulationError**: Simulation errors
- And more...

### utils.py
Contains utility functions:
- **ErrorHandler**: Centralized error handling
- **DebugLogger**: Debug logging utility
- **PerformanceMonitor**: Performance tracking
- **get_version, get_build_info**: System information
- **validate_config**: Configuration validation

### types.py
Contains type definitions and enumerations:
- **NeuronId, SynapseId, RegionId, PopulationId**: Unique identifiers
- **NeuronType**: Types of neurons
- **SynapseType**: Types of synapses
- **ActionType**: Types of actions
- **MotorCommand**: Motor commands
- **WorldObjectType**: Types of world objects
- **DevelopmentalStage**: Developmental stages
- **FiringState**: Neuron firing states
- **PlasticityFlags**: Plasticity rule combinations

## Package Structure

```
pynlm/
├── __init__.py          # Package entry point and public API
├── brain.py             # Core brain simulation
├── world.py             # World simulation
├── agent.py             # Agent interface
├── exceptions.py        # Exception classes
├── utils.py             # Utility functions
├── types.py             # Type definitions
└── test_pynlm.py        # Package test suite
```

## Quick Start

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Core Classes

### Brain
Main neural simulation brain class with:
- LIF neuron dynamics
- Event-driven spike propagation
- Multiple plasticity rules (STDP, Hebbian)
- Integrated memory systems
- Prediction and cognition
- Neuromodulation
- Development stages

### World
2D simulated environment with:
- Object physics and collision detection
- Sensory perception (vision, touch, etc.)
- Action execution and consequences
- Energy management

### AgentBrain
Brain-world interface with:
- Sensory processing and transduction
- Motor command decoding
- Neuromodulation integration
- Developmental control

## Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.neuron_count = 1000
config.region_count = 2
config.connection_probability = 0.1

# Set plasticity parameters
config.stdp_ltp_weight = 0.02
config.stdp_ltd_weight = 0.012

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

## Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Performance

The package includes performance optimizations:

- **SIMD Vectorization**: Parallel processing of neural operations
- **Memory Pools**: Efficient allocation/deallocation
- **Event-Driven Architecture**: Optimized spike propagation
- **Checkpoint System**: Persistent state management

## Testing

Run the test suite:

```bash
python -m pytest test_pynlm.py -v
```

## Building

Build the package using setuptools:

```bash
python setup.py sdist bdist_wheel
```

## License

MIT License. See LICENSE file for details.

## Author

NLM Project Contributors

# Public API Exports

The following classes and functions are exported for public use:

## Brain Module

- `Brain`: Main brain simulation class
- `Config`: Configuration system
- `createBrain(config)`: Factory function to create a brain
- `createDefaultConfig()`: Factory function to create default config

## World Module

- `World`: World simulation class
- `WorldObject`: Objects in the world
- `SensoryPercept`: Sensory input data
- `ActionResult`: Action results
- `createSimpleWorld(config)`: Factory function to create a world
- `createAgentWorld(brain_agent, config)`: Factory function to create agent world

## Agent Module

- `AgentBrain`: Agent brain interface
- `AgentBody`: Agent physical state
- `MotorCommand`: Motor commands
- `createAgentBrain(brain)`: Factory function to create agent brain

## Utilities

- `NLMError`: Base exception class
- `ConfigurationError`: Configuration errors
- `SimulationError`: Simulation errors
- `MemoryError`: Memory system errors
- `PlasticityError`: Plasticity computation errors
- `SensoryError`: Sensory processing errors
- `MotorError`: Motor decoding errors
- `WorldError`: World simulation errors
- `AgentBrainError`: Agent brain errors

- `ErrorHandler`: Centralized error handling
- `DebugLogger`: Debug logging utility
- `PerformanceMonitor`: Performance tracking

- `get_version()`: Get package version
- `get_build_info()`: Get build information
- `validate_config(config_dict)`: Validate configuration
- `print_system_info()`: Print system information

## Type Definitions

- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId`: Unique identifiers
- `NeuronType`: Types of neurons
- `SynapseType`: Types of synapses
- `ActionType`: Types of actions
- `MotorCommand`: Motor commands
- `WorldObjectType`: Types of world objects
- `DevelopmentalStage`: Developmental stages
- `FiringState`: Neuron firing states
- `PlasticityFlags`: Plasticity rule combinations

## Examples

See the `examples/` directory for practical usage examples:

- `01_hello_brain.py`: Basic brain simulation
- `02_brain_world.py`: Brain-world interaction
- `03_agent_learning.py`: Complete agent learning
- `04_memory_systems.py`: Memory system usage
- `05_plasticity_experiments.py`: Plasticity experiments

## Testing

Run the test suite:

```bash
python test_pynlm.py
```

The test suite verifies:
- Module imports
- Basic functionality
- Configuration system
- World simulation
- Agent brain
- Utility functions

## Building and Distribution

The package can be built and distributed using setuptools:

```bash
# Build distribution packages
python setup.py sdist bdist_wheel

# Install from built package
pip install dist/pynlm-*.whl

# Install in development mode
pip install -e .
```

## Requirements

The package requires:
- Python >= 3.8
- pybind11 >= 2.11.0
- numpy >= 1.20

## Development

### Setting Up Development Environment

```bash
# Install development dependencies
pip install -r requirements.txt

# Install in development mode
pip install -e .
```

### Code Quality

The project follows these coding standards:
- PEP 8 code style
- Comprehensive type hints
- Detailed documentation
- Comprehensive test coverage

### Code Quality Checks

```bash
# Check code style
black --check python/
flake8 python/
mypy python/

# Format code
black python/

# Run tests
pytest test_pynlm.py -v

# Type checking
mypy python/
```

## Future Directions

### Planned Enhancements

1. **3D World Simulation**: Extension to three-dimensional environments
2. **Sensory Modalities**: Additional sensory channels (audio, proprioception)
3. **Social Learning**: Multi-agent coordination and social learning
4. **Gene Expression**: Biological-inspired gene regulatory networks
5. **Hardware Acceleration**: GPU and specialized hardware support
6. **Cloud Integration**: Distributed simulation across multiple machines

### Research Areas

- **Neuromorphic Computing**: Hardware implementations of NLM
- **Brain-Computer Interfaces**: Real-world applications
- **Cognitive Architecture**: General intelligence principles
- **Developmental Psychology**: Computational models of cognitive development

## Contributing

Contributions are welcome! Please see the `CONTRIBUTING.md` file for
contribution guidelines and development setup instructions.

## License

MIT License. See LICENSE file for details.

## Contact

- **Website**: https://nlm.org
- **Documentation**: https://docs.nlm.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Email**: contact@nlm.org

The NLM project represents a step toward creating artificial systems that
develop and learn in a manner inspired by biological brains.
